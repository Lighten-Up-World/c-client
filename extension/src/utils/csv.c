#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "csv.h"

int csv_parser_delimiterIsAccepted(const char *delimiter) {
    char actualDelimiter = *delimiter;
    if (actualDelimiter == '\n' || actualDelimiter == '\r' || actualDelimiter == '\0' ||
            actualDelimiter == '\"') {
        return 0;
    }
    return 1;
}

void csv_parser_setErrorMessage(csv_parser_t *csvParser, const char *errorMessage) {
    if (csvParser->errMsg != NULL) {
        free(csvParser->errMsg);
    }
    int errMsgLen = strlen(errorMessage);
    csvParser->errMsg = (char*)malloc(errMsgLen + 1);
    strcpy(csvParser->errMsg, errorMessage);
}

csv_parser_t *csv_parser_new(const char *path, const char *delimiter){
    csv_parser_t *csvParser = (csv_parser_t*)malloc(sizeof(csv_parser_t));
    if (path == NULL) {
        csvParser->path = NULL;
    } else {
        int pathLen = strlen(path);
        csvParser->path = (char*)malloc((pathLen + 1));
        strcpy(csvParser->path, path);
    }
    csvParser->errMsg = NULL;
    if (delimiter == NULL) {
        csvParser->delimiter = ',';
    } else if (csv_parser_delimiterIsAccepted(delimiter)) {
        csvParser->delimiter = *delimiter;
    } else {
        csvParser->delimiter = '\0';
    }
    csvParser->header = NULL;
    csvParser->file = NULL;
	csvParser->from_string = 0;
	csvParser->csv_string = NULL;
	csvParser->csv_stringIter = 0;

    return csvParser;
}

csv_parser_t *csv_parser_new_from_string(const char *csv_string, const char *delimiter) {
	csv_parser_t *csvParser = csv_parser_new(NULL, delimiter);
	csvParser->from_string = 1;
	if (csv_string != NULL) {
		int csv_stringLen = strlen(csv_string);
		csvParser->csv_string = (char*)malloc(csv_stringLen + 1);
		strcpy(csvParser->csv_string, csv_string);
	}
	return csvParser;
}

void csv_parser_destroy(csv_parser_t *csvParser) {
    if (csvParser == NULL) {
        return;
    }
    if (csvParser->path != NULL) {
        free(csvParser->path);
    }
    if (csvParser->errMsg != NULL) {
        free(csvParser->errMsg);
    }
    if (csvParser->file != NULL) {
        fclose(csvParser->file);
    }
    if (csvParser->header != NULL) {
        csv_parser_destroy_row(csvParser->header);
    }
	if (csvParser->csv_string != NULL) {
		free(csvParser->csv_string);
	}
    free(csvParser);
}

void csv_parser_destroy_row(csv_row_t *csvRow) {
    int i;
    for (i = 0 ; i < csvRow->size ; i++) {
        free(csvRow->fields[i]);
    }
    free(csvRow);
}

int csv_parser_getNumFields(csv_row_t *csvRow) {
    return csvRow->size;
}

char **csv_parser_getFields(csv_row_t *csvRow) {
    return csvRow->fields;
}

csv_row_t *csv_parser_getRow(csv_parser_t *csvParser) {
    int numRowRealloc = 0;
    int acceptedFields = 64;
    int acceptedCharsInField = 64;
    if (csvParser->path == NULL && (! csvParser->from_string)) {
        csv_parser_setErrorMessage(csvParser, "Supplied CSV file path is NULL");
        return NULL;
    }
    if (csvParser->csv_string == NULL && csvParser->from_string) {
        csv_parser_setErrorMessage(csvParser, "Supplied CSV string is NULL");
        return NULL;
    }
    if (csvParser->delimiter == '\0') {
        csv_parser_setErrorMessage(csvParser, "Supplied delimiter is not supported");
        return NULL;
    }
    if (! csvParser->from_string) {
        if (csvParser->file == NULL) {
            csvParser->file = fopen(csvParser->path, "r");
            if (csvParser->file == NULL) {
                int errorNum = errno;
                const char *errStr = strerror(errorNum);
                char *errMsg = (char*)malloc(1024 + strlen(errStr));
                strcpy(errMsg, "");
                sprintf(errMsg, "Error opening CSV file for reading: %s : %s", csvParser->path, errStr);
                csv_parser_setErrorMessage(csvParser, errMsg);
                free(errMsg);
                return NULL;
            }
        }
    }
    csv_row_t *csvRow = (csv_row_t*)malloc(sizeof(csv_row_t));
    csvRow->fields = (char**)malloc(acceptedFields * sizeof(char*));
    csvRow->size = 0;
    int fieldIter = 0;
    char *currField = (char*)malloc(acceptedCharsInField);
    int inside_complex_field = 0;
    int currFieldCharIter = 0;
    int seriesOfQuotesLength = 0;
    int lastCharIsQuote = 0;
    int isEndOfFile = 0;
    while (1) {
        char currChar = (csvParser->from_string) ? csvParser->csv_string[csvParser->csv_stringIter] : fgetc(csvParser->file);
        csvParser->csv_stringIter++;
        int endOfFileIndicator;
        if (csvParser->from_string) {
            endOfFileIndicator = (currChar == '\0');
        } else {
            endOfFileIndicator = feof(csvParser->file);
        }
        if (endOfFileIndicator) {
            if (currFieldCharIter == 0 && fieldIter == 0) {
                csv_parser_setErrorMessage(csvParser, "Reached EOF");
                return NULL;
            }
            currChar = '\n';
            isEndOfFile = 1;
        }
        if (currChar == '\r') {
            continue;
        }
        if (currFieldCharIter == 0  && ! lastCharIsQuote) {
            if (currChar == '\"') {
                inside_complex_field = 1;
                lastCharIsQuote = 1;
                continue;
            }
        } else if (currChar == '\"') {
            seriesOfQuotesLength++;
            inside_complex_field = (seriesOfQuotesLength % 2 == 0);
            if (inside_complex_field) {
                currFieldCharIter--;
            }
        } else {
            seriesOfQuotesLength = 0;
        }
        if (isEndOfFile || ((currChar == csvParser->delimiter || currChar == '\n') && ! inside_complex_field) ){
            currField[lastCharIsQuote ? currFieldCharIter - 1 : currFieldCharIter] = '\0';
            csvRow->fields[fieldIter] = (char*)malloc(currFieldCharIter + 1);
            strcpy(csvRow->fields[fieldIter], currField);
            free(currField);
            csvRow->size++;
            if (currChar == '\n') {
                return csvRow;
            }
            if (csvRow->size != 0 && csvRow->size % acceptedFields == 0) {
                csvRow->fields = (char**)realloc(csvRow->fields, ((numRowRealloc + 2) * acceptedFields) * sizeof(char*));
                numRowRealloc++;
            }
            acceptedCharsInField = 64;
            currField = (char*)malloc(acceptedCharsInField);
            currFieldCharIter = 0;
            fieldIter++;
            inside_complex_field = 0;
        } else {
            currField[currFieldCharIter] = currChar;
            currFieldCharIter++;
            if (currFieldCharIter == acceptedCharsInField - 1) {
                acceptedCharsInField *= 2;
                currField = (char*)realloc(currField, acceptedCharsInField);
            }
        }
        lastCharIsQuote = (currChar == '\"') ? 1 : 0;
    }
}


const char *csv_parser_getErrorMessage(csv_parser_t *csvParser) {
    return csvParser->errMsg;
}
