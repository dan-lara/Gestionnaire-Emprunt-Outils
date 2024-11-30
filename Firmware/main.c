#include "stm32f10x.h" // Inclure la biblioth�que STM32f10x
#include "general.h" // Inclure la biblioth�que g�n�rale
#include "timer.h"
//==================Définitions==================
#define SOME_THRESHOLD_LOW 8000    // Exemple : temps bas en µs pour téta
#define SOME_THRESHOLD_HIGH 16000  // Exemple : temps élevé en µs pour 2teta

#define MAX_BITS 64
static uint8_t badge_data[MAX_BITS]; 

volatile uint32_t rising_time = 0;   // Temps du dernier front montant
volatile uint32_t falling_time = 0; // Temps du dernier front descendant
volatile uint32_t interval = 0;     // Intervalle mesuré
volatile uint8_t data_ready = 0;    // Indique si une donnée est prête à être décodée

volatile uint32_t time_ch1 = 0;
volatile uint32_t time_ch2 = 0;
volatile uint8_t state = 0;

void process_signal(uint8_t bit_value);
uint8_t is_valid_badge(uint8_t *badge_data);
void confirm_reading(uint8_t *badge_data);
/*
// Configuration de TIM2 pour g�n�rer un signal PWM avec une fr�quence sp�cifi�e
void TIM2_PWM_Config(uint32_t frequency, uint16_t duty_cycle) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Activer l'horloge pour TIM2

    uint32_t timer_clock = 72000000; // Fr�quence du timer (72 MHz)
    uint32_t arr_value = (timer_clock / frequency) - 1; // Calcul de ARR pour la fr�quence

    TIM2->PSC = 0;                 // Pas de division (prescaler = 0)
    TIM2->ARR = arr_value;         // Charger ARR pour obtenir la fr�quence souhait�e
    TIM2->CCR1 = (duty_cycle * (TIM2->ARR + 1)) / 100; // Configurer le rapport cyclique

    TIM2->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; // Mode PWM 1
    TIM2->CCMR1 |= TIM_CCMR1_OC1PE;     // Activer la pr�charge
    TIM2->CCER |= TIM_CCER_CC1E;        // Activer le canal 1
    TIM2->CR1 |= TIM_CR1_ARPE;          // Activer l'auto-reload
    TIM2->CR1 |= TIM_CR1_CEN;           // Activer le compteur
}
void init_timer_pwm(TIM_TypeDef *TIMx, uint8_t channel, uint32_t frequency, uint16_t duty_cycle) {
    // Activation de l'horloge du timer
    if (TIMx == TIM2) {
        RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    } else if (TIMx == TIM3) {
        RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    } else if (TIMx == TIM4) {
        RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
    }

    uint32_t timer_clock = 72000000; // Fréquence du timer (72 MHz)
    uint32_t arr_value = (timer_clock / frequency) - 1; // Calcul de ARR pour la fréquence

    TIMx->PSC = 0;                 // Pas de division (prescaler = 0)
    TIMx->ARR = arr_value;         // Charger ARR pour obtenir la fréquence souhaitée

    // Configuration du canal sélectionné
    switch (channel) {
        case 1:
            TIMx->CCR1 = (duty_cycle * (TIMx->ARR + 1)) / 100; // Configurer le rapport cyclique
            TIMx->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; // Mode PWM 1
            TIMx->CCMR1 |= TIM_CCMR1_OC1PE;     // Activer la précharge
            TIMx->CCER |= TIM_CCER_CC1E;        // Activer le canal 1
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
}
void init_timer_interruptions(TIM_TypeDef *TIMx, uint32_t frequency) {
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
// Fonction pour modifier le rapport cyclique
void TIM2_SetDutyCycle(uint16_t duty_cycle) {
    if (duty_cycle <= 100)
        TIM2->CCR1 = (duty_cycle * (TIM2->ARR + 1)) / 100; // Mettre � jour CCR1
}
void timer_set_duty_cycle(TIM_TypeDef *TIMx, uint8_t channel, uint16_t duty_cycle) {
    uint32_t new_ccr = (duty_cycle * (TIMx->ARR + 1)) / 100;

    switch (channel) {
        case 1: TIMx->CCR1 = new_ccr; break;
        case 2: TIMx->CCR2 = new_ccr; break;
        case 3: TIMx->CCR3 = new_ccr; break;
        case 4: TIMx->CCR4 = new_ccr; break;
        default: return; // Canal invalide
    }
}*/
void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_CC1IF) { // Interruption CH1 (PA0, front montant)
        time_ch1 = TIM2->CCR1;    // Capturer la valeur du timer pour CH1
        TIM2->SR &= ~TIM_SR_CC1IF; // Nettoyer le flag
        state |= 0x01;             // Marquer CH1 capturé
    }

    if (TIM2->SR & TIM_SR_CC2IF) { // Interruption CH2 (PA1, front descendant)
        time_ch2 = TIM2->CCR2;    // Capturer la valeur du timer pour CH2
        TIM2->SR &= ~TIM_SR_CC2IF; // Nettoyer le flag
        state |= 0x02;             // Marquer CH2 capturé
    }

    // Décodage une fois les deux transitions capturées
    if (state == 0x03) { // Les deux interruptions ont été capturées
        uint32_t delta = (time_ch2 > time_ch1) ? (time_ch2 - time_ch1) : (time_ch1 - time_ch2);
				USART2_SendHexString((uint8_t)(time_ch1));
				USART2_SendHexString((uint8_t)(time_ch1 >> 8));
				USART2_SendHexString((uint8_t)(time_ch1 >> 16));
				USART2_SendHexString((uint8_t)(time_ch1 >> 24));
        if (delta > SOME_THRESHOLD_LOW && delta < SOME_THRESHOLD_HIGH) {
            // Temps normal (téta)
            process_signal(1);
						USART2_SendString("TETA");
        } else if (delta > 2 * SOME_THRESHOLD_LOW && delta < 2 * SOME_THRESHOLD_HIGH) {
            // Temps double (2 * téta)
            process_signal(0);
						USART2_SendString("2TETA");
        } else {
            state = 0;
						process_signal(2);
						USART2_SendString("ERRR");
        }

        state = 0; // Réinitialiser pour les prochaines captures
    }
}
void process_signal(uint8_t bit_value) {
    static uint8_t bit_index = 0;

    // Si bit_value est -1, erreur de lecture, recommencer
    if (bit_value == 2) {
        bit_index = 0;  // Réinitialiser la lecture
        return;
    }

    // Stocker la donnée lue (bit)
    badge_data[bit_index] = bit_value;
    bit_index++;

    // Vérifier si la trame est complète
    if (bit_index == MAX_BITS) {
        // Traiter le badge : valider si le badge est dans la base de données
        if (is_valid_badge(badge_data)) {
            // Envoyer confirmation de lecture réussie
            confirm_reading(badge_data);
        } else {
            // Badge non valide, recommencer
            bit_index = 0;
        }
    }
}
uint8_t is_valid_badge(uint8_t *badge_data) {
    // Rechercher le badge dans la base de données
    // Exemple : utiliser une table de recherche
    /*for (int i = 0; i < NUM_BADGES; i++) {
        if (memcmp(badge_data, badge_db[i], MAX_BITS) == 0) {
            return 1; // Badge trouvé
        }
    }
    return 0; // Badge non trouvé
		*/
		return 1;
}
void confirm_reading(uint8_t *badge_data) {
		USART2_SendString("Data: \n");
    for(int i = 0; i < MAX_BITS; i++){
			USART2_SendHexString(badge_data[i]);
		}
}


int main(void) {
		int r = USART2_init(9600);
		init_gpio('a', 0, GPIO_INPUT_FLOATING);
		init_gpio('a', 1, GPIO_INPUT_FLOATING);
    init_gpio('a', 6, GPIO_AF_PP_50MHZ);
		
		TIM_INTERR_Init(TIM2, 1000000000); //init_timer_interruptions(TIM2, 1000000000);
		TIM_PWM_Init(TIM3, 1, 125000, 50); //init_timer_pwm(TIM3, 1, 125000, 50);
    while (1) {
        // Boucle principale - peut �tre utilis�e pour modifier le rapport cyclique
    }
}
