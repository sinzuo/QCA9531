#include<stdio.h>
#include<stdlib.h>
#include"cJSON.h"
int main()
{
    //CreateObject
    cJSON *pJson = cJSON_CreateObject(); 
    cJSON_AddStringToObject(pJson,"version","1.0");
    cJSON_AddStringToObject(pJson,"token","123456678");
    cJSON_AddStringToObject(pJson,"deviceid","AABBCC");
    //char *out=cJSON_Print(pJson);
    char *out=cJSON_PrintUnformatted(pJson);
 
    printf("%s\n",out);
    free(out);
    cJSON_Delete(pJson);
    
    //Parse 
    pJson = NULL;
    char strJson[64]= "{\"result\":\"200\",\"info\":\"OK\"}";
    pJson = cJSON_Parse(strJson); 
    if(NULL == pJson)
    {
        printf("cJSON_Parse error!\n");
        return 1;
    }
    cJSON *pRes = cJSON_GetObjectItem(pJson,"result");
    cJSON *pInfo = cJSON_GetObjectItem(pJson,"info");
    printf("result=%s\n",pRes->valuestring);
    printf("inf=%s\n",pInfo->valuestring);
    cJSON_Delete(pJson);
    return 0;
}
