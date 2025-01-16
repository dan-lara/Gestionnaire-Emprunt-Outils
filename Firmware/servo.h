#ifndef SERVO_H
#define SERVO_H

#include "stm32f10x.h"
#include "general.h"
#include "timer.h"

#define TIM_CLK 72000000
typedef struct
{
    uint8_t        SERVO_GPIO;      // GPIO utilisé pour le servomoteur
    uint8_t        SERVO_PIN;       // Broche GPIO
    TIM_TypeDef*   TIM_Instance;    // Instance du timer utilisé pour PWM
    uint32_t       PWM_TIM_CH;      // Canal PWM
    float          MinPulse;        // Valeur min de l'impulsion (en ms)
    float          MaxPulse;        // Valeur max de l'impulsion (en ms)
} SERVO_t;


void SERVO_Init(SERVO_t servo); 
void SERVO_MoveTo(SERVO_t servo, float angle);
void SERVO_RawMove(SERVO_t servo, uint16_t pulse);

// void SERVO_Init(SERVO_t servo){
//     // Enable the clock for the GPIO
//     init_gpio(servo.SERVO_GPIO, servo.SERVO_PIN, GPIO_OUT_PP_50MHZ);
//     // Enable the clock for the timer
//     uint32_t PSC_Value = 0;
//     uint32_t ARR_Value = 0;
    
//     TIM_PWM_Init(servo.TIM_Instance, servo.PWM_TIM_CH, 50, 0);
    
//     PSC_Value = (uint32_t) (TIM_CLK / 3276800.0);
// 	ARR_Value = (uint32_t) ((TIM_CLK / (50.0*(PSC_Value+1.0)))-1.0);
// }

// void SERVO_MoveTo(SERVO_t servo, float angle)
// {   
//     uint16_t Period_min = (uint16_t) (ARR_Value * (servo.MinPulse/20.0));
//     uint16_t Period_min = (uint16_t) (ARR_Value * (servo.MinPulse/20.0));
// 	uint16_t pwm = 0;
// 	pwm = ((angle*(servo.Period_Max - servo.Period_Min))/180.0) + servo.Period_Min;
// 	*(servo.TIM_CCRx) = pwm;
// }

// /* Moves A Specific Motor With A Raw Pulse Width Value */
// void SERVO_RawMove(SERVO_t servo, uint16_t pulse)
// {
// 	if(pulse <= servo.Period_Max && pulse >= servo.Period_Min)
// 		*(servo.TIM_CCRx) = pulse;
// }

#endif // SERVO_H