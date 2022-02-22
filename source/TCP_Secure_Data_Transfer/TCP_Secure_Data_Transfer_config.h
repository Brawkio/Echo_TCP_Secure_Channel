/*
 * TCP_Secure_Data_Transfer_config.h
 *
 *	Header file containing configuration parameters that can modify
 *	TCP_Secure_Data_Transfer module, like changing source and
 *	destination IP addresses.
 *
 *  Created on: 21 feb 2022
 *      Author: Toshiba
 */

#ifndef TCP_SECURE_DATA_TRANSFER_TCP_SECURE_DATA_TRANSFER_CONFIG_H_
#define TCP_SECURE_DATA_TRANSFER_TCP_SECURE_DATA_TRANSFER_CONFIG_H_


/* IP address configuration (IP address to be assigned to the K64F). */
#define configIP_ADDR0 192
#define configIP_ADDR1 168
#define configIP_ADDR2 0
#define configIP_ADDR3 3

/* IP server address (IP address to connect with). */
#define server_ADDR0 192
#define server_ADDR1 168
#define server_ADDR2 0
#define server_ADDR3 102

/* IP server PORT (Port to connect with). */
#define server_PORT 10000

/* Netmask configuration. */
#define configNET_MASK0 255
#define configNET_MASK1 255
#define configNET_MASK2 255
#define configNET_MASK3 0

/* Gateway address configuration. */
#define configGW_ADDR0 192
#define configGW_ADDR1 168
#define configGW_ADDR2 0
#define configGW_ADDR3 100

/* MAC address configuration. */
#define configMAC_ADDR                     \
    {                                      \
        0x02, 0x12, 0x13, 0x10, 0x15, 0x11 \
    }

/* Address of PHY interface. */
#define EXAMPLE_PHY_ADDRESS BOARD_ENET0_PHY_ADDRESS

/* MDIO operations. */
#define EXAMPLE_MDIO_OPS enet_ops

/* PHY operations. */
#define EXAMPLE_PHY_OPS phyksz8081_ops

/* ENET clock frequency. */
#define EXAMPLE_CLOCK_FREQ CLOCK_GetFreq(kCLOCK_CoreSysClk)

#ifndef EXAMPLE_NETIF_INIT_FN
/*! @brief Network interface initialization function. */
#define EXAMPLE_NETIF_INIT_FN ethernetif0_init
#endif /* EXAMPLE_NETIF_INIT_FN */



/*******************************************************************************/


#endif /* TCP_SECURE_DATA_TRANSFER_TCP_SECURE_DATA_TRANSFER_CONFIG_H_ */
