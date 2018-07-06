#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GRID_WIDTH 52
#define GRID_HEIGHT 24
#define TOTAL_PIXELS 471

#define X_OFFSET -27
#define Y_OFFSET -1

#define LINE_SIZE 2048

#define SPACING ((double)0.1)


void read_csv(int row, int col, char *filename, double **data){
	FILE *file;
	file = fopen(filename, "r");

	int i = 0;
    char line[4098];
	while (fgets(line, 4098, file) && (i < row))
    {
    	// double row[ssParams->nreal + 1];
        char* tmp = strdup(line);

	    int j = 0;
	    const char* tok;
	    for (tok = strtok(line, "\t"); tok && *tok; j++, tok = strtok(NULL, "\t\n"))
	    {
	        data[i][j] = atof(tok);
	        printf("%f\t", data[i][j]);
	    }
	    printf("\n");

        free(tmp);
        i++;
    }
}

int enumLine(char** buff, char* line, int y)
{
    const char* tok;
    int x = 0;
    for (tok = strtok(line, ",");
            tok && *tok;
            tok = strtok(NULL, ",\n")){
      char sep[4] = ",\n";
      if(strcmp(tok, "1") == 0){
        printf("(%d,%d)\n", x, y);
        if(y+Y_OFFSET == 0 && x == 0){
          sep[0] = '{';
        }
        char *lastBuff = *buff;
        asprintf(buff, "%s%s\t{\"point\": [%.2f, 0, %.2f]}", lastBuff, sep,
                (x+X_OFFSET)*SPACING, (y+Y_OFFSET)*SPACING);
        free(lastBuff);
      }
      else{
        printf("SKIPPED\n");
      }
      x++;
    }
    return 1;
}

int main(int argc, char **argv){
	int row     = 1;
	int col     = 1;
	char fname[256] = "WorldMap.csv";

	double **data;
	data = (double **)malloc(row * sizeof(double *));
	for (int i = 0; i < row; ++i){
		data[i] = (double *)malloc(col * sizeof(double));
	}

	read_csv(row, col, fname, data);

	return 0;

  FILE* stream = fopen("WorldMap.csv", "r");

    char line[LINE_SIZE];
    char *pixels = calloc(4, LINE_SIZE);
    int y = 0;
    while (fgets(line, LINE_SIZE, stream))
    {
        char* tmp = strdup(line);

        enumLine(&pixels, tmp, y);
        y++;
        free(tmp);
    }
    char *lastBuff = pixels;
    asprintf(&pixels, "%s\n}", lastBuff);
    free(lastBuff);

  return 0;
}
