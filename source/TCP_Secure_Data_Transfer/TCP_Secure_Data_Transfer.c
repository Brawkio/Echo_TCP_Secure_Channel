/*
 * TCP_Secure_Data_Transfer.c
 *
 * This file was created to create separate TCP/IP layer from the tcp_echo application
 * and add secure send and receive message functions that implements encryption
 * through AES and Checksum through CRC32.
 *
 * Future work: The created socket for TCP is declared as global for the functions to
 * process it. This is not a proper approach.
 * A timeout functionality could be added in order to don't block the microcontroller
 * when it is waiting to establish a connection.
 *
 * TCP/IP init function could take an IP as parameter(s) for it to be able to connect
 * with different servers, or it could ask for IP in runtime.
 *
 * This file could potentially be reused using a different protocol, as the idea of
 * encrypt and CRC32 is still valid.
 *
 *
 *
 *  Created on: 14 feb 2022 ;)
 *      Author: Braulio Peraza <braulio.peraza@iteso.mx>ITESO's Embedded systems specialty student
 */
#include "TCP_Secure_Data_Transfer.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/

static mdio_handle_t mdioHandle = {.ops = &EXAMPLE_MDIO_OPS};
static phy_handle_t phyHandle   = {.phyAddr = EXAMPLE_PHY_ADDRESS, .mdioHandle = &mdioHandle, .ops = &EXAMPLE_PHY_OPS};

	//TCP global socket variables, they need to be global to be
	//shared between secure send and receive functions
	  struct netconn *conn;
	  err_t err;
	  ip_addr_t srvr_address;
	  struct netbuf *buf;

	  //NETIF Variables
	    static struct netif netif;
	    ip4_addr_t netif_ipaddr, netif_netmask, netif_gw;
	    ethernetif_config_t enet_config = {
	        .phyHandle  = &phyHandle,
	        .macAddress = configMAC_ADDR,
	    };



/**
* Wrapper function that initializes global tcp and socket variables
* Microcontroller's IP and server IP are defined and
*
* @param socket is the netconn structure to be created
* @param destination_address is the ip_addr_t type the created socket will be connecting to
*
*/
void tcp_socket_init(struct netconn *socket)
{

	    mdioHandle.resource.csrClock_Hz = EXAMPLE_CLOCK_FREQ;

	    IP4_ADDR(&netif_ipaddr, configIP_ADDR0, configIP_ADDR1, configIP_ADDR2, configIP_ADDR3);
	    IP4_ADDR(&netif_netmask, configNET_MASK0, configNET_MASK1, configNET_MASK2, configNET_MASK3);
	    IP4_ADDR(&netif_gw, configGW_ADDR0, configGW_ADDR1, configGW_ADDR2, configGW_ADDR3);

	    PRINTF("\r\nInitializing TCP/IP\r\n");
	    tcpip_init(NULL, NULL);

	    netifapi_netif_add(&netif, &netif_ipaddr, &netif_netmask, &netif_gw, &enet_config, EXAMPLE_NETIF_INIT_FN,
	                       tcpip_input);
	    netifapi_netif_set_default(&netif);
	    netifapi_netif_set_up(&netif);

	    PRINTF("\r\n************************************************\r\n");
	    PRINTF(" K64 TCP/IP configuration:\r\n");
	    PRINTF("************************************************\r\n");
	    PRINTF(" IPv4 Address     : %u.%u.%u.%u\r\n", ((u8_t *)&netif_ipaddr)[0], ((u8_t *)&netif_ipaddr)[1],
	           ((u8_t *)&netif_ipaddr)[2], ((u8_t *)&netif_ipaddr)[3]);
	    PRINTF(" IPv4 Subnet mask : %u.%u.%u.%u\r\n", ((u8_t *)&netif_netmask)[0], ((u8_t *)&netif_netmask)[1],
	           ((u8_t *)&netif_netmask)[2], ((u8_t *)&netif_netmask)[3]);
	    PRINTF(" IPv4 Gateway     : %u.%u.%u.%u\r\n", ((u8_t *)&netif_gw)[0], ((u8_t *)&netif_gw)[1],
	           ((u8_t *)&netif_gw)[2], ((u8_t *)&netif_gw)[3]);
	    PRINTF("************************************************\r\n");

	    IP4_ADDR(&srvr_address, server_ADDR0, server_ADDR1, server_ADDR2, server_ADDR3);
		/* Create a new connection identifier. */
		conn = netconn_new(NETCONN_TCP);

		LWIP_ERROR("tcp: invalid connection", (conn != NULL), return;);
		//socket created

}

