#include"promac.h"

stumac_t *tomac(const char *pszStr,stumac_t *pmac)
{
    char * pszTmpStr = NULL;    
    if(NULL == pszStr || NULL == pmac) 
    {
        return NULL; 
    }
    if( STRLEN_12 != strlen(pszStr) && STRLEN_17 != strlen(pszStr))
    {
        printf("error:mac string size is not 12 or 17\n");
        return NULL; 
    }
    else if(STRLEN_17 == strlen(pszStr))
    {
        pszTmpStr = (char*)malloc(STRLEN_12); 
        int i = 0,j = 0;
        while(i < STRLEN_12 && j < STRLEN_17)
        {
           if(':' != pszStr[j])
           {
              pszTmpStr[i] =  pszStr[j];
              i++;
           }
           j++;
        }
        
    }
    else 
    {
        pszTmpStr = (char*)malloc(STRLEN_12); 
        strncpy(pszTmpStr,pszStr,STRLEN_12);
    }
    int iLoop = 0; 
    unsigned char *pszTmp = (unsigned char*)malloc(MACLEN_6); 
    bzero(pszTmp,6);
    char *pLoop = pszTmpStr;
    char *pLoopTwo = pLoop + 1;
    while(iLoop < STRLEN_12)
    {
        pLoop = pszTmpStr + iLoop;
        pLoopTwo = pLoop + 1;
        if(48 <= *pLoop && 57 >= *pLoop) //0~9 
        {
            pszTmp[iLoop/2] = (*pLoop - 48) << 4; 
        }
        else if(65 <= *pLoop && 70 >= *pLoop) //A~F
        {
            pszTmp[iLoop/2] = (*pLoop - 55) << 4; 
        }
        else if(97 <= *pLoop && 102 >= *pLoop) //a~f
        { 
            pszTmp[iLoop/2] = (*pLoop -87) << 4; 
        }
        else
        {
            printf("error:\'%c\' is not hex\n",*pLoop);
            goto error;
        }

        if(48 <= *pLoopTwo && 57 >= *pLoopTwo) //0~9
        {
            pszTmp[iLoop/2] = pszTmp[iLoop/2] + (*pLoopTwo - 48); 
        }
        else if( 65 <= *pLoopTwo && 70 >= *pLoopTwo) //A~F
        {
            pszTmp[iLoop/2] = pszTmp[iLoop/2] + (*pLoopTwo - 55);  
        }
        else if( 97 <= *pLoopTwo && 102 >= *pLoopTwo) //a~f 
        {
            pszTmp[iLoop/2] = pszTmp[iLoop/2] + (*pLoopTwo - 87);   
        }
        else
        {
            printf("error:\'%c\' is not hex\n",*pLoopTwo);
            goto error;

        }
        
        iLoop = iLoop + 2; 
    }
    if( STRLEN_12 == iLoop )
    {
        memcpy(pmac->mac,pszTmp,MACLEN_6); 
        goto done;
    }
error:
    free(pszTmp);
    free(pszTmpStr);
    //printf("iLoop:%d",iLoop);
    return NULL;
done:
    //printf("iLoop:%d",iLoop);
    free(pszTmp);
    free(pszTmpStr);
    return pmac; 
}

stumac_t *macadd(stumac_t *pmac,uint8_t n) //0 <= n <=255
{
    if(NULL == pmac || 0 == n)
    {
        return pmac;
    }
    pmac->mac[5] = pmac->mac[5] + n;
    if(pmac->mac[5] < n)
    {
        pmac->mac[4] = pmac->mac[4] + 1;
        if(0 == pmac->mac[4])
        {
            pmac->mac[3] = pmac->mac[3] + 1;
            if(0 == pmac->mac[3])
            {
                pmac->mac[2] = pmac->mac[2] + 1;
            }
        }
    }
    return pmac;
}
stumac_t *macadd_big(stumac_t *pmac,uint8_t n) //0 <= n <=255
{
    if(NULL == pmac || 0 == n)
    {
        return pmac;
    }
    pmac->mac[3] = pmac->mac[3] + n;
    return pmac;
}

