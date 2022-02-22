/*
 * TCP_Secure_Data_Transfer_Interface.h
 *
 * Contains public functions to be used as API from the application module
 *
 *  Created on: 21 feb 2022
 *      Author: Toshiba
 */

#ifndef TCP_SECURE_DATA_TRANSFER_TCP_SECURE_DATA_TRANSFER_INTERFACE_H_
#define TCP_SECURE_DATA_TRANSFER_TCP_SECURE_DATA_TRANSFER_INTERFACE_H_

uint8_t AES_CRC32_Establish_TCP_Communication_Channel(void);

void AES_CRC32_End_TCP_Communication_Channel(void);

size_t AES_CRC32_secure_message_send(char *in_message, size_t in_message_len, char *secure_message);

u16_t AES_CRC32_secure_message_receive(char *rcv_message);

#endif /* TCP_SECURE_DATA_TRANSFER_TCP_SECURE_DATA_TRANSFER_INTERFACE_H_ */
