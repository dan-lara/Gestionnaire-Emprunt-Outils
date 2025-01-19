/**
 ******************************************************************************
 * @file           : manchester.c
 * @brief          : Decodage Manchester pour badge RFID 125 kHz (EM4102)
 ******************************************************************************
 */

/* Inclusions ----------------------------------------------------------------*/
#include "stm32f10x.h"
#include "string.h"

/* Constantes ----------------------------------------------------------------*/
#define EM4102_SYNC_FIELD        0x1FF       // Champ de synchronisation (9 bits � 1)
#define EM4102_BITS_PER_BYTE     8           // Nombre de bits par octet
#define EM4102_TOTAL_BITS        64          // Nombre total de bits dans une trame
#define EM4102_DATA_BYTES_NUM    (EM4102_TOTAL_BITS / EM4102_BITS_PER_BYTE)
#define USART_BAUDRATE           2400        // D�finition explicite du baudrate

/* Structures ----------------------------------------------------------------*/
typedef struct {
    uint16_t bitStream;                      // Plac� en premier pour l'alignement
    uint8_t data[EM4102_DATA_BYTES_NUM];
    uint8_t bitIdx;
    uint8_t byteIdx;
    uint8_t active;
    uint8_t parityCheck;
} EM4102_Data;

/* Variables globales --------------------------------------------------------*/
static EM4102_Data decodeData;
static volatile uint8_t decode_active;

/* Prototypes de fonctions ---------------------------------------------------*/
void USART1_IRQHandler(void);
static void DecodeManchester(uint8_t received_bit);
static void ResetManchesterDecoder(void);
static void SetDataBit(EM4102_Data* data, uint8_t bit);
static uint8_t ValidateChecksum(const EM4102_Data* data);
void EM4102_DataReadyCallback(void);
void USART1_Init(void);
void StartManchesterDecoding(void);
void SystemInit(void);