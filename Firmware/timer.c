#include "timer.h"

void TIM_PWM_Init(TIM_TypeDef *TIMx, uint8_t channel, uint32_t frequency, uint16_t duty_cycle) {
    // Activation de l'horloge du timer
    if (TIMx == TIM1) {
        RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    } else if (TIMx == TIM2) {
        RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    } else if (TIMx == TIM3) {
        RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    } else if (TIMx == TIM4) {
        RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    }

    uint32_t timer_clock = FREQ_72K ? 72000000 : 36000000; // Fréquence du timer (72 MHz) ou (36 MHz)
    uint32_t arr_value = (timer_clock / frequency) - 1; // Calcul de ARR pour la fréquence

    TIMx->PSC = 0;                 // Pas de division (prescaler = 0)
    TIMx->ARR = arr_value;         // Charger ARR pour obtenir la fréquence souhaitée

    // Configuration du canal sélectionné
    switch (channel) {
        case 1:
            TIMx->CCR1 = (duty_cycle * (TIMx->ARR + 1)) / 100; // Configurer le rapport cyclique
            TIMx->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; // Mode PWM 1
            TIMx->CCMR1 |= TIM_CCMR1_OC1PE;     // Activer la précharge
            TIMx->CCER |= (1<<0);        // Activer le canal 1         
            break;
        case 2:
            TIMx->CCR2 = (duty_cycle * (TIMx->ARR + 1)) / 100; // Configurer le rapport cyclique
            TIMx->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2; // Mode PWM 1
            TIMx->CCMR1 |= TIM_CCMR1_OC2PE;     // Activer la précharge
            TIMx->CCER |= TIM_CCER_CC2E;        // Activer le canal 2
            break;
        case 3:
            TIMx->CCR3 = (duty_cycle * (TIMx->ARR + 1)) / 100; // Configurer le rapport cyclique
            TIMx->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2; // Mode PWM 1
            TIMx->CCMR2 |= TIM_CCMR2_OC3PE;     // Activer la précharge
            TIMx->CCER |= TIM_CCER_CC3E;        // Activer le canal 3
            break;
        case 4:
            TIMx->CCR4 = (duty_cycle * (TIMx->ARR + 1)) / 100; // Configurer le rapport cyclique
            TIMx->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2; // Mode PWM 1
            TIMx->CCMR2 |= TIM_CCMR2_OC4PE;     // Activer la précharge
            TIMx->CCER |= TIM_CCER_CC4E;        // Activer le canal 4
            break;
        default:
            return; // Canal invalide
    }

    TIMx->CR1 |= TIM_CR1_ARPE;          // Activer l'auto-reload
    TIMx->CR1 |= TIM_CR1_CEN;           // Activer le compteur
    TIMx->BDTR |= (1 << 15);            // Activer la sortie principale
}
void TIM_INTERR_Init(TIM_TypeDef *TIMx, uint32_t frequency) {
    // Activation de l'horloge du timer
    if (TIMx == TIM2) {
        RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    } else if (TIMx == TIM3) {
        RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    } else if (TIMx == TIM4) {
        RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    } else {
        return; // Timer invalide
    }

    uint32_t timer_clock = 72000000; // Fréquence d'horloge du timer (72 MHz)
    uint32_t arr_value = (timer_clock / frequency) - 1; // Calcul de ARR pour la fréquence

    TIMx->PSC = 0;            // Pas de prescaler (division = 1)
    TIMx->ARR = arr_value;    // Charger ARR pour définir la fréquence
    TIMx->DIER |= TIM_DIER_UIE; // Activer les interruptions pour les mises à jour

    if (TIMx == TIM2) {
        NVIC_EnableIRQ(TIM2_IRQn); // Activer l'interruption TIM2 dans le NVIC
    } else if (TIMx == TIM3) {
        NVIC_EnableIRQ(TIM3_IRQn); // Activer l'interruption TIM3 dans le NVIC
    } else if (TIMx == TIM4) {
        NVIC_EnableIRQ(TIM4_IRQn); // Activer l'interruption TIM4 dans le NVIC
    }

    TIMx->CR1 |= TIM_CR1_CEN; // Activer le compteur
}

void TIM_PWM_SetDuty(TIM_TypeDef *TIMx, uint8_t channel, uint16_t duty_cycle) {
    uint32_t new_ccr = (duty_cycle * (TIMx->ARR + 1)) / 100;

    switch (channel) {
        case 1: TIMx->CCR1 = new_ccr; break;
        case 2: TIMx->CCR2 = new_ccr; break;
        case 3: TIMx->CCR3 = new_ccr; break;
        case 4: TIMx->CCR4 = new_ccr; break;
        default: return; // Canal invalide
    }
}