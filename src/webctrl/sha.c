#include <printf.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <openssl/sha.h>
#include "ctrlserver.h"

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <stdint.h>

// TODO: figure out how this works and implement error checking?
// https://gist.github.com/barrysteyn/7308212

int Base64Encode(const unsigned char* buffer, size_t length, char** b64text) { //Encodes a binary safe base 64 string
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

int main() {
  char *in = "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n";

  // Extract client hash
  char *key_header_start = strstr(in, WEBSOCKET_KEY_HEADER);
  if (key_header_start == NULL) {
    perror("Sec-WebSocket-Key not present");
    exit(errno);
  }

  char *key_start = strchr(key_header_start, ' ') + 1;
  if (key_start == NULL || key_start != key_header_start + strlen(WEBSOCKET_KEY_HEADER)) {
    perror("Sec-WebSocket-Key header is malformed (key start)");
    exit(errno);
  }

  char *key_end = strchr(key_start, '\r');
  if (key_end == NULL) {
    perror("Sec-WebSocket-Key header is malformed (key end)");
    exit(errno);
  }

  // Append magic string to the client provided hash, then take the sha1 hash to send in response
  size_t key_len = key_end - key_start;
  char *key = calloc(key_len + strlen(SEC_WEBSOCKET_MAGIC), sizeof(char));
  memcpy(key, key_start, key_len);
  memcpy(key + key_len, SEC_WEBSOCKET_MAGIC, strlen(SEC_WEBSOCKET_MAGIC));

  unsigned char *hash = calloc(20, sizeof(char));
  SHA1((const unsigned char *)key, strlen((const char *) key), hash);

  hash[20] = '\0';
  printf("hash: %s\n", hash);

  // base64 encode the output
  char *s2;
  Base64Encode(hash, strlen((const char *) hash), &s2);

  printf("base64: %s\n", s2);

  // should return s3pPLMBiTxaQ9kYGzzhZRbK+xOo=
  char *s1 = "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=";
  //char *s2 = (char *) b64;
  if (strncmp(s1, s2, strlen(s1)) == 0) {
    puts("NAILED IT");
  }

  return 0;
}