stumac_t *macadd_big2(stumac_t *pmac,uint8_t n) //0 <= n <=255
{
    if(NULL == pmac || 0 == n)
    {
        return pmac;
    }
    //pmac->mac[3] = pmac->mac[3] + n;
    pmac->mac[3] = pmac->mac[3] & 0x0f | n ;
    return pmac;
}

int checkmac(const unsigned char* strMac)
{
    if(NULL == strMac || ( STRLEN_12 != strlen(strMac) && STRLEN_17 != strlen(strMac))) 
    {
        return 0;
    }
    int i = 0;
    unsigned char ch;
    if(STRLEN_12 == strlen(strMac))
    {
        for(i=0;i<STRLEN_12;i++)
        {
            ch = strMac[i];
            //0~9,A~F,a~f 
            if(!((48 <= ch && 57 >= ch) || ( 65 <= ch && 70 >= ch) || ( 97 <= ch && 102 >= ch)))
            {
                return 0;
            }
        }
    }
    else
    {
        for(i=0;i<STRLEN_17;i++)
        {
            ch = strMac[i];
            //0~9,A~F,a~f 
            if(!((48 <= ch && 57 >= ch) || ( 65 <= ch && 70 >= ch) || ( 97 <= ch && 102 >= ch) || ':' == ch))
            {
                return 0;
            }
        }
        
    }
    return 1;
}

/*
int writemac(FILE * pf,stumac_t *pstmac,long seek)
{
    if (NULL == pf || NULL == pstmac)
    {
        return -1;
    }
    if (-1 == fseek(pf,seek,SEEK_SET))
    {
        return -1;
    }
    if(1 != fwrite(pstmac->mac,MACLEN_6,1,pf))
    {
         return -1;
    }

    return 0;
}
void usage(void)
{
    fprintf(stderr, "usage: wrmac -m <MAC>\n");
}

void free_safy(void *p)
{
    if(NULL != p)
    {
        free(p);
    }
}
int main(int argc,char *argv[])
{
    int opt = 0;
    unsigned char *argMac = NULL;
    char * optString = "m:";
    while((opt=getopt(argc,argv,optString)) != -1)
    {
        switch(opt)
        {
            case 'm':
                argMac = (unsigned char*)malloc(strlen(optarg)+1);
                strcpy(argMac,optarg);
                //printf("'-o':outputfile:%s\n",optarg);
                break;
        }
    }
    if(3 != argc)
    {
        usage();
        free_safy(argMac); 
        return -1;
    }
    if(!checkmac(argMac))
    {
        printf("invalid mac:%s\n",argMac);
        free_safy(argMac); 
        return -1; 
    }

    stumac_t stmac;
    bzero(&stmac,sizeof(stumac_t));
    tomac(argMac,&stmac);
    
    FILE *pFile=fopen(MTD2,"rb+"); //获取文件的指针
    if(NULL == pFile)
    {
       printf("error:open %s failed!\n",MTD2); 
       goto error;
    }
    fseek(pFile,MAC_SEEK_4,SEEK_SET); //
    fread(stmac.mac,MACLEN_6,1,pFile); //读文件
    
    int i;
    for(i=0; i<MACLEN_6; i++)
    {
        printf("%X",stmac.mac[i]);
    }
    putchar('\n');
    if(0 != writemac(pFile,&stmac,MAC_SEEK_4))
    {
        printf("write mac failed at 4");
        goto error; 
    }
#ifdef MACADD_BIG
    macadd_big(&stmac,1);
#else
    macadd(&stmac,1);
#endif
    if(0 != writemac(pFile,&stmac,MAC_SEEK_40))
    {
        printf("write mac failed at 40");
        goto error;
    }

#ifdef MACADD_BIG
    macadd_big(&stmac,1);
#else
    macadd(&stmac,1);
#endif
    if(0 != writemac(pFile,&stmac,MAC_SEEK_46))
    {
        printf("write mac failed at 46");
        goto error;
    }

    fclose(pFile); 
    free_safy(argMac);  
    printf("write mac to factory successfully!\n"); 
    return 0;    
error:
    if(NULL !=pFile) 
    {
        fclose(pFile); 
    }
    free_safy(argMac); 
    return -1; 
}
*/
