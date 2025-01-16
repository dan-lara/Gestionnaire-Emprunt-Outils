#include "stm32f10x.h" // Inclure la biblioth�que STM32f10x
#include "general.h" // Inclure la biblioth�que g�n�rale
#include "timer.h"
#include "uart.h"
#include "servo.h"
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

SERVO_t servo = {
    .SERVO_GPIO = 'a',
    .SERVO_PIN = 6,
    .TIM_Instance = TIM3,
    .PWM_TIM_CH = 1,
    .MinPulse = 1.0f,  // Impulsion min (en ms)
    .MaxPulse = 2.0f   // Impulsion max (en ms)
};

int main(void) {
		// int r = USART2_init(9600);
		// init_gpio('a', 0, GPIO_INPUT_FLOATING);
		// init_gpio('a', 1, GPIO_INPUT_FLOATING);
    init_gpio('a', 6, GPIO_AF_PP_50MHZ);
		
		// TIM_INTERR_Init(TIM2, 1000000000); //init_timer_interruptions(TIM2, 1000000000);
		// TIM_PWM_Init(TIM3, 1, 125000, 50); //init_timer_pwm(TIM3, 1, 125000, 50);

    SERVO_Init(servo);
    while (1) {
        
        SERVO_MoveTo(servo, 20); // Déplacer le servomoteur à 0 degré
        delay_ms(10000); // Attendre 10 secondes
        SERVO_MoveTo(servo, 50);
        // Boucle principale - peut �tre utilis�e pour modifier le rapport cyclique
    }
}
