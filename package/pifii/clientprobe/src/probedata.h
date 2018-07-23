
#ifndef __PROBEDATA_H__
#define __PROBEDATA_H__

#include <stdint.h>

#define MAC_ADDR_LEN 6
#define SSID_LEN   64
#define MAC_MAX_STR  17
#define MAC_MIN_STR   12

/*
struct probe_pack{
     uint8_t       device_id[MAC_ADDR_LEN];         
     uint8_t       type; //monitor=1 or url=2                   
     uint8_t       ap_or_client;//AP_OR_CLIENT;            
     uint8_t       mac[MAC_ADDR_LEN];           
     int8_t        real_rssi;    
     uint8_t       channel;             
     uint8_t       ssid[SSID_LEN];            
     uint32_t      resvert;  //MAGIC data       
};
*/

struct probe_device{
     uint8_t       device_id[MAC_ADDR_LEN];         
     uint8_t       ssid[SSID_LEN];            
     uint8_t       channel[5];
     int32_t       threshold;
};

struct probe_head{
     uint8_t ver;
     uint8_t type;
     uint8_t apmac[MAC_ADDR_LEN];
     uint32_t size;
}__attribute__((packed));
/*
struct probe_pack{
     uint8_t mac[MAC_ADDR_LEN]; 
     int8_t dbm;
};
*/
char *tomac(const char *pszStr,char *pmac);
char *exe_shell(const char *cmd,char *resbuf,unsigned int size);
char *get_device_id(char *buf);

#endif
