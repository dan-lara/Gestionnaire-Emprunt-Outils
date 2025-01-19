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

/* Fonctions -----------------------------------------------------------------*/
void USART1_Init(void) {
    // Active les horloges
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN;

    // Configure les broches en mode altern�e push-pull
    GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9 | GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
    GPIOA->CRH |= GPIO_CRH_MODE9_1 | GPIO_CRH_CNF9_1;  // PA9 = TX
    GPIOA->CRH |= GPIO_CRH_CNF10_1;                     // PA10 = RX

    // Configure l'USART1
    USART1->BRR = 0x0683;  // Pour 72MHz/2400 baud
    USART1->CR1 = 0;  // Reset CR1
    USART1->CR1 |= USART_CR1_UE | USART_CR1_RE | USART_CR1_RXNEIE | USART_CR1_TE;

    NVIC_EnableIRQ(USART1_IRQn);
}

void USART1_IRQHandler(void) {
    uint16_t status;
    uint8_t received_byte;
    
    status = USART1->SR;
    if (status & USART_SR_RXNE) {
        received_byte = (uint8_t)USART1->DR;
        
        if (decode_active) {
            DecodeManchester(received_byte);
        }
    }
}

void StartManchesterDecoding(void) {
    ResetManchesterDecoder();
    decode_active = 1;
}

static void ResetManchesterDecoder(void) {
    memset(&decodeData, 0, sizeof(EM4102_Data));
    decode_active = 0;
}

static void DecodeManchester(uint8_t received_bit) {
    static uint8_t previous_bit = 0;
    static uint8_t transition_expected = 1;
    
    received_bit &= 0x01;

    if (transition_expected) {
        if (received_bit == previous_bit) {
            ResetManchesterDecoder();
            return;
        }
        
        decodeData.bitStream = (uint16_t)((decodeData.bitStream << 1) | received_bit);

        if (decodeData.bitStream == EM4102_SYNC_FIELD) {
            decodeData.byteIdx = 0;
            decodeData.bitIdx = 0;
            decodeData.active = 1;
        } 
        else if (decodeData.active) {
            SetDataBit(&decodeData, received_bit);
            
            decodeData.bitIdx++;
            if (decodeData.bitIdx == EM4102_BITS_PER_BYTE) {
                decodeData.bitIdx = 0;
                decodeData.byteIdx++;
                
                if (decodeData.byteIdx == EM4102_DATA_BYTES_NUM) {
                    if (ValidateChecksum(&decodeData)) {
                        EM4102_DataReadyCallback();
                    }
                    ResetManchesterDecoder();
                }
            }
        }
    }

    previous_bit = received_bit;
    transition_expected = !transition_expected;
}

static void SetDataBit(EM4102_Data* data, uint8_t bit) {
    if (bit) {
        data->data[data->byteIdx] |= (1 << data->bitIdx);
    }
    data->parityCheck ^= bit;
}

static uint8_t ValidateChecksum(const EM4102_Data* data) {
    uint8_t checksum = 0;
    uint8_t i;
    
    for (i = 0; i < EM4102_DATA_BYTES_NUM - 1; i++) {
        checksum ^= data->data[i];
    }
    
    return (checksum == data->data[EM4102_DATA_BYTES_NUM - 1]);
}

void EM4102_DataReadyCallback(void) {
    uint8_t i;

    if (!ValidateChecksum(&decodeData)) {
        return;
    }

    for (i = 0; i < EM4102_DATA_BYTES_NUM; i++) {
        while (!(USART1->SR & USART_SR_TXE)) {}
        USART1->DR = decodeData.data[i];
    }
    
    while (!(USART1->SR & USART_SR_TC)) {}
}