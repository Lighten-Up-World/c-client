#ifndef ARM11_22_SERVER_CRYPTO_H
#define ARM11_22_SERVER_CRYPTO_H

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <stdint.h>
#include <stdio.h>

int Base64Encode(const unsigned char* buffer, size_t length, char** b64text);

#endif //ARM11_22_SERVER_CRYPTO_H
