// Copyright (c) 2013 Qualcomm Atheros, Inc.  All rights reserved.
// $ATH_LICENSE_HW_HDR_C$
//
// DO NOT EDIT!  This file is automatically generated
//               These definitions are tied to a particular hardware layout


#ifndef _TX_MPDU_START_USERX_H_
#define _TX_MPDU_START_USERX_H_
#if !defined(__ASSEMBLER__)
#endif

// ################ START SUMMARY #################
//
//	Dword	Fields
//	0	full_mpdu_length[13:0], reserved_0a[15:14], mpdu_header_length[23:16], reserved_0b[31:24]
//
// ################ END SUMMARY #################

#define NUM_OF_DWORDS_TX_MPDU_START_USERX 1

struct tx_mpdu_start_userx {
    volatile uint32_t full_mpdu_length                : 14, //[13:0]
                      reserved_0a                     :  2, //[15:14]
                      mpdu_header_length              :  8, //[23:16]
                      reserved_0b                     :  8; //[31:24]
};

/*

full_mpdu_length
			
			Filled in by TX_DMA 
			
			(based on length field in the MPDU link descriptor)

reserved_0a
			
			Set to 0 by the generator, ignored by consumer.  <legal
			0>

mpdu_header_length
			
			This field is filled in by the OLE
			
			Used by PCU, This prevents PCU from having to do this
			again (in the same way))

reserved_0b
			
			Set to 0 by the generator, ignored by consumer.  <legal
			0>
*/


/* Description		TX_MPDU_START_USERX_0_FULL_MPDU_LENGTH
			
			Filled in by TX_DMA 
			
			(based on length field in the MPDU link descriptor)
*/
#define TX_MPDU_START_USERX_0_FULL_MPDU_LENGTH_OFFSET                0x00000000
#define TX_MPDU_START_USERX_0_FULL_MPDU_LENGTH_LSB                   0
#define TX_MPDU_START_USERX_0_FULL_MPDU_LENGTH_MASK                  0x00003fff

/* Description		TX_MPDU_START_USERX_0_RESERVED_0A
			
			Set to 0 by the generator, ignored by consumer.  <legal
			0>
*/
#define TX_MPDU_START_USERX_0_RESERVED_0A_OFFSET                     0x00000000
#define TX_MPDU_START_USERX_0_RESERVED_0A_LSB                        14
#define TX_MPDU_START_USERX_0_RESERVED_0A_MASK                       0x0000c000

/* Description		TX_MPDU_START_USERX_0_MPDU_HEADER_LENGTH
			
			This field is filled in by the OLE
			
			Used by PCU, This prevents PCU from having to do this
			again (in the same way))
*/
#define TX_MPDU_START_USERX_0_MPDU_HEADER_LENGTH_OFFSET              0x00000000
#define TX_MPDU_START_USERX_0_MPDU_HEADER_LENGTH_LSB                 16
#define TX_MPDU_START_USERX_0_MPDU_HEADER_LENGTH_MASK                0x00ff0000

/* Description		TX_MPDU_START_USERX_0_RESERVED_0B
			
			Set to 0 by the generator, ignored by consumer.  <legal
			0>
*/
#define TX_MPDU_START_USERX_0_RESERVED_0B_OFFSET                     0x00000000
#define TX_MPDU_START_USERX_0_RESERVED_0B_LSB                        24
#define TX_MPDU_START_USERX_0_RESERVED_0B_MASK                       0xff000000


#endif // _TX_MPDU_START_USERX_H_
