#ifeq ($(CONFIG_FLASH_TOOL), y)
#EXEC += flash
#endif
#ifeq ($(CONFIG_ETHMAC_TOOL), y)
EXEC += writemac
#endif

all: $(EXEC) 

#flash: flash_mtd.o
#	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

writemac: writemac.o promac.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

#romfs:
#	$(ROMFSINST) -e CONFIG_FLASH_TOOL /bin/flash
#	$(ROMFSINST) -e CONFIG_ETHMAC_TOOL /bin/eth_mac

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o

