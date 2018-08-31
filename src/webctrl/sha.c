#include <printf.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <openssl/sha.h>
#include "ctrlserver.h"

int main() {
  char *in = "Sec-WebSocket-Key: keyherelololol\r\n";

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
  for (int i = 0; i < 20; i++) {
    printf("%02x", hash[i]);
  }
  puts("");


  return 0;
}