/**
* Function that uses AES encryption library to encrypt a data array and pads it
* in order for the message to be a multiple of 16.
* It is worth mentioning that the function uses strlen, meaning that if there is
* the message will be read until it fins a null char.
* It uses a predefined key an initialization vector. It is recommended to change
* them for them to be defined in runtime
*
* @param data_in is a pointer to the array of chars containing the message
* @param data_out is a pointer to an array of chars where the encrypted data will be stored
* @param data_out_length is the variable where the length of the array will be stored
*
*/
void aes_encrypt(char *data_in, char *data_out, size_t *data_out_length)
{
	/* AES data */
	uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
	uint8_t iv[]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	struct AES_ctx ctx;
	size_t in_data_len, alt_in_data_len;
	//size_t data_out_length;

	PRINTF("\nAES 128, Encryption in progress...\r\n\n");
	/* Init the AES context structure */
	AES_init_ctx_iv(&ctx, key, iv);

	/* To encrypt an array its length must be a multiple of 16 so we concatenate zeros */
	in_data_len = strlen(data_in);
	//PRINTF("in_data_len = %d \r\n",in_data_len);

	if (in_data_len%16 != 0)
	{
	*data_out_length = in_data_len + (16 - (in_data_len%16) );
	}
	else
	{
		*data_out_length = in_data_len;
	}
	//PRINTF("data_out_length = %d \r\n",*data_out_length);
	memcpy(data_out, data_in, in_data_len);

	AES_CBC_encrypt_buffer(&ctx, data_out, *data_out_length);
	PRINTF("\r\n");
}

/**
* Function that uses CRC-32's K64 module to initialize a CRC configuration.
* It acts as a pseudo-wraper, as it contains the CRC_Init function
* @param base is a pointer to a CRC_Type structure that will contain the configuration
* @param seed is the value of the "seed" to be used in the crc-32 algorithm
*
*/
static void crc32_config(CRC_Type *base, uint32_t seed)
{
	/* CRC data */
    crc_config_t config_crc;
    config_crc.polynomial         = 0x04C11DB7U;
    config_crc.seed               = seed;
    config_crc.reflectIn          = true;
    config_crc.reflectOut         = true;
    config_crc.complementChecksum = true;
    config_crc.crcBits            = kCrcBits32;
    config_crc.crcResult          = kCrcFinalChecksum;

    CRC_Init(base, &config_crc);

}

/**
* Function that computes the CRC32 of a char data array, it then appends it or
* concatenates it to the end of the message in a little endian way.
*
* @param data_in is a pointer to the array of chars containing the message
* @param data_in_length is the variable that contains the length of data_in
* @param data_out is a pointer to an array of chars where the message + CRC32 will be stored
* @param data_out_length is the variable where the length of the array will be stored (data+4bytes)
*
*/
void append_crc32(char *data_in, size_t data_in_length, char *data_out, size_t *data_out_length)
{
	CRC_Type *base = CRC0;
	uint32_t checksum32;


	crc32_config(base,0xFFFFFFFFU);
	PRINTF("\nCalculating CRC-32...\r\n\n");

	//PRINTF("crc32 data_in_length = %d \r\n",data_in_length);
    CRC_WriteData(base, (uint8_t *)&data_in[0], data_in_length);
    checksum32 = CRC_Get32bitResult(base);

    PRINTF("CRC-32 is: 0x%08x or %d\r\n", checksum32, checksum32);

    PRINTF("\nAdding Checksum to the array...\r\n");
    //checksum must be converted into an array
    //we can do so shifting values and applying a mask
    uint32_t mask=0x000000FF;
    uint32_t checksum_array[4];
    //put in little endian order
    checksum_array[3]=(checksum32>>24) & mask;
    checksum_array[2]=(checksum32>>16) & mask;
    checksum_array[1]=(checksum32>>8) & mask;
    checksum_array[0]=checksum32 & mask;

    //increment array's length for it to be able to hold the checksum
    *data_out_length=data_in_length+4;

    //transfer encoded message to the new array which has +4 extra bytes for checksum
    for(int i=0;i<data_in_length;i++)
    {
    	data_out[i]=data_in[i];
    }

	/*PRINTF("\nOriginal Encrypted Message: \n");
	for(int i=0; i<data_in_length; i++) {
		PRINTF("0x%02x,", data_out[i]);
	}*/

    //Now, the checksum of the encrypted message is appended to the encrypted message (concatenate)
    for(int i=0; i<4; i++) {
    	data_out[data_in_length+i]=checksum_array[i];
	}
    PRINTF("\r\n");

	/*PRINTF("Encrypted Message with Checksum: \n");
	for(int i=0; i<*data_out_length; i++) {
		PRINTF("0x%02x,", data_out[i]);
	}

	PRINTF("\r\n");*/
}

