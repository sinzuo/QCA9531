#ifndef __PROBE_SOURCE_H__
#define __PROBE_SOURCE_H__

#include <stdint.h>
int probe_source_init();
uint32_t probe_source_read(uint8_t *mactable,uint32_t size);
void probe_source_destroy();

#endif
