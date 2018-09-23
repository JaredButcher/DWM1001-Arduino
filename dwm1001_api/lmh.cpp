/* 
 *  Jared Butcher <jared.butcher1219@gmail.com>
 * 9/21/2018
 *
 * Ported from Decawave DWM1001 API as found at https://www.decawave.com/product/dwm1001-module/
 * for use with Arduinos
 */
#include "lmh.h"

/** 
 * @brief initializes the LMH utilities over defined interface
 *
 * @param none
 *
 * @return none
 */
void LMH_Init(void){
    #if LMH_INTERFACE_MODE == 2
    #elif LMH_INTERFACE_MODE == 3
    #else
        pinMode(LMH_SLAVE_SELECT, OUTPUT);
        SPI.begin();
        digitalWrite(LMH_SLAVE_SELECT, HIGH);
        #if LMH_DEBUG_LOG == 1
            Serial.println("LMH SPI started");
        #endif
    #endif
}

/** 
 * @brief transmit data over defined interface
 *
 * @param [in] data: pointer to the Tx data buffer
 * @param [in] length: length of data to be received
 *
 * @return Error code
 */
int  LMH_Tx(uint8_t* data, uint8_t* length){
    #if LMH_INTERFACE_MODE == 2
    #elif LMH_INTERFACE_MODE == 3
    #else
        SPI.beginTransaction(SPISettings(LMH_SPI_MAX_SPEED, LMH_SPI_DATA_ORDER, LMH_SPI_MODE));
        digitalWrite(LMH_SLAVE_SELECT, LOW);
        SPI.transfer(data, length);
        digitalWrite(LMH_SLAVE_SELECT, HIGH);
        SPI.endTransaction();
        #if LMH_DEBUG_LOG == 1
            Serial.println("LMH SPI sent:");
            Serial.write(data, *length);
            Serial.println();
        #endif
    #endif
}

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
int  LMH_WaitForRx(uint8_t* data, uint16_t* length, uint16_t exp_length){
    uint32_t time = millis();
    uint8_t header[LMH_HEADER_LENGTH], location;
    location = 0;
    memset(header, 0, LMH_HEADER_LENGTH);

    #if INTERFACE_MODE == 2
    #elif INTERFACE_MODE == 3
    #else
        SPI.beginTransaction(SPISettings(LMH_SPI_MAX_SPEED, LMH_SPI_DATA_ORDER, LMH_SPI_MODE));
        digitalWrite(LMH_SLAVE_SELECT, LOW);
        while(location < LMH_HEADER_LENGTH && millis() - time > LMH_TIMEOUT){
            header[location] = SPI.transfer(0x00);
        }
        for(uint8_t i = 0; i < header[LMH_HEADER_SIZE_OFFSET] && millis() - time > LMH_TIMEOUT; ++i){
            data[i] = SPI.transfer(0x00);
        }
        *length = header[LMH_HEADER_SIZE_OFFSET];
        digitalWrite(LMH_SLAVE_SELECT, HIGH);
        SPI.endTransaction();
        if(LMH_CheckRetVal(data) == LMH_ERR || millis() - time < LMH_TIMEOUT || exp_length != *length){
            #if LMH_DEBUG_LOG == 1
                Serial.println("LMH SPI received failed");
            #endif
            return LMH_ERR;
        }
        #if LMH_DEBUG_LOG == 1
            Serial.println("LMH SPI received header:");
            Serial.write(header, LMH_HEADER_LENGTH);
            Serial.println("Data:");
            Serial.write(data, *length);
            Serial.println();
        #endif
        return LMH_OK;

    #endif
}

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
int LMH_CheckRetVal(uint8_t* ret_val){
   if(ret_val[0] == DWM1001_TLV_TYPE_RET_VAL && ret_val[1] == 1 && ret_val[2] == RV_OK)
   {
      return LMH_OK;
   }
   return LMH_ERR;
}