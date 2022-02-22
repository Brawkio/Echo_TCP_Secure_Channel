/*
 * Echo.c
 *
 * This file is used as an application layer file.
 *
 * It implements an echo communication paradigm through TCP_Secure_Data_Transfer functions
 *
 * The files asks for data to be sent and through the debug console, it shows the data sent,
 * encrypted data, encrypted data + crc32, received data, crc32 validation, and the decrypted data
 *
 *  Created on: 14 feb 2022 ;)
 *      Author: Braulio Peraza <braulio.peraza@iteso.mx>ITESO's Embedded systems specialty student
 *
 */

#include "Echo.h"

/*-----------------------------------------------------------------------------------*/
static void echo_thread(void *arg)
{
	size_t msg_len;
	u16_t rcv_buf_len;
	char msg[512]={0};
	char connect = 'n', retry = 'n';

	//Dato interesante, si inserto la funcionalidad de crear una nueva conexión después de haber
	//terminado una, el error Function called without core lock aparece, lo que no permite
	//ciclar la creación de sockets
	do
	{
		PRINTF("\n\tDo you wish to establish a connection with the server? (y/n)\n");
		SCANF(" %[^\n]c", &connect);
		if(connect != 'y')
		{
			retry=0;
			PRINTF("\n\tNo connection established \n");
			break;
		}
		retry = 'y';//enables a new message sending
	}while(AES_CRC32_Establish_TCP_Communication_Channel() != 0);

	while (retry == 'y')
	{
		printf("\nType the message and press enter to send it: \n");
		SCANF(" %[^\n]s",&msg);////////VERY IMPORTANT!!!!!, a space should be added to every scanf after the first one
		msg_len=strlen(msg);
		//Before writing, Message must be encrypted
		PRINTF("\nStarting message securing with AES and CRC-32\r\n");
		msg_len = AES_CRC32_secure_message_send(msg, msg_len, msg);
		PRINTF("Secure message sent: \n");
		//PRINTF("%s",msg);
		  for(int i=0; i<msg_len; i++)
		  {
			  PRINTF("%c", msg[i]);
		  }
		  printf("\n");
		msg_len = AES_CRC32_secure_message_receive(msg);

		PRINTF("\nDo you wish to keep the session alive? (y/n)\r\n");
		SCANF(" %[^\n]c", &retry);
		if(retry != 'y')
		{
			PRINTF("\n\tEnding session.. \n");
			AES_CRC32_End_TCP_Communication_Channel();
			break;
		}
	}
	PRINTF("\n\tApplication terminated. \n");

}
/*-----------------------------------------------------------------------------------*/
void
echo_init(void)
{
  sys_thread_new("echo_thread", echo_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}
/*-----------------------------------------------------------------------------------*/