/**
* Function that decrypts a message containing AES encryption with a key and
* an initialization vector.
* The output decrypted message is contained in the same array of the original one
*
* @param encrypted_message is a pointer to the array of chars containing the encrypted message
* @param message_length is the variable that contains the length of data_in
*
*/
void aes_decrypt(char *encrypted_message, size_t message_length)
{
	/* AES data */
	uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
	uint8_t iv[]  = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	struct AES_ctx ctx;
	/*PRINTF("\nEncrypted message:\n");
	PRINTF("%s",encrypted_message);*/
	PRINTF("\nDecrypting message...\r\n");
	/* Init the AES context structure */
	AES_init_ctx_iv(&ctx, key, iv);

	AES_CBC_decrypt_buffer(&ctx, encrypted_message, message_length);

	/*PRINTF("Decrypted Message in hexadecimal: ");
	for(int i=0; i<message_length; i++) {
		PRINTF("0x%02x,", encrypted_message[i]);
	}*/
}

/**
* Function that takes the crc32 appended to a char array and compares it to the computed
* checksum of the rest of the message.
*
* @param data_in is a pointer to the array of chars containing the message with crc32
* @param data_in_length is the variable that contains the length of data_in
*
* @return Returns 0 if crc is invalid, returns 1 if checksum is correct
*/
uint8_t check_crc32(char *data_in, size_t data_in_length)
{
	CRC_Type *base = CRC0;
	uint32_t in_checksum32=0, calc_checksum32;
	size_t data_out_length;
	char rcv_data[512] = {0};

	uint8_t return_value=0;

	/*Check if data length is valid (must be a 16 multiple without the checksum)*/
	//PRINTF("\nReceived message data_in_length = %d \r\n",data_in_length);
	if((data_in_length-4)%16 != 0)
	{
		PRINTF("\n\n\tInvalid encrypted data length\n\n");
		return 0;//return error
	}

	crc32_config(base,0xFFFFFFFFU);
	PRINTF("\nValidating received message's CRC32\r\n\n");

	/*PRINTF("Received Encrypted Message: ");
	for(int i=0; i<data_in_length; i++) {
		PRINTF("0x%02x,", data_in[i]);
	}*/

	//create a local copy of the data
	memcpy(rcv_data, data_in, data_in_length);

	/*PRINTF("Local copy of Received Encrypted Message: ");
	for(int i=0; i<data_in_length; i++) {
		PRINTF("0x%02x,", rcv_data[i]);
	}*/

	PRINTF("\n\tRetrieving appended message's CRC32...\r\n");
	//separate the last 4 bytes
	in_checksum32 |= rcv_data[data_in_length-4];
	//PRINTF("rcv_data[data_in_length-4] 0x%02x,", rcv_data[data_in_length-4]);
	//PRINTF("in_CRC-32: 0x%08x\r\n", in_checksum32);
	in_checksum32 |= rcv_data[data_in_length-3] << 8;
	//PRINTF("rcv_data[data_in_length-3] 0x%02x,", rcv_data[data_in_length-3]);
	//PRINTF("in_CRC-32: 0x%08x\r\n", in_checksum32);
	in_checksum32 |= rcv_data[data_in_length-2] << 16;
	//PRINTF("rcv_data[data_in_length-2] 0x%02x,", rcv_data[data_in_length-2]);
	//PRINTF("in_CRC-32: 0x%08x\r\n", in_checksum32);
	in_checksum32 |= rcv_data[data_in_length-1] << 24;
	//PRINTF("rcv_data[data_in_length-1] 0x%02x,", rcv_data[data_in_length-1]);
	PRINTF("Checksum appended to the message is: 0x%08x or %d\r", in_checksum32, in_checksum32);

	//Now, the checksum to the incoming data is performed

	PRINTF("\n\tComputing message's CRC32...\r\n");
	 CRC_WriteData(base, (uint8_t *)&data_in[0], (data_in_length-4));
	 calc_checksum32 = CRC_Get32bitResult(base);
	 PRINTF("Calculated Checksum is: 0x%08x or %d\r\n", calc_checksum32, calc_checksum32);

	 //now, compare the calculated checksum with the extracted checksum
	 if(calc_checksum32 == in_checksum32)
	 {
		 PRINTF("\n\t  Checksum validated!\n");
		 return_value=1;
	 }
	 else
	 {
		 PRINTF("\n\n\t Checksum not validated!");
		 return_value=0;
	 }
}

