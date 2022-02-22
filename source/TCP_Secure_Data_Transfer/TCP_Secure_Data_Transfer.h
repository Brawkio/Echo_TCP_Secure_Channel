/*
 * TCP_Secure_Data_Transfer.h
 *
 *	Header file containing private functions for the new layer
 *
 *  Created on: 14 feb 2022
 *      Author: Toshiba
 */

#ifndef TCP_SECURE_DATA_TRANSFER_H
#define TCP_SECURE_DATA_TRANSFER_H


#include "lwip/opt.h"


#include "lwip/sys.h"
#include "lwip/api.h"

#include "aes.h"
#include "fsl_crc.h"
#include "fsl_debug_console.h"

#include "lwip/netifapi.h"
#include "lwip/tcpip.h"
#include "netif/ethernet.h"
#include "enet_ethernetif.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_phy.h"

#include "fsl_phyksz8081.h"
#include "fsl_enet_mdio.h"
#include "fsl_device_registers.h"

#include "TCP_Secure_Data_Transfer_Interface.h"
#include "TCP_Secure_Data_Transfer_config.h"
/*******************************************************************************/

/************************TCPIP/Ethernet/Socket initialization****************************/
static void tcp_socket_init(struct netconn *connection);

/****************************Encrypt/Decrypt functions************************************/
void aes_encrypt(char *data_in, char *data_out, size_t *data_out_length);
void aes_decrypt(char *encrypted_message, size_t data_in_length);

/****************************************CRC-32 functions************************************/
static void crc32_config(CRC_Type *base, uint32_t seed);
void append_crc32(char *data_in, size_t data_in_length, char *data_out, size_t *data_out_length);
uint8_t check_crc32(char *data_in, size_t data_in_length);


#endif /* TCP_SECURE_DATA_TRANSFER_H */
