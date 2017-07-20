#include "probedata.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

char *exe_shell(const char *cmd,char *resbuf,unsigned int size)
{
    if(NULL == cmd || NULL == resbuf || 0 == size)
    {
        return NULL;
    }
    FILE *fpp = popen(cmd,"r");
    if(fgets(resbuf,size,fpp) != NULL)
    {
       if( '\n' == resbuf[strlen(resbuf) - 1])
       {
           resbuf[strlen(resbuf) - 1] = '\0';
       }else
       {
           resbuf[strlen(resbuf)] = '\0';
       }
    }
    pclose(fpp);
    return resbuf;
}

static char *get_mtd(char *name,char *buf)
{
    if( NULL == name || NULL == buf)
    {
        return NULL;
    }
    char line[64]="\0";
    char *mtd = NULL;
    FILE * fp = fopen("/proc/mtd","r");
    //FILE * fp = fopen("./mtd","r");
    if (NULL == fp)
    {
       return NULL;
    }
    while(NULL != fgets(line,sizeof(line),fp))
    {
        if(NULL != strstr(line,name) && NULL != strtok(line,":"))
        {
            strcpy(buf,"/dev/");
            strncpy(buf+5,line,strlen(line)+1);
            fclose(fp);
            return buf;
        }
    }
    fclose(fp);
    return NULL;
}

char *tomac(const char *pszStr,char *pmac)
{
    char * pszTmpStr = NULL;    
    if(NULL == pszStr || NULL == pmac) 
    {
        return NULL; 
    }
    if( MAC_MIN_STR != strlen(pszStr) && MAC_MAX_STR != strlen(pszStr))
    {
        printf("error:mac string size is not 12 or 17\n");
        return NULL; 
    }
    else if(MAC_MAX_STR == strlen(pszStr))
    {
        pszTmpStr = (char*)malloc(MAC_MIN_STR); 
        int i = 0,j = 0;
        while(i < MAC_MIN_STR && j < MAC_MAX_STR)
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
        pszTmpStr = (char*)malloc(MAC_MIN_STR); 
        strncpy(pszTmpStr,pszStr,MAC_MIN_STR);
    }
    int iLoop = 0; 
    unsigned char *pszTmp = (unsigned char*)malloc(MAC_ADDR_LEN); 
    bzero(pszTmp,6);
    char *pLoop = pszTmpStr;
    char *pLoopTwo = pLoop + 1;
    while(iLoop < MAC_MIN_STR)
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
    if( MAC_MIN_STR == iLoop )
    {
        memcpy(pmac,pszTmp,MAC_ADDR_LEN); 
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

char *get_device_id(char *buf)
{
    if( NULL == buf)
    {
        return NULL; 
    }
    char mtd[16]="\0";
    get_mtd("art",mtd);
    int fd = open(mtd, O_RDONLY);
    if(fd < 0)
    {
        printf("Could not open mtd device: %s\n", mtd);
        return NULL;
    } 
    if(read(fd, buf, MAC_ADDR_LEN) != MAC_ADDR_LEN){
        printf("read() %s failed\n",mtd);
        close(fd);
        return NULL;
    }    
    close(fd);
    return buf;
}

