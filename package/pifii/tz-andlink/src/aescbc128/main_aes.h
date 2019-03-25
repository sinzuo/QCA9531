#include <stdio.h>
#include <string.h>
#include "base64.h"
#include "aes.h"

#define BLOCK_SIZE                 (AES_BLOCK_SIZE)                  
#define AES_KEY_SIZE               (128)                 

#define AES_FAIL (-1)
#define AES_OK (0)

void print_hex(unsigned char *buf,size_t len);

//PKCS5
static unsigned int en_padding_op(unsigned char *ibuf, 
                                  int ilen, 
                                  int blksize);

static unsigned int de_padding_op(unsigned char *ibuf);

int aes_cbc128_encrypt(const unsigned char *in, 
                       const unsigned char *key,
                       unsigned char *out, 
                       size_t outlen /*out max len*/);

int aes_cbc128_decrypt(const unsigned char *in, 
                       const unsigned char *key,
                       unsigned char *out, 
                       size_t outlen);

