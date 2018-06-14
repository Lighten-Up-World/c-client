#include "opc_client.h"

opc_header_t *opc_header_init(uint8_t channel, uint8_t command, uint8_t length) {





}

uint8_t *opc_header_data(opc_header_t *self);

int opc_connect(opc_client_t* self);

int opc_close(opc_client_t* self);

int opc_resolve(opc_client_t* self, const char *hostport, int defaultPort);

int opc_write(const uint8_t *data, size_t length);