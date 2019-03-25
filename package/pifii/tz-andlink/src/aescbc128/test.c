#include <stdio.h>
#include <string.h>
#include "main_aes.h"

int main(int argc,const char **argv)
{
    if (6 != argc || (0 != memcmp(argv[1],"-e",2) && 0 != memcmp(argv[1],"-d",2))
        || 0 != memcmp(argv[2],"-k",2) 
        || 0 != memcmp(argv[4],"-v",2)){
        printf("usage:\n");
        printf("    %s [-e|-d] [-k] [-v]\n", argv[0]);
        printf("examples:\n");
        printf("    encrypt:  %s -e -k 1234567812345678 -v abcdefg\n", argv[0]);
        printf("    encrypt:  %s -d -k 1234567812345678 -v GYizV+IAvmnri/lCMd1dIw==\n", argv[0]);
        return -1;
    }
    char data[4096] = {0};

    if (0 == memcmp(argv[1],"-e",2)){
        if( AES_FAIL == aes_cbc128_encrypt((unsigned char*)argv[5],
                                           (unsigned char*)argv[3], 
                                           data, sizeof(data))){
            printf("fail encrypt\n");
            return -1;
        }
        printf("%s\n", data);
    } else{
        if( AES_FAIL == aes_cbc128_decrypt((unsigned char*)argv[5],
                                           (unsigned char*)argv[3],
                                           data, sizeof(data))){
            printf("fail decrypt\n");
            return -1;
        }
        printf("%s\n", data);
    }
    return 0;
}