uint8_t AES_CRC32_Establish_TCP_Communication_Channel(void)
{
	//creates a socket with the server address
	tcp_socket_init(conn);

    PRINTF("\r\n************************************************\r\n");
    PRINTF("Trying to connect with Server Address %u.%u.%u.%u in Port %u .....\r\n", ((u8_t *)&srvr_address)[0], ((u8_t *)&srvr_address)[1],
           ((u8_t *)&srvr_address)[2], ((u8_t *)&srvr_address)[3], server_PORT);
    err= netconn_connect(conn, &srvr_address, server_PORT);
    //add functionality of time out (pending)
	if(err == 0)
	{
	PRINTF("\nServer accepted new connection! %p\n", conn);
	return 0;
	}
	else
	{
		PRINTF("Cannot connect with the server %p\n", conn);
		return 3;
	}
}


/**
* Wrapper function that closes and deletes a TCP socket session using netconn functions.
*
*/
void AES_CRC32_End_TCP_Communication_Channel(void)
{
	/* Close connection and discard connection identifier. */
	netconn_close(conn);
	netconn_delete(conn);

}

/**
* Function that takes a message (array of chars) encrypts it with AESs 128,
* appends a CRC32 to the end of the array and sends it through a socket connection
*
* @param in_message is a pointer to the array of chars containing the message to be sent
* @param in_message_len is the variable that contains the length of data_in
* @param secure_message is a pointer to the array of chars containing the sent message
*
* @return Returns the length of the sent message
*/
size_t AES_CRC32_secure_message_send(char *in_message, size_t in_message_len, char *secure_message)
{

	//Message variables
	char encrypted_msg[512]= {0};
	size_t encrypted_msg_len, secure_msg_len;
	char local_msg[512]= {0};

	//if (err == ERR_OK)
	//{
		memcpy(local_msg, in_message, in_message_len);

		aes_encrypt(local_msg, encrypted_msg, &encrypted_msg_len);
		PRINTF("Encrypted Message: \n");
		//printf("%s\n",encrypted_msg);
		  for(int i=0; i<encrypted_msg_len; i++)
		  {
			  PRINTF("%c", encrypted_msg[i]);
		  }
		  printf("\n");

		append_crc32(encrypted_msg, encrypted_msg_len, secure_message, &secure_msg_len);
		//PRINTF("secure_msg_len = %d \r\n", secure_msg_len);
		//PRINTF("Secure message to be sent: \n");
		//PRINTF("%s",secure_message);

		err=netconn_write(conn, secure_message, secure_msg_len, NETCONN_COPY);//send message
		return secure_msg_len;
	//}
	//else
	//{
		/* Close connection and discard connection identifier. */
		//netconn_close(conn);
		//netconn_delete(conn);
	//}
}


/**
* Function that receives a message from a socket connection, validates the crc-32, decrypts it
* using AES 128 and prints it to the console
*
* @param in_message is a pointer to the array of chars containing the message to be sent
* @param in_message_len is the variable that contains the length of data_in
* @param secure_message is a pointer to the array of chars containing the sent message
*
* @return Returns the length of the sent message
*/

u16_t AES_CRC32_secure_message_receive(char *received_message)
{

	u16_t rcv_message_len;

	 /* if (err == ERR_OK)
	  {*/
	  err=netconn_recv(conn, &buf);//receive message and store it in a buffer

	  PRINTF("\n\n\t::::: -> Data received from socket <- :::::\n");

	  netbuf_data(buf, (void**) &received_message, &rcv_message_len);//rcvd_data now points toward the buf containing the echoed data
		PRINTF("\nReceived message:\n");
		//PRINTF("%s \n",received_message);
		  for(int i=0; i<rcv_message_len; i++)
		  {
			  PRINTF("%c", received_message[i]);
		  }
		  PRINTF("\n");

	  if(check_crc32(received_message, rcv_message_len)==1)
	  {//if checksum is validated, decryption follows

		  aes_decrypt(received_message, rcv_message_len-4);

		  //Finally, print the decrypted data
		  PRINTF("\n\nDecrypted Message: \n");
		  //printf("%s",received_message);
		  for(int i=0; i<rcv_message_len-4; i++)
		  {
			  PRINTF("%c", received_message[i]);
		  }
		  printf("\n");
		  netbuf_delete(buf);
		  return (rcv_message_len-4);
	  }
	  else
	  {
		  netbuf_delete(buf);
		  PRINTF("Received message's data is CORRUPTED! D:\n");
	  }

	  }
	  //else
	  //{
	     // netconn_close(conn);
	      //netconn_delete(conn);
	  //}

//}
