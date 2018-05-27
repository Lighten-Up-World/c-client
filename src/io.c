#include <stdio.h>
#include <stdlib.h>
#include "arm.h"

/**
* Writes the entire file into the buffer (or throws an error)
*
* @param path - A string to the binary file to read.
* @param buffer - A pointer to an allocated array which it gets stored
* @param buffer_size - the size of buffer allocated.
* @return a status code for the result
*/
int writeFile(const char *path, byte_t *buffer, size_t buffer_size){
  FILE* fp = fopen(path, "wb");
  if(fp == NULL){
    perror("fopen failed at path");
    return 1;
  }
  const int read = fwrite(buffer, buffer_size, 1, fp);
  if(read != buffer_size && ferror(fp)){
    perror("Couldn't write file to completion");
    return 3;
  }
  if(fclose(fp) != 0){
    perror("Couldn't close file");
    return 4;
  }
  return 0;
}

/**
* Loads the entire file into the buffer (or throws an error)
*
* @param path - A string to the binary file to read.
* @param buffer - A pointer to an allocated array which it gets stored
* @param buffer_size - the size of buffer allocated.
* @return a status code for the result
*/
int readFile(const char *path, byte_t *buffer, size_t buffer_size){
  long file_size = 0;
  FILE* fp = fopen(path, "rb");
  if(fp == NULL){
    perror("fopen failed at path");
    return 1;
  }
  file_size = ftell(fp);
  if(file_size == -1){
    perror("Couldn't determine file size");
    return 2;
  }
  const int read = fread(buffer, buffer_size, 1, fp);
  if(read != file_size && ferror(fp)){
    perror("Couldn't read file to completion");
    return 3;
  }
  if(fclose(fp) != 0){
    perror("Couldn't close file");
    return 4;
  }
  return 0;
}

/**
* Loads a single byte from the address given in the binary file.
*
* @param path - A string to the binary file to read.
* @param byte - A pointer to a byte
* @param address - the address at which the byte is read from
* @return a status code for the result
*/
int readByte(const char *path, byte_t *byte, address_t address){
  FILE *fp = fopen(path, "rb");
  if(fp == NULL){
    perror("fopen failed at path");
    return 1;
  }
  if(fseek(fp, address, SEEK_SET) != 0){
    perror("fseek failed to get address");
    return 2;
  }
  *byte = fgetc(fp);
  if(fclose(fp) != 0){
    perror("Couldn't close file");
    return 4;
  }
  return 0;
}

/**
* Loads a single word from the address given in the binary file.
*
* @param path - A string to the binary file to read.
* @param byte - A pointer to a word
* @param address - the address at which the byte is read from
* @return a status code for the result
*/
int readWord(const char *path, word_t *word, address_t address){
  FILE *fp = fopen(path, "rb");
  if(fp == NULL){
    perror("fopen failed at path");
    return 1;
  }
  if(fseek(fp, address, SEEK_SET) != 0){
    perror("fseek failed to get address");
    return 2;
  }
  fread(word, sizeof(word_t) / sizeof(byte_t), sizeof(byte_t), fp);
  if(fclose(fp) != 0){
    perror("Couldn't close file");
    return 4;
  }
  return 0;
}

int main(void){
  byte_t *buffer = malloc(MEM_SIZE);
  if(buffer == NULL){
    return EXIT_FAILURE;
  }
  readFile("../test/test_cases/add01", buffer, MEM_SIZE);
  printf("[ ");
  for(size_t i = 0; i < MEM_SIZE; i++)
  {
    if(buffer[i]==0){
      break;
    }
    printf("%02x ", buffer[i]);
    //ompare with xxd
  }
  printf("]\n");
}
