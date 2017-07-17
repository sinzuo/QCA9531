/*
 * Copyright (c) 2015 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

#ifndef _CE1_CE_REG_CSR_H_
#define _CE1_CE_REG_CSR_H_


#ifndef __CE1_CE_REG_CSR_BASE_ADDRESS
#define __CE1_CE_REG_CSR_BASE_ADDRESS (0x4a400)
#endif


// 0x0 (CE1_SR_BA)
#define CE1_SR_BA_V_LSB                                                        0
#define CE1_SR_BA_V_MSB                                                        31
#define CE1_SR_BA_V_MASK                                                       0xffffffff
#define CE1_SR_BA_V_GET(x)                                                     (((x) & CE1_SR_BA_V_MASK) >> CE1_SR_BA_V_LSB)
#define CE1_SR_BA_V_SET(x)                                                     (((0 | (x)) << CE1_SR_BA_V_LSB) & CE1_SR_BA_V_MASK)
#define CE1_SR_BA_V_RESET                                                      0x0
#define CE1_SR_BA_ADDRESS                                                      (0x0 + __CE1_CE_REG_CSR_BASE_ADDRESS)
#define CE1_SR_BA_RSTMASK                                                      0xffffffff
#define CE1_SR_BA_RESET                                                        0x0

// 0x4 (CE1_SR_SIZE)
#define CE1_SR_SIZE_V_LSB                                                      0
#define CE1_SR_SIZE_V_MSB                                                      15
#define CE1_SR_SIZE_V_MASK                                                     0xffff
#define CE1_SR_SIZE_V_GET(x)                                                   (((x) & CE1_SR_SIZE_V_MASK) >> CE1_SR_SIZE_V_LSB)
#define CE1_SR_SIZE_V_SET(x)                                                   (((0 | (x)) << CE1_SR_SIZE_V_LSB) & CE1_SR_SIZE_V_MASK)
#define CE1_SR_SIZE_V_RESET                                                    0x0
#define CE1_SR_SIZE_ADDRESS                                                    (0x4 + __CE1_CE_REG_CSR_BASE_ADDRESS)
#define CE1_SR_SIZE_RSTMASK                                                    0xffff
#define CE1_SR_SIZE_RESET                                                      0x0

// 0x8 (CE1_DR_BA)
#define CE1_DR_BA_V_LSB                                                        0
#define CE1_DR_BA_V_MSB                                                        31
#define CE1_DR_BA_V_MASK                                                       0xffffffff
#define CE1_DR_BA_V_GET(x)                                                     (((x) & CE1_DR_BA_V_MASK) >> CE1_DR_BA_V_LSB)
#define CE1_DR_BA_V_SET(x)                                                     (((0 | (x)) << CE1_DR_BA_V_LSB) & CE1_DR_BA_V_MASK)
#define CE1_DR_BA_V_RESET                                                      0x0
#define CE1_DR_BA_ADDRESS                                                      (0x8 + __CE1_CE_REG_CSR_BASE_ADDRESS)
#define CE1_DR_BA_RSTMASK                                                      0xffffffff
#define CE1_DR_BA_RESET                                                        0x0

// 0xc (CE1_DR_SIZE)
#define CE1_DR_SIZE_V_LSB                                                      0
#define CE1_DR_SIZE_V_MSB                                                      15
#define CE1_DR_SIZE_V_MASK                                                     0xffff
#define CE1_DR_SIZE_V_GET(x)                                                   (((x) & CE1_DR_SIZE_V_MASK) >> CE1_DR_SIZE_V_LSB)
#define CE1_DR_SIZE_V_SET(x)                                                   (((0 | (x)) << CE1_DR_SIZE_V_LSB) & CE1_DR_SIZE_V_MASK)
#define CE1_DR_SIZE_V_RESET                                                    0x0
#define CE1_DR_SIZE_ADDRESS                                                    (0xc + __CE1_CE_REG_CSR_BASE_ADDRESS)
#define CE1_DR_SIZE_RSTMASK                                                    0xffff
#define CE1_DR_SIZE_RESET                                                      0x0

// 0x10 (CE1_CE_CTRL1)
#define CE1_CE_CTRL1_MSI_EN_LSB                                                18
#define CE1_CE_CTRL1_MSI_EN_MSB                                                18
#define CE1_CE_CTRL1_MSI_EN_MASK                                               0x40000
#define CE1_CE_CTRL1_MSI_EN_GET(x)                                             (((x) & CE1_CE_CTRL1_MSI_EN_MASK) >> CE1_CE_CTRL1_MSI_EN_LSB)
#define CE1_CE_CTRL1_MSI_EN_SET(x)                                             (((0 | (x)) << CE1_CE_CTRL1_MSI_EN_LSB) & CE1_CE_CTRL1_MSI_EN_MASK)
#define CE1_CE_CTRL1_MSI_EN_RESET                                              0x0
#define CE1_CE_CTRL1_DST_RING_BYTE_SWAP_EN_LSB                                 17
#define CE1_CE_CTRL1_DST_RING_BYTE_SWAP_EN_MSB                                 17
#define CE1_CE_CTRL1_DST_RING_BYTE_SWAP_EN_MASK                                0x20000
#define CE1_CE_CTRL1_DST_RING_BYTE_SWAP_EN_GET(x)                              (((x) & CE1_CE_CTRL1_DST_RING_BYTE_SWAP_EN_MASK) >> CE1_CE_CTRL1_DST_RING_BYTE_SWAP_EN_LSB)
#define CE1_CE_CTRL1_DST_RING_BYTE_SWAP_EN_SET(x)                              (((0 | (x)) << CE1_CE_CTRL1_DST_RING_BYTE_SWAP_EN_LSB) & CE1_CE_CTRL1_DST_RING_BYTE_SWAP_EN_MASK)
#define CE1_CE_CTRL1_DST_RING_BYTE_SWAP_EN_RESET                               0x0
#define CE1_CE_CTRL1_SRC_RING_BYTE_SWAP_EN_LSB                                 16
#define CE1_CE_CTRL1_SRC_RING_BYTE_SWAP_EN_MSB                                 16
#define CE1_CE_CTRL1_SRC_RING_BYTE_SWAP_EN_MASK                                0x10000
#define CE1_CE_CTRL1_SRC_RING_BYTE_SWAP_EN_GET(x)                              (((x) & CE1_CE_CTRL1_SRC_RING_BYTE_SWAP_EN_MASK) >> CE1_CE_CTRL1_SRC_RING_BYTE_SWAP_EN_LSB)
#define CE1_CE_CTRL1_SRC_RING_BYTE_SWAP_EN_SET(x)                              (((0 | (x)) << CE1_CE_CTRL1_SRC_RING_BYTE_SWAP_EN_LSB) & CE1_CE_CTRL1_SRC_RING_BYTE_SWAP_EN_MASK)
#define CE1_CE_CTRL1_SRC_RING_BYTE_SWAP_EN_RESET                               0x0
#define CE1_CE_CTRL1_DMAX_LENGTH_LSB                                           0
#define CE1_CE_CTRL1_DMAX_LENGTH_MSB                                           15
#define CE1_CE_CTRL1_DMAX_LENGTH_MASK                                          0xffff
#define CE1_CE_CTRL1_DMAX_LENGTH_GET(x)                                        (((x) & CE1_CE_CTRL1_DMAX_LENGTH_MASK) >> CE1_CE_CTRL1_DMAX_LENGTH_LSB)
#define CE1_CE_CTRL1_DMAX_LENGTH_SET(x)                                        (((0 | (x)) << CE1_CE_CTRL1_DMAX_LENGTH_LSB) & CE1_CE_CTRL1_DMAX_LENGTH_MASK)
#define CE1_CE_CTRL1_DMAX_LENGTH_RESET                                         0x80
#define CE1_CE_CTRL1_ADDRESS                                                   (0x10 + __CE1_CE_REG_CSR_BASE_ADDRESS)
#define CE1_CE_CTRL1_RSTMASK                                                   0x7ffff
#define CE1_CE_CTRL1_RESET                                                     0x80

// 0x14 (CE1_CE_CTRL2)
#define CE1_CE_CTRL2_NUM_AXI_OUTSTANDING_WRITES_LSB                            25
#define CE1_CE_CTRL2_NUM_AXI_OUTSTANDING_WRITES_MSB                            29
#define CE1_CE_CTRL2_NUM_AXI_OUTSTANDING_WRITES_MASK                           0x3e000000
#define CE1_CE_CTRL2_NUM_AXI_OUTSTANDING_WRITES_GET(x)                         (((x) & CE1_CE_CTRL2_NUM_AXI_OUTSTANDING_WRITES_MASK) >> CE1_CE_CTRL2_NUM_AXI_OUTSTANDING_WRITES_LSB)
#define CE1_CE_CTRL2_NUM_AXI_OUTSTANDING_WRITES_SET(x)                         (((0 | (x)) << CE1_CE_CTRL2_NUM_AXI_OUTSTANDING_WRITES_LSB) & CE1_CE_CTRL2_NUM_AXI_OUTSTANDING_WRITES_MASK)
#define CE1_CE_CTRL2_NUM_AXI_OUTSTANDING_WRITES_RESET                          0x8
#define CE1_CE_CTRL2_NUM_AXI_OUTSTANDING_READS_LSB                             20
#define CE1_CE_CTRL2_NUM_AXI_OUTSTANDING_READS_MSB                             24
#define CE1_CE_CTRL2_NUM_AXI_OUTSTANDING_READS_MASK                            0x1f00000
#define CE1_CE_CTRL2_NUM_AXI_OUTSTANDING_READS_GET(x)                          (((x) & CE1_CE_CTRL2_NUM_AXI_OUTSTANDING_READS_MASK) >> CE1_CE_CTRL2_NUM_AXI_OUTSTANDING_READS_LSB)
#define CE1_CE_CTRL2_NUM_AXI_OUTSTANDING_READS_SET(x)                          (((0 | (x)) << CE1_CE_CTRL2_NUM_AXI_OUTSTANDING_READS_LSB) & CE1_CE_CTRL2_NUM_AXI_OUTSTANDING_READS_MASK)
#define CE1_CE_CTRL2_NUM_AXI_OUTSTANDING_READS_RESET                           0x18
#define CE1_CE_CTRL2_AXI_TIMEOUT_LSB                                           4
#define CE1_CE_CTRL2_AXI_TIMEOUT_MSB                                           19
#define CE1_CE_CTRL2_AXI_TIMEOUT_MASK                                          0xffff0
#define CE1_CE_CTRL2_AXI_TIMEOUT_GET(x)                                        (((x) & CE1_CE_CTRL2_AXI_TIMEOUT_MASK) >> CE1_CE_CTRL2_AXI_TIMEOUT_LSB)
#define CE1_CE_CTRL2_AXI_TIMEOUT_SET(x)                                        (((0 | (x)) << CE1_CE_CTRL2_AXI_TIMEOUT_LSB) & CE1_CE_CTRL2_AXI_TIMEOUT_MASK)
#define CE1_CE_CTRL2_AXI_TIMEOUT_RESET                                         0x1f40
#define CE1_CE_CTRL2_DEST_AXI_MAX_LEN_LSB                                      2
#define CE1_CE_CTRL2_DEST_AXI_MAX_LEN_MSB                                      3
#define CE1_CE_CTRL2_DEST_AXI_MAX_LEN_MASK                                     0xc
#define CE1_CE_CTRL2_DEST_AXI_MAX_LEN_GET(x)                                   (((x) & CE1_CE_CTRL2_DEST_AXI_MAX_LEN_MASK) >> CE1_CE_CTRL2_DEST_AXI_MAX_LEN_LSB)
#define CE1_CE_CTRL2_DEST_AXI_MAX_LEN_SET(x)                                   (((0 | (x)) << CE1_CE_CTRL2_DEST_AXI_MAX_LEN_LSB) & CE1_CE_CTRL2_DEST_AXI_MAX_LEN_MASK)
#define CE1_CE_CTRL2_DEST_AXI_MAX_LEN_RESET                                    0x1
#define CE1_CE_CTRL2_SRC_AXI_MAX_LEN_LSB                                       0
#define CE1_CE_CTRL2_SRC_AXI_MAX_LEN_MSB                                       1
#define CE1_CE_CTRL2_SRC_AXI_MAX_LEN_MASK                                      0x3
#define CE1_CE_CTRL2_SRC_AXI_MAX_LEN_GET(x)                                    (((x) & CE1_CE_CTRL2_SRC_AXI_MAX_LEN_MASK) >> CE1_CE_CTRL2_SRC_AXI_MAX_LEN_LSB)
#define CE1_CE_CTRL2_SRC_AXI_MAX_LEN_SET(x)                                    (((0 | (x)) << CE1_CE_CTRL2_SRC_AXI_MAX_LEN_LSB) & CE1_CE_CTRL2_SRC_AXI_MAX_LEN_MASK)
#define CE1_CE_CTRL2_SRC_AXI_MAX_LEN_RESET                                     0x1
#define CE1_CE_CTRL2_ADDRESS                                                   (0x14 + __CE1_CE_REG_CSR_BASE_ADDRESS)
#define CE1_CE_CTRL2_RSTMASK                                                   0x3fffffff
#define CE1_CE_CTRL2_RESET                                                     0x1181f405

// 0x18 (CE1_CE_CMD)
#define CE1_CE_CMD_HALT_STATUS_LSB                                             3
#define CE1_CE_CMD_HALT_STATUS_MSB                                             3
#define CE1_CE_CMD_HALT_STATUS_MASK                                            0x8
#define CE1_CE_CMD_HALT_STATUS_GET(x)                                          (((x) & CE1_CE_CMD_HALT_STATUS_MASK) >> CE1_CE_CMD_HALT_STATUS_LSB)
#define CE1_CE_CMD_HALT_STATUS_SET(x)                                          (((0 | (x)) << CE1_CE_CMD_HALT_STATUS_LSB) & CE1_CE_CMD_HALT_STATUS_MASK)
#define CE1_CE_CMD_HALT_STATUS_RESET                                           0x0
#define CE1_CE_CMD_DST_FLUSH_LSB                                               2
#define CE1_CE_CMD_DST_FLUSH_MSB                                               2
#define CE1_CE_CMD_DST_FLUSH_MASK                                              0x4
#define CE1_CE_CMD_DST_FLUSH_GET(x)                                            (((x) & CE1_CE_CMD_DST_FLUSH_MASK) >> CE1_CE_CMD_DST_FLUSH_LSB)
#define CE1_CE_CMD_DST_FLUSH_SET(x)                                            (((0 | (x)) << CE1_CE_CMD_DST_FLUSH_LSB) & CE1_CE_CMD_DST_FLUSH_MASK)
#define CE1_CE_CMD_DST_FLUSH_RESET                                             0x0
#define CE1_CE_CMD_SRC_FLUSH_LSB                                               1
#define CE1_CE_CMD_SRC_FLUSH_MSB                                               1
#define CE1_CE_CMD_SRC_FLUSH_MASK                                              0x2
#define CE1_CE_CMD_SRC_FLUSH_GET(x)                                            (((x) & CE1_CE_CMD_SRC_FLUSH_MASK) >> CE1_CE_CMD_SRC_FLUSH_LSB)
#define CE1_CE_CMD_SRC_FLUSH_SET(x)                                            (((0 | (x)) << CE1_CE_CMD_SRC_FLUSH_LSB) & CE1_CE_CMD_SRC_FLUSH_MASK)
#define CE1_CE_CMD_SRC_FLUSH_RESET                                             0x0
#define CE1_CE_CMD_HALT_LSB                                                    0
#define CE1_CE_CMD_HALT_MSB                                                    0
#define CE1_CE_CMD_HALT_MASK                                                   0x1
#define CE1_CE_CMD_HALT_GET(x)                                                 (((x) & CE1_CE_CMD_HALT_MASK) >> CE1_CE_CMD_HALT_LSB)
#define CE1_CE_CMD_HALT_SET(x)                                                 (((0 | (x)) << CE1_CE_CMD_HALT_LSB) & CE1_CE_CMD_HALT_MASK)
#define CE1_CE_CMD_HALT_RESET                                                  0x0
#define CE1_CE_CMD_ADDRESS                                                     (0x18 + __CE1_CE_REG_CSR_BASE_ADDRESS)
#define CE1_CE_CMD_RSTMASK                                                     0xf
#define CE1_CE_CMD_RESET                                                       0x0

// 0x1c (CE1_MSI_ADDR)
#define CE1_MSI_ADDR_V_LSB                                                     0
#define CE1_MSI_ADDR_V_MSB                                                     31
#define CE1_MSI_ADDR_V_MASK                                                    0xffffffff
#define CE1_MSI_ADDR_V_GET(x)                                                  (((x) & CE1_MSI_ADDR_V_MASK) >> CE1_MSI_ADDR_V_LSB)
#define CE1_MSI_ADDR_V_SET(x)                                                  (((0 | (x)) << CE1_MSI_ADDR_V_LSB) & CE1_MSI_ADDR_V_MASK)
#define CE1_MSI_ADDR_V_RESET                                                   0x0
#define CE1_MSI_ADDR_ADDRESS                                                   (0x1c + __CE1_CE_REG_CSR_BASE_ADDRESS)
#define CE1_MSI_ADDR_RSTMASK                                                   0xffffffff
#define CE1_MSI_ADDR_RESET                                                     0x0

// 0x20 (CE1_MSI_DATA)
#define CE1_MSI_DATA_V_LSB                                                     0
#define CE1_MSI_DATA_V_MSB                                                     31
#define CE1_MSI_DATA_V_MASK                                                    0xffffffff
#define CE1_MSI_DATA_V_GET(x)                                                  (((x) & CE1_MSI_DATA_V_MASK) >> CE1_MSI_DATA_V_LSB)
#define CE1_MSI_DATA_V_SET(x)                                                  (((0 | (x)) << CE1_MSI_DATA_V_LSB) & CE1_MSI_DATA_V_MASK)
#define CE1_MSI_DATA_V_RESET                                                   0x0
#define CE1_MSI_DATA_ADDRESS                                                   (0x20 + __CE1_CE_REG_CSR_BASE_ADDRESS)
#define CE1_MSI_DATA_RSTMASK                                                   0xffffffff
#define CE1_MSI_DATA_RESET                                                     0x0

// 0x24 (CE1_TARGET_IE)
#define CE1_TARGET_IE_DST_RING_LOW_WATERMARK_LSB                               4
#define CE1_TARGET_IE_DST_RING_LOW_WATERMARK_MSB                               4
#define CE1_TARGET_IE_DST_RING_LOW_WATERMARK_MASK                              0x10
#define CE1_TARGET_IE_DST_RING_LOW_WATERMARK_GET(x)                            (((x) & CE1_TARGET_IE_DST_RING_LOW_WATERMARK_MASK) >> CE1_TARGET_IE_DST_RING_LOW_WATERMARK_LSB)
#define CE1_TARGET_IE_DST_RING_LOW_WATERMARK_SET(x)                            (((0 | (x)) << CE1_TARGET_IE_DST_RING_LOW_WATERMARK_LSB) & CE1_TARGET_IE_DST_RING_LOW_WATERMARK_MASK)
#define CE1_TARGET_IE_DST_RING_LOW_WATERMARK_RESET                             0x0
#define CE1_TARGET_IE_DST_RING_HIGH_WATERMARK_LSB                              3
#define CE1_TARGET_IE_DST_RING_HIGH_WATERMARK_MSB                              3
#define CE1_TARGET_IE_DST_RING_HIGH_WATERMARK_MASK                             0x8
#define CE1_TARGET_IE_DST_RING_HIGH_WATERMARK_GET(x)                           (((x) & CE1_TARGET_IE_DST_RING_HIGH_WATERMARK_MASK) >> CE1_TARGET_IE_DST_RING_HIGH_WATERMARK_LSB)
#define CE1_TARGET_IE_DST_RING_HIGH_WATERMARK_SET(x)                           (((0 | (x)) << CE1_TARGET_IE_DST_RING_HIGH_WATERMARK_LSB) & CE1_TARGET_IE_DST_RING_HIGH_WATERMARK_MASK)
#define CE1_TARGET_IE_DST_RING_HIGH_WATERMARK_RESET                            0x0
#define CE1_TARGET_IE_SRC_RING_LOW_WATERMARK_LSB                               2
#define CE1_TARGET_IE_SRC_RING_LOW_WATERMARK_MSB                               2
#define CE1_TARGET_IE_SRC_RING_LOW_WATERMARK_MASK                              0x4
#define CE1_TARGET_IE_SRC_RING_LOW_WATERMARK_GET(x)                            (((x) & CE1_TARGET_IE_SRC_RING_LOW_WATERMARK_MASK) >> CE1_TARGET_IE_SRC_RING_LOW_WATERMARK_LSB)
#define CE1_TARGET_IE_SRC_RING_LOW_WATERMARK_SET(x)                            (((0 | (x)) << CE1_TARGET_IE_SRC_RING_LOW_WATERMARK_LSB) & CE1_TARGET_IE_SRC_RING_LOW_WATERMARK_MASK)
#define CE1_TARGET_IE_SRC_RING_LOW_WATERMARK_RESET                             0x0
#define CE1_TARGET_IE_SRC_RING_HIGH_WATERMARK_LSB                              1
#define CE1_TARGET_IE_SRC_RING_HIGH_WATERMARK_MSB                              1
#define CE1_TARGET_IE_SRC_RING_HIGH_WATERMARK_MASK                             0x2
#define CE1_TARGET_IE_SRC_RING_HIGH_WATERMARK_GET(x)                           (((x) & CE1_TARGET_IE_SRC_RING_HIGH_WATERMARK_MASK) >> CE1_TARGET_IE_SRC_RING_HIGH_WATERMARK_LSB)
#define CE1_TARGET_IE_SRC_RING_HIGH_WATERMARK_SET(x)                           (((0 | (x)) << CE1_TARGET_IE_SRC_RING_HIGH_WATERMARK_LSB) & CE1_TARGET_IE_SRC_RING_HIGH_WATERMARK_MASK)
#define CE1_TARGET_IE_SRC_RING_HIGH_WATERMARK_RESET                            0x0
#define CE1_TARGET_IE_COPY_COMPLETE_LSB                                        0
#define CE1_TARGET_IE_COPY_COMPLETE_MSB                                        0
#define CE1_TARGET_IE_COPY_COMPLETE_MASK                                       0x1
#define CE1_TARGET_IE_COPY_COMPLETE_GET(x)                                     (((x) & CE1_TARGET_IE_COPY_COMPLETE_MASK) >> CE1_TARGET_IE_COPY_COMPLETE_LSB)
#define CE1_TARGET_IE_COPY_COMPLETE_SET(x)                                     (((0 | (x)) << CE1_TARGET_IE_COPY_COMPLETE_LSB) & CE1_TARGET_IE_COPY_COMPLETE_MASK)
#define CE1_TARGET_IE_COPY_COMPLETE_RESET                                      0x0
#define CE1_TARGET_IE_ADDRESS                                                  (0x24 + __CE1_CE_REG_CSR_BASE_ADDRESS)
#define CE1_TARGET_IE_RSTMASK                                                  0x1f
#define CE1_TARGET_IE_RESET                                                    0x0

// 0x28 (CE1_TARGET_IS)
#define CE1_TARGET_IS_DST_RING_LOW_WATERMARK_LSB                               4
#define CE1_TARGET_IS_DST_RING_LOW_WATERMARK_MSB                               4
#define CE1_TARGET_IS_DST_RING_LOW_WATERMARK_MASK                              0x10
#define CE1_TARGET_IS_DST_RING_LOW_WATERMARK_GET(x)                            (((x) & CE1_TARGET_IS_DST_RING_LOW_WATERMARK_MASK) >> CE1_TARGET_IS_DST_RING_LOW_WATERMARK_LSB)
#define CE1_TARGET_IS_DST_RING_LOW_WATERMARK_SET(x)                            (((0 | (x)) << CE1_TARGET_IS_DST_RING_LOW_WATERMARK_LSB) & CE1_TARGET_IS_DST_RING_LOW_WATERMARK_MASK)
#define CE1_TARGET_IS_DST_RING_LOW_WATERMARK_RESET                             0x0
#define CE1_TARGET_IS_DST_RING_HIGH_WATERMARK_LSB                              3
#define CE1_TARGET_IS_DST_RING_HIGH_WATERMARK_MSB                              3
#define CE1_TARGET_IS_DST_RING_HIGH_WATERMARK_MASK                             0x8
#define CE1_TARGET_IS_DST_RING_HIGH_WATERMARK_GET(x)                           (((x) & CE1_TARGET_IS_DST_RING_HIGH_WATERMARK_MASK) >> CE1_TARGET_IS_DST_RING_HIGH_WATERMARK_LSB)
#define CE1_TARGET_IS_DST_RING_HIGH_WATERMARK_SET(x)                           (((0 | (x)) << CE1_TARGET_IS_DST_RING_HIGH_WATERMARK_LSB) & CE1_TARGET_IS_DST_RING_HIGH_WATERMARK_MASK)
#define CE1_TARGET_IS_DST_RING_HIGH_WATERMARK_RESET                            0x0
#define CE1_TARGET_IS_SRC_RING_LOW_WATERMARK_LSB                               2
#define CE1_TARGET_IS_SRC_RING_LOW_WATERMARK_MSB                               2
#define CE1_TARGET_IS_SRC_RING_LOW_WATERMARK_MASK                              0x4
#define CE1_TARGET_IS_SRC_RING_LOW_WATERMARK_GET(x)                            (((x) & CE1_TARGET_IS_SRC_RING_LOW_WATERMARK_MASK) >> CE1_TARGET_IS_SRC_RING_LOW_WATERMARK_LSB)
#define CE1_TARGET_IS_SRC_RING_LOW_WATERMARK_SET(x)                            (((0 | (x)) << CE1_TARGET_IS_SRC_RING_LOW_WATERMARK_LSB) & CE1_TARGET_IS_SRC_RING_LOW_WATERMARK_MASK)
#define CE1_TARGET_IS_SRC_RING_LOW_WATERMARK_RESET                             0x0
#define CE1_TARGET_IS_SRC_RING_HIGH_WATERMARK_LSB                              1
#define CE1_TARGET_IS_SRC_RING_HIGH_WATERMARK_MSB                              1
#define CE1_TARGET_IS_SRC_RING_HIGH_WATERMARK_MASK                             0x2
#define CE1_TARGET_IS_SRC_RING_HIGH_WATERMARK_GET(x)                           (((x) & CE1_TARGET_IS_SRC_RING_HIGH_WATERMARK_MASK) >> CE1_TARGET_IS_SRC_RING_HIGH_WATERMARK_LSB)
#define CE1_TARGET_IS_SRC_RING_HIGH_WATERMARK_SET(x)                           (((0 | (x)) << CE1_TARGET_IS_SRC_RING_HIGH_WATERMARK_LSB) & CE1_TARGET_IS_SRC_RING_HIGH_WATERMARK_MASK)
#define CE1_TARGET_IS_SRC_RING_HIGH_WATERMARK_RESET                            0x0
#define CE1_TARGET_IS_COPY_COMPLETE_LSB                                        0
#define CE1_TARGET_IS_COPY_COMPLETE_MSB                                        0
#define CE1_TARGET_IS_COPY_COMPLETE_MASK                                       0x1
#define CE1_TARGET_IS_COPY_COMPLETE_GET(x)                                     (((x) & CE1_TARGET_IS_COPY_COMPLETE_MASK) >> CE1_TARGET_IS_COPY_COMPLETE_LSB)
#define CE1_TARGET_IS_COPY_COMPLETE_SET(x)                                     (((0 | (x)) << CE1_TARGET_IS_COPY_COMPLETE_LSB) & CE1_TARGET_IS_COPY_COMPLETE_MASK)
#define CE1_TARGET_IS_COPY_COMPLETE_RESET                                      0x0
#define CE1_TARGET_IS_ADDRESS                                                  (0x28 + __CE1_CE_REG_CSR_BASE_ADDRESS)
#define CE1_TARGET_IS_RSTMASK                                                  0x1f
#define CE1_TARGET_IS_RESET                                                    0x0

// 0x2c (CE1_HOST_IE)
#define CE1_HOST_IE_DST_RING_LOW_WATERMARK_LSB                                 4
#define CE1_HOST_IE_DST_RING_LOW_WATERMARK_MSB                                 4
#define CE1_HOST_IE_DST_RING_LOW_WATERMARK_MASK                                0x10
#define CE1_HOST_IE_DST_RING_LOW_WATERMARK_GET(x)                              (((x) & CE1_HOST_IE_DST_RING_LOW_WATERMARK_MASK) >> CE1_HOST_IE_DST_RING_LOW_WATERMARK_LSB)
#define CE1_HOST_IE_DST_RING_LOW_WATERMARK_SET(x)                              (((0 | (x)) << CE1_HOST_IE_DST_RING_LOW_WATERMARK_LSB) & CE1_HOST_IE_DST_RING_LOW_WATERMARK_MASK)
#define CE1_HOST_IE_DST_RING_LOW_WATERMARK_RESET                               0x0
#define CE1_HOST_IE_DST_RING_HIGH_WATERMARK_LSB                                3
#define CE1_HOST_IE_DST_RING_HIGH_WATERMARK_MSB                                3
#define CE1_HOST_IE_DST_RING_HIGH_WATERMARK_MASK                               0x8
#define CE1_HOST_IE_DST_RING_HIGH_WATERMARK_GET(x)                             (((x) & CE1_HOST_IE_DST_RING_HIGH_WATERMARK_MASK) >> CE1_HOST_IE_DST_RING_HIGH_WATERMARK_LSB)
#define CE1_HOST_IE_DST_RING_HIGH_WATERMARK_SET(x)                             (((0 | (x)) << CE1_HOST_IE_DST_RING_HIGH_WATERMARK_LSB) & CE1_HOST_IE_DST_RING_HIGH_WATERMARK_MASK)
#define CE1_HOST_IE_DST_RING_HIGH_WATERMARK_RESET                              0x0
#define CE1_HOST_IE_SRC_RING_LOW_WATERMARK_LSB                                 2
#define CE1_HOST_IE_SRC_RING_LOW_WATERMARK_MSB                                 2
#define CE1_HOST_IE_SRC_RING_LOW_WATERMARK_MASK                                0x4
#define CE1_HOST_IE_SRC_RING_LOW_WATERMARK_GET(x)                              (((x) & CE1_HOST_IE_SRC_RING_LOW_WATERMARK_MASK) >> CE1_HOST_IE_SRC_RING_LOW_WATERMARK_LSB)
#define CE1_HOST_IE_SRC_RING_LOW_WATERMARK_SET(x)                              (((0 | (x)) << CE1_HOST_IE_SRC_RING_LOW_WATERMARK_LSB) & CE1_HOST_IE_SRC_RING_LOW_WATERMARK_MASK)
#define CE1_HOST_IE_SRC_RING_LOW_WATERMARK_RESET                               0x0
#define CE1_HOST_IE_SRC_RING_HIGH_WATERMARK_LSB                                1
#define CE1_HOST_IE_SRC_RING_HIGH_WATERMARK_MSB                                1
#define CE1_HOST_IE_SRC_RING_HIGH_WATERMARK_MASK                               0x2
#define CE1_HOST_IE_SRC_RING_HIGH_WATERMARK_GET(x)                             (((x) & CE1_HOST_IE_SRC_RING_HIGH_WATERMARK_MASK) >> CE1_HOST_IE_SRC_RING_HIGH_WATERMARK_LSB)
#define CE1_HOST_IE_SRC_RING_HIGH_WATERMARK_SET(x)                             (((0 | (x)) << CE1_HOST_IE_SRC_RING_HIGH_WATERMARK_LSB) & CE1_HOST_IE_SRC_RING_HIGH_WATERMARK_MASK)
#define CE1_HOST_IE_SRC_RING_HIGH_WATERMARK_RESET                              0x0
#define CE1_HOST_IE_COPY_COMPLETE_LSB                                          0
#define CE1_HOST_IE_COPY_COMPLETE_MSB                                          0
#define CE1_HOST_IE_COPY_COMPLETE_MASK                                         0x1
#define CE1_HOST_IE_COPY_COMPLETE_GET(x)                                       (((x) & CE1_HOST_IE_COPY_COMPLETE_MASK) >> CE1_HOST_IE_COPY_COMPLETE_LSB)
#define CE1_HOST_IE_COPY_COMPLETE_SET(x)                                       (((0 | (x)) << CE1_HOST_IE_COPY_COMPLETE_LSB) & CE1_HOST_IE_COPY_COMPLETE_MASK)
#define CE1_HOST_IE_COPY_COMPLETE_RESET                                        0x0
#define CE1_HOST_IE_ADDRESS                                                    (0x2c + __CE1_CE_REG_CSR_BASE_ADDRESS)
#define CE1_HOST_IE_RSTMASK                                                    0x1f
#define CE1_HOST_IE_RESET                                                      0x0

// 0x30 (CE1_HOST_IS)
#define CE1_HOST_IS_DST_RING_LOW_WATERMARK_LSB                                 4
#define CE1_HOST_IS_DST_RING_LOW_WATERMARK_MSB                                 4
#define CE1_HOST_IS_DST_RING_LOW_WATERMARK_MASK                                0x10
#define CE1_HOST_IS_DST_RING_LOW_WATERMARK_GET(x)                              (((x) & CE1_HOST_IS_DST_RING_LOW_WATERMARK_MASK) >> CE1_HOST_IS_DST_RING_LOW_WATERMARK_LSB)
#define CE1_HOST_IS_DST_RING_LOW_WATERMARK_SET(x)                              (((0 | (x)) << CE1_HOST_IS_DST_RING_LOW_WATERMARK_LSB) & CE1_HOST_IS_DST_RING_LOW_WATERMARK_MASK)
#define CE1_HOST_IS_DST_RING_LOW_WATERMARK_RESET                               0x0
#define CE1_HOST_IS_DST_RING_HIGH_WATERMARK_LSB                                3
#define CE1_HOST_IS_DST_RING_HIGH_WATERMARK_MSB                                3
#define CE1_HOST_IS_DST_RING_HIGH_WATERMARK_MASK                               0x8
#define CE1_HOST_IS_DST_RING_HIGH_WATERMARK_GET(x)                             (((x) & CE1_HOST_IS_DST_RING_HIGH_WATERMARK_MASK) >> CE1_HOST_IS_DST_RING_HIGH_WATERMARK_LSB)
#define CE1_HOST_IS_DST_RING_HIGH_WATERMARK_SET(x)                             (((0 | (x)) << CE1_HOST_IS_DST_RING_HIGH_WATERMARK_LSB) & CE1_HOST_IS_DST_RING_HIGH_WATERMARK_MASK)
#define CE1_HOST_IS_DST_RING_HIGH_WATERMARK_RESET                              0x0
#define CE1_HOST_IS_SRC_RING_LOW_WATERMARK_LSB                                 2
#define CE1_HOST_IS_SRC_RING_LOW_WATERMARK_MSB                                 2
#define CE1_HOST_IS_SRC_RING_LOW_WATERMARK_MASK                                0x4
#define CE1_HOST_IS_SRC_RING_LOW_WATERMARK_GET(x)                              (((x) & CE1_HOST_IS_SRC_RING_LOW_WATERMARK_MASK) >> CE1_HOST_IS_SRC_RING_LOW_WATERMARK_LSB)
#define CE1_HOST_IS_SRC_RING_LOW_WATERMARK_SET(x)                              (((0 | (x)) << CE1_HOST_IS_SRC_RING_LOW_WATERMARK_LSB) & CE1_HOST_IS_SRC_RING_LOW_WATERMARK_MASK)
#define CE1_HOST_IS_SRC_RING_LOW_WATERMARK_RESET                               0x0
#define CE1_HOST_IS_SRC_RING_HIGH_WATERMARK_LSB                                1
#define CE1_HOST_IS_SRC_RING_HIGH_WATERMARK_MSB                                1
#define CE1_HOST_IS_SRC_RING_HIGH_WATERMARK_MASK                               0x2
#define CE1_HOST_IS_SRC_RING_HIGH_WATERMARK_GET(x)                             (((x) & CE1_HOST_IS_SRC_RING_HIGH_WATERMARK_MASK) >> CE1_HOST_IS_SRC_RING_HIGH_WATERMARK_LSB)
#define CE1_HOST_IS_SRC_RING_HIGH_WATERMARK_SET(x)                             (((0 | (x)) << CE1_HOST_IS_SRC_RING_HIGH_WATERMARK_LSB) & CE1_HOST_IS_SRC_RING_HIGH_WATERMARK_MASK)
#define CE1_HOST_IS_SRC_RING_HIGH_WATERMARK_RESET                              0x0
#define CE1_HOST_IS_COPY_COMPLETE_LSB                                          0
#define CE1_HOST_IS_COPY_COMPLETE_MSB                                          0
#define CE1_HOST_IS_COPY_COMPLETE_MASK                                         0x1
#define CE1_HOST_IS_COPY_COMPLETE_GET(x)                                       (((x) & CE1_HOST_IS_COPY_COMPLETE_MASK) >> CE1_HOST_IS_COPY_COMPLETE_LSB)
#define CE1_HOST_IS_COPY_COMPLETE_SET(x)                                       (((0 | (x)) << CE1_HOST_IS_COPY_COMPLETE_LSB) & CE1_HOST_IS_COPY_COMPLETE_MASK)
#define CE1_HOST_IS_COPY_COMPLETE_RESET                                        0x0
#define CE1_HOST_IS_ADDRESS                                                    (0x30 + __CE1_CE_REG_CSR_BASE_ADDRESS)
#define CE1_HOST_IS_RSTMASK                                                    0x1f
#define CE1_HOST_IS_RESET                                                      0x0

// 0x34 (CE1_MISC_IE)
#define CE1_MISC_IE_AXI_ERR_LSB                                                10
#define CE1_MISC_IE_AXI_ERR_MSB                                                10
#define CE1_MISC_IE_AXI_ERR_MASK                                               0x400
#define CE1_MISC_IE_AXI_ERR_GET(x)                                             (((x) & CE1_MISC_IE_AXI_ERR_MASK) >> CE1_MISC_IE_AXI_ERR_LSB)
#define CE1_MISC_IE_AXI_ERR_SET(x)                                             (((0 | (x)) << CE1_MISC_IE_AXI_ERR_LSB) & CE1_MISC_IE_AXI_ERR_MASK)
#define CE1_MISC_IE_AXI_ERR_RESET                                              0x0
#define CE1_MISC_IE_DST_ADDR_ERR_LSB                                           9
#define CE1_MISC_IE_DST_ADDR_ERR_MSB                                           9
#define CE1_MISC_IE_DST_ADDR_ERR_MASK                                          0x200
#define CE1_MISC_IE_DST_ADDR_ERR_GET(x)                                        (((x) & CE1_MISC_IE_DST_ADDR_ERR_MASK) >> CE1_MISC_IE_DST_ADDR_ERR_LSB)
#define CE1_MISC_IE_DST_ADDR_ERR_SET(x)                                        (((0 | (x)) << CE1_MISC_IE_DST_ADDR_ERR_LSB) & CE1_MISC_IE_DST_ADDR_ERR_MASK)
#define CE1_MISC_IE_DST_ADDR_ERR_RESET                                         0x0
#define CE1_MISC_IE_SRC_LEN_ERR_LSB                                            8
#define CE1_MISC_IE_SRC_LEN_ERR_MSB                                            8
#define CE1_MISC_IE_SRC_LEN_ERR_MASK                                           0x100
#define CE1_MISC_IE_SRC_LEN_ERR_GET(x)                                         (((x) & CE1_MISC_IE_SRC_LEN_ERR_MASK) >> CE1_MISC_IE_SRC_LEN_ERR_LSB)
#define CE1_MISC_IE_SRC_LEN_ERR_SET(x)                                         (((0 | (x)) << CE1_MISC_IE_SRC_LEN_ERR_LSB) & CE1_MISC_IE_SRC_LEN_ERR_MASK)
#define CE1_MISC_IE_SRC_LEN_ERR_RESET                                          0x0
#define CE1_MISC_IE_DST_MAX_LEN_VIO_LSB                                        7
#define CE1_MISC_IE_DST_MAX_LEN_VIO_MSB                                        7
#define CE1_MISC_IE_DST_MAX_LEN_VIO_MASK                                       0x80
#define CE1_MISC_IE_DST_MAX_LEN_VIO_GET(x)                                     (((x) & CE1_MISC_IE_DST_MAX_LEN_VIO_MASK) >> CE1_MISC_IE_DST_MAX_LEN_VIO_LSB)
#define CE1_MISC_IE_DST_MAX_LEN_VIO_SET(x)                                     (((0 | (x)) << CE1_MISC_IE_DST_MAX_LEN_VIO_LSB) & CE1_MISC_IE_DST_MAX_LEN_VIO_MASK)
#define CE1_MISC_IE_DST_MAX_LEN_VIO_RESET                                      0x0
#define CE1_MISC_IE_DST_RING_OVERFLOW_LSB                                      6
#define CE1_MISC_IE_DST_RING_OVERFLOW_MSB                                      6
#define CE1_MISC_IE_DST_RING_OVERFLOW_MASK                                     0x40
#define CE1_MISC_IE_DST_RING_OVERFLOW_GET(x)                                   (((x) & CE1_MISC_IE_DST_RING_OVERFLOW_MASK) >> CE1_MISC_IE_DST_RING_OVERFLOW_LSB)
#define CE1_MISC_IE_DST_RING_OVERFLOW_SET(x)                                   (((0 | (x)) << CE1_MISC_IE_DST_RING_OVERFLOW_LSB) & CE1_MISC_IE_DST_RING_OVERFLOW_MASK)
#define CE1_MISC_IE_DST_RING_OVERFLOW_RESET                                    0x0
#define CE1_MISC_IE_SRC_RING_OVERFLOW_LSB                                      5
#define CE1_MISC_IE_SRC_RING_OVERFLOW_MSB                                      5
#define CE1_MISC_IE_SRC_RING_OVERFLOW_MASK                                     0x20
#define CE1_MISC_IE_SRC_RING_OVERFLOW_GET(x)                                   (((x) & CE1_MISC_IE_SRC_RING_OVERFLOW_MASK) >> CE1_MISC_IE_SRC_RING_OVERFLOW_LSB)
#define CE1_MISC_IE_SRC_RING_OVERFLOW_SET(x)                                   (((0 | (x)) << CE1_MISC_IE_SRC_RING_OVERFLOW_LSB) & CE1_MISC_IE_SRC_RING_OVERFLOW_MASK)
#define CE1_MISC_IE_SRC_RING_OVERFLOW_RESET                                    0x0
#define CE1_MISC_IE_DST_RING_LOW_WATERMARK_LSB                                 4
#define CE1_MISC_IE_DST_RING_LOW_WATERMARK_MSB                                 4
#define CE1_MISC_IE_DST_RING_LOW_WATERMARK_MASK                                0x10
#define CE1_MISC_IE_DST_RING_LOW_WATERMARK_GET(x)                              (((x) & CE1_MISC_IE_DST_RING_LOW_WATERMARK_MASK) >> CE1_MISC_IE_DST_RING_LOW_WATERMARK_LSB)
#define CE1_MISC_IE_DST_RING_LOW_WATERMARK_SET(x)                              (((0 | (x)) << CE1_MISC_IE_DST_RING_LOW_WATERMARK_LSB) & CE1_MISC_IE_DST_RING_LOW_WATERMARK_MASK)
#define CE1_MISC_IE_DST_RING_LOW_WATERMARK_RESET                               0x0
#define CE1_MISC_IE_DST_RING_HIGH_WATERMARK_LSB                                3
#define CE1_MISC_IE_DST_RING_HIGH_WATERMARK_MSB                                3
#define CE1_MISC_IE_DST_RING_HIGH_WATERMARK_MASK                               0x8
#define CE1_MISC_IE_DST_RING_HIGH_WATERMARK_GET(x)                             (((x) & CE1_MISC_IE_DST_RING_HIGH_WATERMARK_MASK) >> CE1_MISC_IE_DST_RING_HIGH_WATERMARK_LSB)
#define CE1_MISC_IE_DST_RING_HIGH_WATERMARK_SET(x)                             (((0 | (x)) << CE1_MISC_IE_DST_RING_HIGH_WATERMARK_LSB) & CE1_MISC_IE_DST_RING_HIGH_WATERMARK_MASK)
#define CE1_MISC_IE_DST_RING_HIGH_WATERMARK_RESET                              0x0
#define CE1_MISC_IE_SRC_RING_LOW_WATERMARK_LSB                                 2
#define CE1_MISC_IE_SRC_RING_LOW_WATERMARK_MSB                                 2
#define CE1_MISC_IE_SRC_RING_LOW_WATERMARK_MASK                                0x4
#define CE1_MISC_IE_SRC_RING_LOW_WATERMARK_GET(x)                              (((x) & CE1_MISC_IE_SRC_RING_LOW_WATERMARK_MASK) >> CE1_MISC_IE_SRC_RING_LOW_WATERMARK_LSB)
#define CE1_MISC_IE_SRC_RING_LOW_WATERMARK_SET(x)                              (((0 | (x)) << CE1_MISC_IE_SRC_RING_LOW_WATERMARK_LSB) & CE1_MISC_IE_SRC_RING_LOW_WATERMARK_MASK)
#define CE1_MISC_IE_SRC_RING_LOW_WATERMARK_RESET                               0x0
#define CE1_MISC_IE_SRC_RING_HIGH_WATERMARK_LSB                                1
#define CE1_MISC_IE_SRC_RING_HIGH_WATERMARK_MSB                                1
#define CE1_MISC_IE_SRC_RING_HIGH_WATERMARK_MASK                               0x2
#define CE1_MISC_IE_SRC_RING_HIGH_WATERMARK_GET(x)                             (((x) & CE1_MISC_IE_SRC_RING_HIGH_WATERMARK_MASK) >> CE1_MISC_IE_SRC_RING_HIGH_WATERMARK_LSB)
#define CE1_MISC_IE_SRC_RING_HIGH_WATERMARK_SET(x)                             (((0 | (x)) << CE1_MISC_IE_SRC_RING_HIGH_WATERMARK_LSB) & CE1_MISC_IE_SRC_RING_HIGH_WATERMARK_MASK)
#define CE1_MISC_IE_SRC_RING_HIGH_WATERMARK_RESET                              0x0
#define CE1_MISC_IE_COPY_COMPLETE_LSB                                          0
#define CE1_MISC_IE_COPY_COMPLETE_MSB                                          0
#define CE1_MISC_IE_COPY_COMPLETE_MASK                                         0x1
#define CE1_MISC_IE_COPY_COMPLETE_GET(x)                                       (((x) & CE1_MISC_IE_COPY_COMPLETE_MASK) >> CE1_MISC_IE_COPY_COMPLETE_LSB)
#define CE1_MISC_IE_COPY_COMPLETE_SET(x)                                       (((0 | (x)) << CE1_MISC_IE_COPY_COMPLETE_LSB) & CE1_MISC_IE_COPY_COMPLETE_MASK)
#define CE1_MISC_IE_COPY_COMPLETE_RESET                                        0x0
#define CE1_MISC_IE_ADDRESS                                                    (0x34 + __CE1_CE_REG_CSR_BASE_ADDRESS)
#define CE1_MISC_IE_RSTMASK                                                    0x7ff
#define CE1_MISC_IE_RESET                                                      0x0

// 0x38 (CE1_MISC_IS)
#define CE1_MISC_IS_AXI_ERR_LSB                                                10
#define CE1_MISC_IS_AXI_ERR_MSB                                                10
#define CE1_MISC_IS_AXI_ERR_MASK                                               0x400
#define CE1_MISC_IS_AXI_ERR_GET(x)                                             (((x) & CE1_MISC_IS_AXI_ERR_MASK) >> CE1_MISC_IS_AXI_ERR_LSB)
#define CE1_MISC_IS_AXI_ERR_SET(x)                                             (((0 | (x)) << CE1_MISC_IS_AXI_ERR_LSB) & CE1_MISC_IS_AXI_ERR_MASK)
#define CE1_MISC_IS_AXI_ERR_RESET                                              0x0
#define CE1_MISC_IS_DST_ADDR_ERR_LSB                                           9
#define CE1_MISC_IS_DST_ADDR_ERR_MSB                                           9
#define CE1_MISC_IS_DST_ADDR_ERR_MASK                                          0x200
#define CE1_MISC_IS_DST_ADDR_ERR_GET(x)                                        (((x) & CE1_MISC_IS_DST_ADDR_ERR_MASK) >> CE1_MISC_IS_DST_ADDR_ERR_LSB)
#define CE1_MISC_IS_DST_ADDR_ERR_SET(x)                                        (((0 | (x)) << CE1_MISC_IS_DST_ADDR_ERR_LSB) & CE1_MISC_IS_DST_ADDR_ERR_MASK)
#define CE1_MISC_IS_DST_ADDR_ERR_RESET                                         0x0
#define CE1_MISC_IS_SRC_LEN_ERR_LSB                                            8
#define CE1_MISC_IS_SRC_LEN_ERR_MSB                                            8
#define CE1_MISC_IS_SRC_LEN_ERR_MASK                                           0x100
#define CE1_MISC_IS_SRC_LEN_ERR_GET(x)                                         (((x) & CE1_MISC_IS_SRC_LEN_ERR_MASK) >> CE1_MISC_IS_SRC_LEN_ERR_LSB)
#define CE1_MISC_IS_SRC_LEN_ERR_SET(x)                                         (((0 | (x)) << CE1_MISC_IS_SRC_LEN_ERR_LSB) & CE1_MISC_IS_SRC_LEN_ERR_MASK)
#define CE1_MISC_IS_SRC_LEN_ERR_RESET                                          0x0
#define CE1_MISC_IS_DST_MAX_LEN_VIO_LSB                                        7
#define CE1_MISC_IS_DST_MAX_LEN_VIO_MSB                                        7
#define CE1_MISC_IS_DST_MAX_LEN_VIO_MASK                                       0x80
#define CE1_MISC_IS_DST_MAX_LEN_VIO_GET(x)                                     (((x) & CE1_MISC_IS_DST_MAX_LEN_VIO_MASK) >> CE1_MISC_IS_DST_MAX_LEN_VIO_LSB)
#define CE1_MISC_IS_DST_MAX_LEN_VIO_SET(x)                                     (((0 | (x)) << CE1_MISC_IS_DST_MAX_LEN_VIO_LSB) & CE1_MISC_IS_DST_MAX_LEN_VIO_MASK)
#define CE1_MISC_IS_DST_MAX_LEN_VIO_RESET                                      0x0
#define CE1_MISC_IS_DST_RING_OVERFLOW_LSB                                      6
#define CE1_MISC_IS_DST_RING_OVERFLOW_MSB                                      6
#define CE1_MISC_IS_DST_RING_OVERFLOW_MASK                                     0x40
#define CE1_MISC_IS_DST_RING_OVERFLOW_GET(x)                                   (((x) & CE1_MISC_IS_DST_RING_OVERFLOW_MASK) >> CE1_MISC_IS_DST_RING_OVERFLOW_LSB)
#define CE1_MISC_IS_DST_RING_OVERFLOW_SET(x)                                   (((0 | (x)) << CE1_MISC_IS_DST_RING_OVERFLOW_LSB) & CE1_MISC_IS_DST_RING_OVERFLOW_MASK)
#define CE1_MISC_IS_DST_RING_OVERFLOW_RESET                                    0x0
#define CE1_MISC_IS_SRC_RING_OVERFLOW_LSB                                      5
#define CE1_MISC_IS_SRC_RING_OVERFLOW_MSB                                      5
#define CE1_MISC_IS_SRC_RING_OVERFLOW_MASK                                     0x20
#define CE1_MISC_IS_SRC_RING_OVERFLOW_GET(x)                                   (((x) & CE1_MISC_IS_SRC_RING_OVERFLOW_MASK) >> CE1_MISC_IS_SRC_RING_OVERFLOW_LSB)
#define CE1_MISC_IS_SRC_RING_OVERFLOW_SET(x)                                   (((0 | (x)) << CE1_MISC_IS_SRC_RING_OVERFLOW_LSB) & CE1_MISC_IS_SRC_RING_OVERFLOW_MASK)
#define CE1_MISC_IS_SRC_RING_OVERFLOW_RESET                                    0x0
#define CE1_MISC_IS_DST_RING_LOW_WATERMARK_LSB                                 4
#define CE1_MISC_IS_DST_RING_LOW_WATERMARK_MSB                                 4
#define CE1_MISC_IS_DST_RING_LOW_WATERMARK_MASK                                0x10
#define CE1_MISC_IS_DST_RING_LOW_WATERMARK_GET(x)                              (((x) & CE1_MISC_IS_DST_RING_LOW_WATERMARK_MASK) >> CE1_MISC_IS_DST_RING_LOW_WATERMARK_LSB)
#define CE1_MISC_IS_DST_RING_LOW_WATERMARK_SET(x)                              (((0 | (x)) << CE1_MISC_IS_DST_RING_LOW_WATERMARK_LSB) & CE1_MISC_IS_DST_RING_LOW_WATERMARK_MASK)
#define CE1_MISC_IS_DST_RING_LOW_WATERMARK_RESET                               0x0
#define CE1_MISC_IS_DST_RING_HIGH_WATERMARK_LSB                                3
#define CE1_MISC_IS_DST_RING_HIGH_WATERMARK_MSB                                3
#define CE1_MISC_IS_DST_RING_HIGH_WATERMARK_MASK                               0x8
#define CE1_MISC_IS_DST_RING_HIGH_WATERMARK_GET(x)                             (((x) & CE1_MISC_IS_DST_RING_HIGH_WATERMARK_MASK) >> CE1_MISC_IS_DST_RING_HIGH_WATERMARK_LSB)
#define CE1_MISC_IS_DST_RING_HIGH_WATERMARK_SET(x)                             (((0 | (x)) << CE1_MISC_IS_DST_RING_HIGH_WATERMARK_LSB) & CE1_MISC_IS_DST_RING_HIGH_WATERMARK_MASK)
#define CE1_MISC_IS_DST_RING_HIGH_WATERMARK_RESET                              0x0
#define CE1_MISC_IS_SRC_RING_LOW_WATERMARK_LSB                                 2
#define CE1_MISC_IS_SRC_RING_LOW_WATERMARK_MSB                                 2
#define CE1_MISC_IS_SRC_RING_LOW_WATERMARK_MASK                                0x4
#define CE1_MISC_IS_SRC_RING_LOW_WATERMARK_GET(x)                              (((x) & CE1_MISC_IS_SRC_RING_LOW_WATERMARK_MASK) >> CE1_MISC_IS_SRC_RING_LOW_WATERMARK_LSB)
#define CE1_MISC_IS_SRC_RING_LOW_WATERMARK_SET(x)                              (((0 | (x)) << CE1_MISC_IS_SRC_RING_LOW_WATERMARK_LSB) & CE1_MISC_IS_SRC_RING_LOW_WATERMARK_MASK)
#define CE1_MISC_IS_SRC_RING_LOW_WATERMARK_RESET                               0x0
#define CE1_MISC_IS_SRC_RING_HIGH_WATERMARK_LSB                                1
#define CE1_MISC_IS_SRC_RING_HIGH_WATERMARK_MSB                                1
#define CE1_MISC_IS_SRC_RING_HIGH_WATERMARK_MASK                               0x2
#define CE1_MISC_IS_SRC_RING_HIGH_WATERMARK_GET(x)                             (((x) & CE1_MISC_IS_SRC_RING_HIGH_WATERMARK_MASK) >> CE1_MISC_IS_SRC_RING_HIGH_WATERMARK_LSB)
#define CE1_MISC_IS_SRC_RING_HIGH_WATERMARK_SET(x)                             (((0 | (x)) << CE1_MISC_IS_SRC_RING_HIGH_WATERMARK_LSB) & CE1_MISC_IS_SRC_RING_HIGH_WATERMARK_MASK)
#define CE1_MISC_IS_SRC_RING_HIGH_WATERMARK_RESET                              0x0
#define CE1_MISC_IS_COPY_COMPLETE_LSB                                          0
#define CE1_MISC_IS_COPY_COMPLETE_MSB                                          0
#define CE1_MISC_IS_COPY_COMPLETE_MASK                                         0x1
#define CE1_MISC_IS_COPY_COMPLETE_GET(x)                                       (((x) & CE1_MISC_IS_COPY_COMPLETE_MASK) >> CE1_MISC_IS_COPY_COMPLETE_LSB)
#define CE1_MISC_IS_COPY_COMPLETE_SET(x)                                       (((0 | (x)) << CE1_MISC_IS_COPY_COMPLETE_LSB) & CE1_MISC_IS_COPY_COMPLETE_MASK)
#define CE1_MISC_IS_COPY_COMPLETE_RESET                                        0x0
#define CE1_MISC_IS_ADDRESS                                                    (0x38 + __CE1_CE_REG_CSR_BASE_ADDRESS)
#define CE1_MISC_IS_RSTMASK                                                    0x7ff
#define CE1_MISC_IS_RESET                                                      0x0

// 0x3c (CE1_SR_WR_INDEX)
#define CE1_SR_WR_INDEX_V_LSB                                                  0
#define CE1_SR_WR_INDEX_V_MSB                                                  15
#define CE1_SR_WR_INDEX_V_MASK                                                 0xffff
#define CE1_SR_WR_INDEX_V_GET(x)                                               (((x) & CE1_SR_WR_INDEX_V_MASK) >> CE1_SR_WR_INDEX_V_LSB)
#define CE1_SR_WR_INDEX_V_SET(x)                                               (((0 | (x)) << CE1_SR_WR_INDEX_V_LSB) & CE1_SR_WR_INDEX_V_MASK)
#define CE1_SR_WR_INDEX_V_RESET                                                0x0
#define CE1_SR_WR_INDEX_ADDRESS                                                (0x3c + __CE1_CE_REG_CSR_BASE_ADDRESS)
#define CE1_SR_WR_INDEX_RSTMASK                                                0xffff
#define CE1_SR_WR_INDEX_RESET                                                  0x0

// 0x40 (CE1_DST_WR_INDEX)
#define CE1_DST_WR_INDEX_V_LSB                                                 0
#define CE1_DST_WR_INDEX_V_MSB                                                 15
#define CE1_DST_WR_INDEX_V_MASK                                                0xffff
#define CE1_DST_WR_INDEX_V_GET(x)                                              (((x) & CE1_DST_WR_INDEX_V_MASK) >> CE1_DST_WR_INDEX_V_LSB)
#define CE1_DST_WR_INDEX_V_SET(x)                                              (((0 | (x)) << CE1_DST_WR_INDEX_V_LSB) & CE1_DST_WR_INDEX_V_MASK)
#define CE1_DST_WR_INDEX_V_RESET                                               0x0
#define CE1_DST_WR_INDEX_ADDRESS                                               (0x40 + __CE1_CE_REG_CSR_BASE_ADDRESS)
#define CE1_DST_WR_INDEX_RSTMASK                                               0xffff
#define CE1_DST_WR_INDEX_RESET                                                 0x0

// 0x44 (CE1_CURRENT_SRRI)
#define CE1_CURRENT_SRRI_V_LSB                                                 0
#define CE1_CURRENT_SRRI_V_MSB                                                 15
#define CE1_CURRENT_SRRI_V_MASK                                                0xffff
#define CE1_CURRENT_SRRI_V_GET(x)                                              (((x) & CE1_CURRENT_SRRI_V_MASK) >> CE1_CURRENT_SRRI_V_LSB)
#define CE1_CURRENT_SRRI_V_SET(x)                                              (((0 | (x)) << CE1_CURRENT_SRRI_V_LSB) & CE1_CURRENT_SRRI_V_MASK)
#define CE1_CURRENT_SRRI_V_RESET                                               0x0
#define CE1_CURRENT_SRRI_ADDRESS                                               (0x44 + __CE1_CE_REG_CSR_BASE_ADDRESS)
#define CE1_CURRENT_SRRI_RSTMASK                                               0xffff
#define CE1_CURRENT_SRRI_RESET                                                 0x0

// 0x48 (CE1_CURRENT_DRRI)
#define CE1_CURRENT_DRRI_V_LSB                                                 0
#define CE1_CURRENT_DRRI_V_MSB                                                 15
#define CE1_CURRENT_DRRI_V_MASK                                                0xffff
#define CE1_CURRENT_DRRI_V_GET(x)                                              (((x) & CE1_CURRENT_DRRI_V_MASK) >> CE1_CURRENT_DRRI_V_LSB)
#define CE1_CURRENT_DRRI_V_SET(x)                                              (((0 | (x)) << CE1_CURRENT_DRRI_V_LSB) & CE1_CURRENT_DRRI_V_MASK)
#define CE1_CURRENT_DRRI_V_RESET                                               0x0
#define CE1_CURRENT_DRRI_ADDRESS                                               (0x48 + __CE1_CE_REG_CSR_BASE_ADDRESS)
#define CE1_CURRENT_DRRI_RSTMASK                                               0xffff
#define CE1_CURRENT_DRRI_RESET                                                 0x0

// 0x4c (CE1_SRC_WATERMARK)
#define CE1_SRC_WATERMARK_LOW_LSB                                              16
#define CE1_SRC_WATERMARK_LOW_MSB                                              31
#define CE1_SRC_WATERMARK_LOW_MASK                                             0xffff0000
#define CE1_SRC_WATERMARK_LOW_GET(x)                                           (((x) & CE1_SRC_WATERMARK_LOW_MASK) >> CE1_SRC_WATERMARK_LOW_LSB)
#define CE1_SRC_WATERMARK_LOW_SET(x)                                           (((0 | (x)) << CE1_SRC_WATERMARK_LOW_LSB) & CE1_SRC_WATERMARK_LOW_MASK)
#define CE1_SRC_WATERMARK_LOW_RESET                                            0x0
#define CE1_SRC_WATERMARK_HIGH_LSB                                             0
#define CE1_SRC_WATERMARK_HIGH_MSB                                             15
#define CE1_SRC_WATERMARK_HIGH_MASK                                            0xffff
#define CE1_SRC_WATERMARK_HIGH_GET(x)                                          (((x) & CE1_SRC_WATERMARK_HIGH_MASK) >> CE1_SRC_WATERMARK_HIGH_LSB)
#define CE1_SRC_WATERMARK_HIGH_SET(x)                                          (((0 | (x)) << CE1_SRC_WATERMARK_HIGH_LSB) & CE1_SRC_WATERMARK_HIGH_MASK)
#define CE1_SRC_WATERMARK_HIGH_RESET                                           0x0
#define CE1_SRC_WATERMARK_ADDRESS                                              (0x4c + __CE1_CE_REG_CSR_BASE_ADDRESS)
#define CE1_SRC_WATERMARK_RSTMASK                                              0xffffffff
#define CE1_SRC_WATERMARK_RESET                                                0x0

// 0x50 (CE1_DST_WATERMARK)
#define CE1_DST_WATERMARK_LOW_LSB                                              16
#define CE1_DST_WATERMARK_LOW_MSB                                              31
#define CE1_DST_WATERMARK_LOW_MASK                                             0xffff0000
#define CE1_DST_WATERMARK_LOW_GET(x)                                           (((x) & CE1_DST_WATERMARK_LOW_MASK) >> CE1_DST_WATERMARK_LOW_LSB)
#define CE1_DST_WATERMARK_LOW_SET(x)                                           (((0 | (x)) << CE1_DST_WATERMARK_LOW_LSB) & CE1_DST_WATERMARK_LOW_MASK)
#define CE1_DST_WATERMARK_LOW_RESET                                            0x0
#define CE1_DST_WATERMARK_HIGH_LSB                                             0
#define CE1_DST_WATERMARK_HIGH_MSB                                             15
#define CE1_DST_WATERMARK_HIGH_MASK                                            0xffff
#define CE1_DST_WATERMARK_HIGH_GET(x)                                          (((x) & CE1_DST_WATERMARK_HIGH_MASK) >> CE1_DST_WATERMARK_HIGH_LSB)
#define CE1_DST_WATERMARK_HIGH_SET(x)                                          (((0 | (x)) << CE1_DST_WATERMARK_HIGH_LSB) & CE1_DST_WATERMARK_HIGH_MASK)
#define CE1_DST_WATERMARK_HIGH_RESET                                           0x0
#define CE1_DST_WATERMARK_ADDRESS                                              (0x50 + __CE1_CE_REG_CSR_BASE_ADDRESS)
#define CE1_DST_WATERMARK_RSTMASK                                              0xffffffff
#define CE1_DST_WATERMARK_RESET                                                0x0

// 0x54 (CE1_CE_DEBUG)
#define CE1_CE_DEBUG_SEL_LSB                                                   0
#define CE1_CE_DEBUG_SEL_MSB                                                   5
#define CE1_CE_DEBUG_SEL_MASK                                                  0x3f
#define CE1_CE_DEBUG_SEL_GET(x)                                                (((x) & CE1_CE_DEBUG_SEL_MASK) >> CE1_CE_DEBUG_SEL_LSB)
#define CE1_CE_DEBUG_SEL_SET(x)                                                (((0 | (x)) << CE1_CE_DEBUG_SEL_LSB) & CE1_CE_DEBUG_SEL_MASK)
#define CE1_CE_DEBUG_SEL_RESET                                                 0x0
#define CE1_CE_DEBUG_ADDRESS                                                   (0x54 + __CE1_CE_REG_CSR_BASE_ADDRESS)
#define CE1_CE_DEBUG_RSTMASK                                                   0x3f
#define CE1_CE_DEBUG_RESET                                                     0x0

// 0x58 (CE1_CE_WATCHDOG)
#define CE1_CE_WATCHDOG_STATUS_LSB                                             16
#define CE1_CE_WATCHDOG_STATUS_MSB                                             31
#define CE1_CE_WATCHDOG_STATUS_MASK                                            0xffff0000
#define CE1_CE_WATCHDOG_STATUS_GET(x)                                          (((x) & CE1_CE_WATCHDOG_STATUS_MASK) >> CE1_CE_WATCHDOG_STATUS_LSB)
#define CE1_CE_WATCHDOG_STATUS_SET(x)                                          (((0 | (x)) << CE1_CE_WATCHDOG_STATUS_LSB) & CE1_CE_WATCHDOG_STATUS_MASK)
#define CE1_CE_WATCHDOG_STATUS_RESET                                           0x0
#define CE1_CE_WATCHDOG_LIMIT_LSB                                              0
#define CE1_CE_WATCHDOG_LIMIT_MSB                                              15
#define CE1_CE_WATCHDOG_LIMIT_MASK                                             0xffff
#define CE1_CE_WATCHDOG_LIMIT_GET(x)                                           (((x) & CE1_CE_WATCHDOG_LIMIT_MASK) >> CE1_CE_WATCHDOG_LIMIT_LSB)
#define CE1_CE_WATCHDOG_LIMIT_SET(x)                                           (((0 | (x)) << CE1_CE_WATCHDOG_LIMIT_LSB) & CE1_CE_WATCHDOG_LIMIT_MASK)
#define CE1_CE_WATCHDOG_LIMIT_RESET                                            0xffff
#define CE1_CE_WATCHDOG_ADDRESS                                                (0x58 + __CE1_CE_REG_CSR_BASE_ADDRESS)
#define CE1_CE_WATCHDOG_RSTMASK                                                0xffffffff
#define CE1_CE_WATCHDOG_RESET                                                  0xffff



#endif /* _CE1_CE_REG_CSR_H_ */
