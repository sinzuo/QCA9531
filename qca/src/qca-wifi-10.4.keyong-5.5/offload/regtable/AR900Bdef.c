/*
 * Copyright (c) 2010 Atheros Communications, Inc.
 * All rights reserved.
 *
 *
 * $ATH_LICENSE_HOSTSDK0_C$
 *
 */
#if defined(AR900B_HEADERS_DEF)
#define AR900B 1

#define WLAN_HEADERS 1
#include "common_drv.h"
#include "AR900B/soc_addrs.h"
#include "AR900B/extra/hw/apb_map.h"
// FIXME_WIFI2 these headers are peregrine leftovers
#include "AR900B/hw/gpio_athr_wlan_reg.h"
#ifdef WLAN_HEADERS
#include "AR900B/extra/hw/wifi_top_reg_map.h"
#include "AR900B/hw/rtc_soc_reg.h"
#endif
#include "AR900B/hw/si_reg.h"
#include "AR900B/extra/hw/pcie_local_reg.h"
#include "AR900B/hw/ce_wrapper_reg_csr.h"
#if 0
#include "hw/soc_core_reg.h"
#include "hw/soc_pcie_reg.h"
#include "hw/ce_reg_csr.h"
#endif

#include "AR900B/extra/hw/soc_core_reg.h"
#include "AR900B/hw/soc_pcie_reg.h"
#include "AR900B/extra/hw/ce_reg_csr.h"
#include <AR900B/hw/interface/rx_location_info.h>
#include <AR900B/hw/interface/rx_pkt_end.h>
#include <AR900B/hw/interface/rx_phy_ppdu_end.h>
#include <AR900B/hw/interface/rx_timing_offset.h>
#include <AR900B/hw/interface/rx_location_info.h>
#include <AR900B/hw/tlv/rx_ppdu_start.h>
#include <AR900B/hw/tlv/rx_ppdu_end.h>
#include <AR900B/hw/tlv/rx_mpdu_start.h>
#include <AR900B/hw/tlv/rx_mpdu_end.h>
#include <AR900B/hw/tlv/rx_msdu_start.h>
#include <AR900B/hw/tlv/rx_msdu_end.h>
#include <AR900B/hw/tlv/rx_attention.h>
#include <AR900B/hw/tlv/rx_frag_info.h>
#include <AR900B/hw/datastruct/msdu_link_ext.h>
#include <AR900B/hw/emu_phy_reg.h>

/* Base address is defined in pcie_local_reg.h. Macros which access the
 * registers include the base address in their definition.
 */
#define PCIE_LOCAL_BASE_ADDRESS 0

#define FW_EVENT_PENDING_ADDRESS (WIFICMN_SCRATCH_3_ADDRESS)
#define DRAM_BASE_ADDRESS TARG_DRAM_START

/* Backwards compatibility -- TBDXXX */

#define MISSING 0

