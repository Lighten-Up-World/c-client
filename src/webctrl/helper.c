// Contains all functions not now needed, but may be shortly

#include "ctrlserver.h"

// TODO: add error checking
char *extract_last_cmd(ctrl_server *server) {
  // Extract last command from buffer, ignore everything before
  char *last = strrchr(server->buffer, CMD_TERMINATOR);
  char *prev = server->buffer - 1;
  char *next = strchr(server->buffer, CMD_TERMINATOR);
  while (strcmp(next, last) != 0) {
    prev = next;
    next = strchr(next + 1, CMD_TERMINATOR);
  }
  return prev + 1;
}

