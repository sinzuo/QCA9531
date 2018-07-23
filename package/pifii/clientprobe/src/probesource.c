#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "log.h"
#include "probesource.h"
//hard coding read after the module installed
#define KERNEL_PHY_ADDR  0x50b4000
#define NUM_MAC_TABLE 256
#define ONE_PACK_SIZE 8 
//uint8_t maccount;
//uint8_t mactable[NUM_MAC_TABLE][7];
static uint8_t *g_map_buf = NULL;
static int g_mem_fd=-1;
size_t g_pagesize = 0;

int probe_source_init()
{
        unsigned long phy_addr;
      	FILE *fp;
        uint8_t addr[5];
        g_pagesize = getpagesize();

        //从/tmp/memaddr 读取驱动生成的内存虚拟地址 
        phy_addr=KERNEL_PHY_ADDR;

	if((fp=fopen("/tmp/memaddr","r"))==NULL)
        {
	    D("/tmp/memaddr file cannot be opened");
	    return -1;
	}

	if(4!=fread(addr,1,4,fp))
	{
	    D("/tmp/memaddr read error count");
	    fclose(fp);
	    return -1;
	}
	fclose(fp);
	phy_addr= addr[0]<<24  | addr[1] <<16 | addr[2] <<8 | addr[3];
	
	//printf("%X \n",phy_addr);
	D("%X \n",phy_addr);
		
        // 通过phy_addr 建立内存映射
        g_mem_fd=open("/dev/mem",O_RDWR);
        if(g_mem_fd == -1)
        {
            //perror("open");
	    D("/dev/mem file cannot be opened");
	    return -1;
        }

        //建立映射
        //g_map_buf=mmap(0, g_pagesize, PROT_READ|PROT_WRITE, MAP_SHARED, g_mem_fd, phy_addr);
        g_map_buf=mmap(0, g_pagesize, PROT_READ, MAP_SHARED, g_mem_fd, phy_addr);

        if(g_map_buf == MAP_FAILED)
        {
            //perror("mmap");
            D("mmap failed");
            g_map_buf=NULL;
            close(g_mem_fd);
            return -1;
        }
        return 0;
}
 
uint32_t probe_source_read(uint8_t *mactable,uint32_t size)
{
        if (NULL == g_map_buf)
        {
            D("error:probe source data not init");
            return 0;
        }
        uint8_t maccount=0;       //探到的mac数据量
        uint32_t datasize=0;

	//映射成功后  可以直接通过buf读取 驱动的探针数据  
	// uint8_t mactable[NUM_MAC_TABLE][7];  这是驱动层的探针数据
	// 其中 mactable[0][0]写入的是 探测到的数量
	maccount=g_map_buf[0];
        if(1 <= maccount)
        {
            datasize=maccount*ONE_PACK_SIZE-ONE_PACK_SIZE;
            if (0 < datasize && size >= datasize) 
            {
                memcpy(mactable,g_map_buf+ONE_PACK_SIZE,datasize);
                return datasize;
            }
        }
	//所以 从mactable[1] 开始才是mac数据
	//for(i=1;i<maccount;i++)
        //printf("%02X %02X %02X %02X %02X %02X %d\n",mactable[i][0],mactable[i][1],mactable[i][2],mactable[i][3],mactable[i][4],mactable[i][5],(char )mactable[i][6]-95);
        
        return 0;

}

int probe_source_status()
{
    if(NULL == g_map_buf)
    {
        return -1;
    } 
    return 0;
}

void probe_source_destroy()
{
    if(NULL != g_map_buf)
    {
        munmap(g_map_buf,g_pagesize);
    }
    if(-1 != g_mem_fd)
    {
        close(g_mem_fd);
    }
}
