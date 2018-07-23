#include<stdio.h>
typedef unsigned char BYTE;
int main(int argc, char* argv[])
{
    unsigned int num,*p;
    p = &num;
    num = 0;
    *(BYTE *)p = 0xff;
    if(num == 0xff)
    {
        printf("The endian of cpu is little\n");
    }
    else //num == 0xff000000
    {
        printf("The endian of cpu is big\n");
    }
    return 0;
}
