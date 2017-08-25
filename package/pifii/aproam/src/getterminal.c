#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <getopt.h>

int main(int argc,const char**argv)
{
    struct option long_opts[] = {
                {"thredshold", 1, NULL, 't'},
                {NULL, 0, NULL, 0}
        };
    char thstr[4]="\0";
    int c;
    int32_t th = -95;
    while (1)
    {
        c = getopt_long(argc, argv, "t:", long_opts, NULL);
        if (c == EOF)
            break;
        switch (c)
        {
            case 't':
                 memset(thstr,0,4); 
                 memcpy(thstr,optarg,3);
                 th = atoi(thstr);
                 break;
            default:
                 fprintf(stderr, "error while parsing options\n");
                 return -1;
        }
    }
    th = th >=0 ? -95:th; 
    //printf("thredshold:%d\n",th);
    FILE *fp;
    if(NULL == (fp=fopen("/tmp/mac","r")))
    {   
        fprintf(stderr,"error:/tmp/mac file cannot be opened\n");
        return -1;
    }
    uint8_t mactable[1400];
    uint8_t *data=NULL;
    int32_t dbm=-100; 
    int i=0;
    uint32_t size=fread(mactable,1,1400,fp);
    fclose(fp);
    for(i=0;i<size;i=i+7) 
    {
        data=mactable + i;
        dbm=data[6]-95;
        if(th < dbm)
        {
            printf("%02X:%02X:%02X:%02X:%02X:%02X %d\n",data[0],data[1],data[2],data[3],data[4],data[5],dbm);
        }
    }
    return 0;
}