#define WLAN_SYSTEM_SLEEP_DISABLE_LSB           WIFI_SYSTEM_SLEEP_DISABLE_LSB
#define WLAN_SYSTEM_SLEEP_DISABLE_MASK          WIFI_SYSTEM_SLEEP_DISABLE_MASK
#define WLAN_RESET_CONTROL_COLD_RST_MASK        WIFI_RESET_CONTROL_MAC_COLD_RST_MASK
#define WLAN_RESET_CONTROL_WARM_RST_MASK        WIFI_RESET_CONTROL_MAC_WARM_RST_MASK
#define SOC_CLOCK_CONTROL_OFFSET                SOC_CLOCK_CONTROL_ADDRESS
#define SOC_RESET_CONTROL_OFFSET                SOC_RESET_CONTROL_ADDRESS
#define CPU_CLOCK_OFFSET                        SOC_CPU_CLOCK_ADDRESS
#define SOC_LPO_CAL_OFFSET                      SOC_LPO_CAL_ADDRESS
#define SOC_RESET_CONTROL_CE_RST_MASK           WIFI_RESET_CONTROL_CE_RESET_MASK
#define WLAN_SYSTEM_SLEEP_OFFSET                WIFI_SYSTEM_SLEEP_ADDRESS
#define WLAN_RESET_CONTROL_OFFSET               WIFI_RESET_CONTROL_ADDRESS
#define CLOCK_CONTROL_OFFSET                    SOC_CLOCK_CONTROL_OFFSET
#define CLOCK_CONTROL_SI0_CLK_MASK              SOC_CLOCK_CONTROL_SI0_CLK_MASK
#define RESET_CONTROL_SI0_RST_MASK              SOC_RESET_CONTROL_SI0_RST_MASK
#define GPIO_BASE_ADDRESS                       WLAN_GPIO_BASE_ADDRESS
#define GPIO_PIN0_OFFSET                        WLAN_GPIO_PIN0_ADDRESS
#define GPIO_PIN1_OFFSET                        WLAN_GPIO_PIN1_ADDRESS
#define GPIO_PIN0_CONFIG_MASK                   WLAN_GPIO_PIN0_CONFIG_MASK
#define GPIO_PIN1_CONFIG_MASK                   WLAN_GPIO_PIN1_CONFIG_MASK
#define SI_BASE_ADDRESS                         WLAN_SI_BASE_ADDRESS
#define SCRATCH_BASE_ADDRESS                    SOC_CORE_BASE_ADDRESS
#define LOCAL_SCRATCH_OFFSET                    0x18
#define GPIO_PIN10_OFFSET                       WLAN_GPIO_PIN10_ADDRESS
#define GPIO_PIN11_OFFSET                       WLAN_GPIO_PIN11_ADDRESS
#define GPIO_PIN12_OFFSET                       WLAN_GPIO_PIN12_ADDRESS
#define GPIO_PIN13_OFFSET                       WLAN_GPIO_PIN13_ADDRESS
#define SI_CONFIG_OFFSET                        SI_CONFIG_ADDRESS
#define SI_TX_DATA0_OFFSET                      SI_TX_DATA0_ADDRESS
#define SI_TX_DATA1_OFFSET                      SI_TX_DATA1_ADDRESS
#define SI_RX_DATA0_OFFSET                      SI_RX_DATA0_ADDRESS
#define SI_RX_DATA1_OFFSET                      SI_RX_DATA1_ADDRESS
#define SI_CS_OFFSET                            SI_CS_ADDRESS
#define CPU_CLOCK_STANDARD_LSB                  SOC_CPU_CLOCK_STANDARD_LSB
#define CPU_CLOCK_STANDARD_MASK                 SOC_CPU_CLOCK_STANDARD_MASK
#define LPO_CAL_ENABLE_LSB                      SOC_LPO_CAL_ENABLE_LSB
#define LPO_CAL_ENABLE_MASK                     SOC_LPO_CAL_ENABLE_MASK
#define ANALOG_INTF_BASE_ADDRESS                WLAN_ANALOG_INTF_BASE_ADDRESS
#define MBOX_BASE_ADDRESS                       MISSING
#define INT_STATUS_ENABLE_ERROR_LSB             MISSING
#define INT_STATUS_ENABLE_ERROR_MASK            MISSING
#define INT_STATUS_ENABLE_CPU_LSB               MISSING
#define INT_STATUS_ENABLE_CPU_MASK              MISSING
#define INT_STATUS_ENABLE_COUNTER_LSB           MISSING
#define INT_STATUS_ENABLE_COUNTER_MASK          MISSING
#define INT_STATUS_ENABLE_MBOX_DATA_LSB         MISSING
#define INT_STATUS_ENABLE_MBOX_DATA_MASK        MISSING
#define ERROR_STATUS_ENABLE_RX_UNDERFLOW_LSB    MISSING
#define ERROR_STATUS_ENABLE_RX_UNDERFLOW_MASK   MISSING
#define ERROR_STATUS_ENABLE_TX_OVERFLOW_LSB     MISSING
#define ERROR_STATUS_ENABLE_TX_OVERFLOW_MASK    MISSING
#define COUNTER_INT_STATUS_ENABLE_BIT_LSB       MISSING
#define COUNTER_INT_STATUS_ENABLE_BIT_MASK      MISSING
#define INT_STATUS_ENABLE_ADDRESS               MISSING
#define CPU_INT_STATUS_ENABLE_BIT_LSB           MISSING
#define CPU_INT_STATUS_ENABLE_BIT_MASK          MISSING
#define HOST_INT_STATUS_ADDRESS                 MISSING
#define CPU_INT_STATUS_ADDRESS                  MISSING
#define ERROR_INT_STATUS_ADDRESS                MISSING
#define ERROR_INT_STATUS_WAKEUP_MASK            MISSING
#define ERROR_INT_STATUS_WAKEUP_LSB             MISSING
#define ERROR_INT_STATUS_RX_UNDERFLOW_MASK      MISSING
#define ERROR_INT_STATUS_RX_UNDERFLOW_LSB       MISSING
#define ERROR_INT_STATUS_TX_OVERFLOW_MASK       MISSING
#define ERROR_INT_STATUS_TX_OVERFLOW_LSB        MISSING
#define COUNT_DEC_ADDRESS                       MISSING
#define HOST_INT_STATUS_CPU_MASK                MISSING
#define HOST_INT_STATUS_CPU_LSB                 MISSING
#define HOST_INT_STATUS_ERROR_MASK              MISSING
#define HOST_INT_STATUS_ERROR_LSB               MISSING
#define HOST_INT_STATUS_COUNTER_MASK            MISSING
#define HOST_INT_STATUS_COUNTER_LSB             MISSING
#define RX_LOOKAHEAD_VALID_ADDRESS              MISSING
#define WINDOW_DATA_ADDRESS                     MISSING
#define WINDOW_READ_ADDR_ADDRESS                MISSING
#define WINDOW_WRITE_ADDR_ADDRESS               MISSING
/* MAC Descriptor */
#define RX_PPDU_END_ANTENNA_OFFSET_DWORD        (RX_PPDU_END_25_RX_ANTENNA_OFFSET >> 2)
/* GPIO Register */
#define GPIO_ENABLE_W1TS_LOW_ADDRESS            WLAN_GPIO_ENABLE_W1TS_LOW_ADDRESS
#define GPIO_PIN0_CONFIG_LSB                    WLAN_GPIO_PIN0_CONFIG_LSB
#define GPIO_PIN0_PAD_PULL_LSB                  WLAN_GPIO_PIN0_PAD_PULL_LSB
#define GPIO_PIN0_PAD_PULL_MASK                 WLAN_GPIO_PIN0_PAD_PULL_MASK
/* CE descriptor */
#define CE_SRC_DESC_SIZE_DWORD         2
#define CE_DEST_DESC_SIZE_DWORD        2
#define CE_SRC_DESC_SRC_PTR_OFFSET_DWORD    0
#define CE_SRC_DESC_INFO_OFFSET_DWORD       1
#define CE_DEST_DESC_DEST_PTR_OFFSET_DWORD  0
#define CE_DEST_DESC_INFO_OFFSET_DWORD      1
#if _BYTE_ORDER == _BIG_ENDIAN
#define CE_SRC_DESC_INFO_NBYTES_MASK               0xFFFF0000
#define CE_SRC_DESC_INFO_NBYTES_SHIFT              16
#define CE_SRC_DESC_INFO_GATHER_MASK               0x00008000
#define CE_SRC_DESC_INFO_GATHER_SHIFT              15
#define CE_SRC_DESC_INFO_BYTE_SWAP_MASK            0x00004000
#define CE_SRC_DESC_INFO_BYTE_SWAP_SHIFT           14
#define CE_SRC_DESC_INFO_HOST_INT_DISABLE_MASK     0x00002000
#define CE_SRC_DESC_INFO_HOST_INT_DISABLE_SHIFT    13
#define CE_SRC_DESC_INFO_TARGET_INT_DISABLE_MASK   0x00001000
#define CE_SRC_DESC_INFO_TARGET_INT_DISABLE_SHIFT  12
#define CE_SRC_DESC_INFO_META_DATA_MASK            0x00000FFF
#define CE_SRC_DESC_INFO_META_DATA_SHIFT           0
#else
#define CE_SRC_DESC_INFO_NBYTES_MASK               0x0000FFFF
#define CE_SRC_DESC_INFO_NBYTES_SHIFT              0
#define CE_SRC_DESC_INFO_GATHER_MASK               0x00010000
#define CE_SRC_DESC_INFO_GATHER_SHIFT              16
#define CE_SRC_DESC_INFO_BYTE_SWAP_MASK            0x00020000
#define CE_SRC_DESC_INFO_BYTE_SWAP_SHIFT           17
#define CE_SRC_DESC_INFO_HOST_INT_DISABLE_MASK     0x00040000
#define CE_SRC_DESC_INFO_HOST_INT_DISABLE_SHIFT    18
#define CE_SRC_DESC_INFO_TARGET_INT_DISABLE_MASK   0x00080000
#define CE_SRC_DESC_INFO_TARGET_INT_DISABLE_SHIFT  19
#define CE_SRC_DESC_INFO_META_DATA_MASK            0xFFF00000
#define CE_SRC_DESC_INFO_META_DATA_SHIFT           20
#endif
#if _BYTE_ORDER == _BIG_ENDIAN
#define CE_DEST_DESC_INFO_NBYTES_MASK              0xFFFF0000
#define CE_DEST_DESC_INFO_NBYTES_SHIFT             16
#define CE_DEST_DESC_INFO_GATHER_MASK              0x00008000
#define CE_DEST_DESC_INFO_GATHER_SHIFT             15
#define CE_DEST_DESC_INFO_BYTE_SWAP_MASK           0x00004000
#define CE_DEST_DESC_INFO_BYTE_SWAP_SHIFT          14
#define CE_DEST_DESC_INFO_HOST_INT_DISABLE_MASK    0x00002000
#define CE_DEST_DESC_INFO_HOST_INT_DISABLE_SHIFT   13
#define CE_DEST_DESC_INFO_TARGET_INT_DISABLE_MASK  0x00001000
#define CE_DEST_DESC_INFO_TARGET_INT_DISABLE_SHIFT 12
#define CE_DEST_DESC_INFO_META_DATA_MASK           0x00000FFF
#define CE_DEST_DESC_INFO_META_DATA_SHIFT          0
#else
#define CE_DEST_DESC_INFO_NBYTES_MASK              0x0000FFFF
#define CE_DEST_DESC_INFO_NBYTES_SHIFT             0
#define CE_DEST_DESC_INFO_GATHER_MASK              0x00010000
#define CE_DEST_DESC_INFO_GATHER_SHIFT             16
#define CE_DEST_DESC_INFO_BYTE_SWAP_MASK           0x00020000
#define CE_DEST_DESC_INFO_BYTE_SWAP_SHIFT          17
#define CE_DEST_DESC_INFO_HOST_INT_DISABLE_MASK    0x00040000
#define CE_DEST_DESC_INFO_HOST_INT_DISABLE_SHIFT   18
#define CE_DEST_DESC_INFO_TARGET_INT_DISABLE_MASK  0x00080000
#define CE_DEST_DESC_INFO_TARGET_INT_DISABLE_SHIFT 19
#define CE_DEST_DESC_INFO_META_DATA_MASK           0xFFF00000
#define CE_DEST_DESC_INFO_META_DATA_SHIFT          20
#endif

#define MY_TARGET_DEF AR900B_TARGETdef
#define MY_HOST_DEF AR900B_HOSTdef
#define MY_TARGET_BOARD_DATA_SZ AR900B_BOARD_DATA_SZ
#define MY_TARGET_BOARD_EXT_DATA_SZ AR900B_BOARD_EXT_DATA_SZ
#include "targetdef.h"
#include "hostdef.h"
#else
#include "common_drv.h"
#include "targetdef.h"
#include "hostdef.h"
struct targetdef_s *AR900B_TARGETdef=NULL;
struct hostdef_s *AR900B_HOSTdef=NULL;
#endif /*AR900B_HEADERS_DEF */