#ifndef TIMER_H
#define TIMER_H

#include "general.h"

// Définitions pour la configuration
#define FREQ_72K 1

//Prototype de les fonctions
void TIM_PWM_Init(TIM_TypeDef *TIMx, uint8_t channel, uint32_t frequency, uint16_t duty_cycle);
void TIM_PWM_SetDuty(TIM_TypeDef *TIMx, uint8_t channel, uint16_t duty_cycle);
void TIM_INTERR_Init(TIM_TypeDef *TIMx, uint32_t frequency);

#endif // TIMER_H