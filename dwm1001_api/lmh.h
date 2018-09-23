/* 
 *  Jared Butcher <jared.butcher1219@gmail.com>
 * 9/21/2018
 *
 * Ported from Decawave DWM1001 API as found at https://www.decawave.com/product/dwm1001-module/
 * for use with Arduinos
 */
#ifndef _LMH_ARDUINO_H_
#define _LMH_ARDUINO_H_

#include "Arduino.h"
#include <stdint.h>
#include "dwm1001_tlv.h"
#include "dwm_api.h"

/* Defines what is being used to communicate with DWM1001
 * INTERFACE_MODE = 
 * 0 - SPI
 * 1 - SPI with interupt    -Not implemented
 * 2 - UART                 -Not implemented
 * 3 - I2C                  -Not implemented
*/
#define LMH_INTERFACE_MODE 0

#if LMH_INTERFACE_MODE == 2
#elif LMH_INTERFACE_MODE == 3
    #include<Wire.h>
#else
    #include <SPI.h>
    #define LMH_SLAVE_SELECT 13
    #define LMH_SPI_MAX_LENGTH 255
    #define LMH_SPI_MAX_SPEED 8000000
    #define LMH_SPI_MODE SPI_MODE0
    #define LMH_SPI_DATA_ORDER LSBFIRST //TODO could be wrong
#endif

#define LMH_DEBUG_LOG 1

#define LMH_HEADER_LENGTH 1 //TODO could be wrong
#define LMH_HEADER_SIZE_OFFSET 0 //TODO could be wrong
#define LMH_TIMEOUT   1000
#define LMH_OK    0
#define LMH_ERR   1


/** 
 * @brief initializes the LMH utilities over defined interface
 *
 * @param none
 *
 * @return none
 */
void LMH_Init(void);

/** 
 * @brief transmit data over defined interface
 *
 * @param [in] data: pointer to the Tx data buffer
 * @param [in] length: length of data to be received
 *
 * @return Error code
 */
int  LMH_Tx(uint8_t* data, uint8_t* length);

/** 
 * @brief wait for response data over defined interface
 *       note: this function is blocking 
 *
 * @param [out] data: pointer to the RX data buffer
 * @param [out] length: length of data to be received
 * @param [in] exp_length: expected length of response data. 
 *             Note - If the user doesn't know how long the response from DWM1001 to the host 
 *                   is, then this parameter should be set to DWM1001_TLV_MAX_SIZE as defined
 *                   in dwm1001.h. In this case,
 *                   for SPI, length check won't report error no matter how long the received
 *                   data is;
 *                   for UART, this function will not return until the timeout period expires. 
 *
 * @return Error code
 */
int  LMH_WaitForRx(uint8_t* data, uint16_t* length, uint16_t exp_length);

/** 
 * @brief wait for response data over defined interface
 *       note: this function is blocking 
 *
 * @param [in] ret_val: pointer to the response data buffer, where the first three bytes must 
 *       be TLV values 0x40, 0x01, 0x00 meaning a RV_OK, to indicating that the request is 
 *       properly parsed. Otherwise the previous communication between the host and DWM1001 
 *       was not acting correctly.
 *
 * @return Error code
 */
int LMH_CheckRetVal(uint8_t* ret_val);


#endif //_LMH_ARDUINO_H_