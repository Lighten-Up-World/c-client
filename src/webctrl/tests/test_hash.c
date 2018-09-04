#include <printf.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <openssl/sha.h>

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <stdint.h>
#include "../ctrlserver.h"

// TODO: figure out how this works and implement error checking?
// https://gist.github.com/barrysteyn/7308212

int Base64Encode(const unsigned char* buffer, int length, char** b64text) { //Encodes a binary safe base 64 string
  BIO *bio, *b64;
  BUF_MEM *bufferPtr;

  b64 = BIO_new(BIO_f_base64());
  bio = BIO_new(BIO_s_mem());
  bio = BIO_push(b64, bio);

  BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line
  BIO_write(bio, buffer, length);
  BIO_flush(bio);
  BIO_get_mem_ptr(bio, &bufferPtr);
  BIO_set_close(bio, BIO_NOCLOSE);
  BIO_free_all(bio);

  *b64text=bufferPtr->data;

  return 0;
}

void calc_hash(char *key) {
  // Append magic string to the client provided hash, then take the sha1 hash to send in response
  char *to_hash = calloc(strlen(key) + strlen(SEC_WEBSOCKET_MAGIC), sizeof(char));
  if (to_hash == NULL) {
    perror("Hash--alloc");
    exit(errno);
  }
  strncpy(to_hash, key, strlen(key));
  strncat(to_hash, SEC_WEBSOCKET_MAGIC, strlen(SEC_WEBSOCKET_MAGIC));

  unsigned char *sha1 = calloc(SHA1_HASH_LEN + 1, sizeof(char));
  if (sha1 == NULL) {
    perror("Hash alloc");
    exit(errno);
  }
  sha1 = SHA1((const unsigned char *)to_hash, strlen((const char *) to_hash), sha1);
  sha1[SHA1_HASH_LEN] = '\0'; // needed for debug only

  // Zero the buffer
  char *base64 = calloc(29, sizeof(char));
  Base64Encode((const unsigned char *) sha1, SHA1_HASH_LEN, &base64);
  printf("base64: %s\n", base64);
}

int main() {
  FILE *f = fopen("/Users/Matt/Projects/ARM Project/c-client/src/webctrl/tests/keys_filt.txt", "r");

  size_t len = strlen("3Q7rCgkquNBD3DgkdYDl5Q==");
  char *key = calloc(len + 2, sizeof(char)); // account for '\r\n' at end of line
  char *hash = calloc(21, sizeof(char));
  if (key == NULL || hash == NULL) {
    perror("hash, key");
    exit(errno);
  }
  int n = 0;
  while (fgets(key, (int) (len + 3), f) != NULL && n < 1) {
    //overwrite '\n' with '\0'
    key[len] = '\0';
    calc_hash(key);
    n++;
  }

  fclose(f);

  return 0;
}

