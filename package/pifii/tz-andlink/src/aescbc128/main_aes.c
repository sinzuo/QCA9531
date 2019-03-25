#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "base64.h"
#include "aes.h"
#include "main_aes.h"

char g_iv_en[] = "0000000000000000";
char g_iv_de[] = "0000000000000000";

void print_hex(unsigned char *buf,size_t len)
{
    for(size_t i = 0;i < len ;i++)
    {
        printf("0x%02X",buf[i]);
        if(i > 0 && 0 == (i%(BLOCK_SIZE-1)))
        {
            printf("\n");
        }
    }
}

//PKCS5
static unsigned int en_padding_op(unsigned char *ibuf, 
                                  int ilen, 
                                  int blksize){
    unsigned int i;    /* loop counter*/
    unsigned char pad; /* pad character (calculated)*/
    unsigned char *p;  /*pointer to end of data*/

    if (0 == (ilen % blksize))
    {
        pad = (unsigned char)blksize;

    }else
    {
        pad = (unsigned char)(blksize - (ilen % blksize));
    }
    p = ibuf + ilen;
    for (i = 0; i < (int) pad; i++)
    {
        *p = pad;
        ++p;
    }

    return (ilen + pad);
}

static unsigned int de_padding_op(unsigned char *ibuf){
    if(NULL == ibuf)
    {
        return 0;
    }
    size_t ilen = strlen(ibuf);
    if( 0 == ilen % BLOCK_SIZE)
    {
        unsigned char pad = ibuf[ilen-1];
        ilen = ilen - pad; 
        ibuf[ilen] = 0x00;
    }
    return ilen;   
}

int aes_cbc128_encrypt(const unsigned char *in, 
                       const unsigned char *key,
                       unsigned char *out, 
                       size_t outlen /*out max len*/){
    AES_KEY aes_key;
    int result = AES_OK;
    unsigned char *encrypt_buffer = NULL;
    unsigned char *base64_encode_buffer = NULL;
    unsigned char *in_pad_buffer = NULL;
    size_t in_pad_len = 0;

    if ((NULL == in) || (NULL == out))
    {
        return AES_FAIL;
    }

    in_pad_buffer = (unsigned char *)calloc(1, strlen(in) + BLOCK_SIZE + 1);
    if (NULL == in_pad_buffer)
    {
        return AES_FAIL;
    }
    memset(in_pad_buffer, '\0', strlen(in) + BLOCK_SIZE + 1);
    memcpy(in_pad_buffer, in, strlen(in));
    //printf("Content:%s\n",in_pad_buffer);

    in_pad_len = en_padding_op(in_pad_buffer, strlen(in), BLOCK_SIZE);
    //printf("in_pad_buffer:");
    //print_hex(in_pad_buffer,in_pad_len);
    result = AES_set_encrypt_key(key, AES_KEY_SIZE, &aes_key);
    if (AES_OK != result)
    {
        free(in_pad_buffer);
        return AES_FAIL;
    }

    encrypt_buffer = (unsigned char *)calloc(in_pad_len, sizeof(unsigned char));
    if (NULL == encrypt_buffer)
    {
        free(in_pad_buffer);
        return AES_FAIL;
    }

    //printf("AES IV: %s\n", g_iv_en);
    AES_cbc_encrypt(in_pad_buffer, encrypt_buffer, in_pad_len, &aes_key, g_iv_en, AES_ENCRYPT);
    //print_hex(encrypt_buffer,in_pad_len);
    size_t base64_encode_len = (in_pad_len / 3 + (in_pad_len % 3 == 0 ? 0:1)) * 4 + 1;
    base64_encode_buffer = (unsigned char *)calloc(1,base64_encode_len);
    base64_encode(base64_encode_buffer,encrypt_buffer,in_pad_len,base64_encode_len);
    //printf("Output (base64): %s\n", base64_encode_buffer);

    if (NULL == base64_encode_buffer)
    {
        free(encrypt_buffer);
        free(in_pad_buffer);
        return AES_FAIL;
    }
    snprintf(out, outlen, "%s", base64_encode_buffer);

    free(in_pad_buffer);
    free(encrypt_buffer);
    free(base64_encode_buffer);

    return result;
}

int aes_cbc128_decrypt(const unsigned char *in, 
                       const unsigned char *key,
                       unsigned char *out, 
                       size_t outlen){
    int result = AES_OK;
    AES_KEY aes_key;

    if ((NULL == in) || (NULL == out))
    {
        return AES_FAIL;
    }
    char* base64_decode_buffer = NULL;
    //int base64_decode_len = Base64Decode(in, strlen(in), &base64_decode_buffer);

    size_t base64_decode_len = strlen(in) / 4 * 3 + 1;
    base64_decode_buffer = (unsigned char *)calloc(base64_decode_len, sizeof(unsigned char));
    int res_base64 = base64_decode(base64_decode_buffer,in,strlen(in),base64_decode_len);
    //print_hex(base64_decode_buffer,base64_decode_len);
    if(0 >= res_base64){
        return AES_FAIL;
    }
    base64_decode_len = res_base64;
    result = AES_set_decrypt_key(key, AES_KEY_SIZE, &aes_key);
    //print_hex((unsigned char*)aes_key.rd_key,16);
    if (AES_OK != result)
    {
        return AES_FAIL;
    }
    //printf("AES IV: %s\n", g_iv_de);
    AES_cbc_encrypt(base64_decode_buffer, out,\
         base64_decode_len < outlen ? base64_decode_len : outlen,\
         &aes_key, g_iv_de, AES_DECRYPT);
    //printf("out pad data:");
    //print_hex(out,strlen(out));
    de_padding_op(out);
    free(base64_decode_buffer);
    return result;
}
/*
int main(int argc, char **argv)
{
    if ( argc <= 1 || argc > 2){
        printf("usage:\n");
        printf("    %s plaintext\n", argv[0]);
        return -1;
    }
    static unsigned char key[] = "1234567812345678";
    unsigned char encrypt_data[1024] = {0};
    unsigned char decrypt_data[1024] = {0};

    if( AES_FAIL == aes_cbc128_encrypt(argv[1],key, encrypt_data, sizeof(encrypt_data))){
        printf("fail encrypt\n");
        return -1;
    }
    printf("encrypt result: %s\n", encrypt_data);

    if( AES_FAIL == aes_cbc128_decrypt(encrypt_data,key, decrypt_data, sizeof(decrypt_data))){
        printf("fail decrypt\n");
        return -1;
    }   
    printf("decrypt length: %d\n", strlen(decrypt_data));
    printf("decrypt result: %s\n", decrypt_data);
}
*/
