#include "stm32f10x.h" // Inclure la bibliothèque STM32f10x
#include "general.h" // Inclure la bibliothèque générale
#include "timer.h"
#include "uart.h"
#include "servo.h"
#include "logs.h"
#include "manchester.h"

//==================Définitions==================
//Définitions pour le système
#define SYSTICK_FREQ 1000 // Fréquence du systick en Hz
#define MACHINE_ID 0x01 // ID de la machine
#define CAISSE_ID_DROITE 0x01 // ID de la caisse
#define CAISSE_ID_GAUCHE 0x02 // ID de la caisse
#define TOOL_ID_DROITE 0x10 // ID de l'outil
#define TOOL_ID_GAUCHE 0x21 // ID de l'outil

// Définitions pour l'ADXL345
#define ADXL345_POWER_CTL   0x2D  // Adresse du registre pour le contrôle de l'alimentation
#define ADXL345_DATA_FORMAT 0x31  // Adresse du registre pour le format des données
#define ADXL345_DEVID       0x00  // Adresse du registre pour l'ID de l'appareil
#define ADXL345_CS_PIN      4
#define ADXL345_DATAX0      0x32  // Adresse du registre pour les données de l'axe X
#define DEBUG 0
#define ID_UNIQUE 0x2A
#define CODE_SESSION 0x0F
#define CODE_CAPTEUR 0x01

// Définitions pour le servomoteur
#define SERVO_MIN_PULSE 800    // Largeur d'impulsion pour 0 degrés (en microsecondes)
#define SERVO_MAX_PULSE 2200   // Largeur d'impulsion pour 180 degrés (en microsecondes)
#define SERVO_PWM_PERIOD 20000 // Période PWM en microsecondes (50 Hz)
#define DEBOUNCE_TIME_MS 50

//==================Variables globales==================

volatile uint16_t servo_angle = 1500; // Position neutre (90°)
volatile uint8_t bouton_flags[16] = {0}; // Drapeaux pour identifier quel bouton a été pressé
volatile uint32_t last_interrupt_time[16] = {0};
const uint8_t badge_id[8] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0}; // 123456789ABCDEF0
// Flags pour les boutons
volatile uint8_t choisir = 0;
volatile uint8_t libere = 0;

// Structure pour le servomoteur
SERVO_t servo = {
  .SERVO_GPIO = 'a',
  .SERVO_PIN = 8,
  .TIM_Instance = TIM1,
  .PWM_TIM_CH = 1,
  .MinPulse = 0.8f,  // Impulsion min (en ms)
  .MaxPulse = 2.2f   // Impulsion max (en ms)
};

// Structure pour le log
LogType log_data = {
  .id_machine = 0x00,
  .operation = OPERATION_EMPRUNT,
  .id_badge = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0},
  .id_caisse = 0x00,
  .id_tool = 0x00,
  .type_anomalie = ANOMALIE_NULLE,
  .timestamp = ""
};

//==================Prototypes==================
//Temps
uint32_t get_current_time_ms(void);
void set_log_timestamp(LogType *log, uint32_t current_time);

// Fonctions pour le servomoteur
void init_Servo_TIM1(void);
void set_servo_angle(uint16_t angle);

// Fonctions pour les interruptions
void configure_bouton_interrupts(void);
void configure_interrupt_priorities(void);
void USART2_IRQHandler(void);
void EXTI15_10_IRQHandler(void);
void EXTI9_5_IRQHandler(void);

// Fonctions pour les boutons
void init_boutons(void);
uint8_t read_bouton(GPIO_TypeDef *GPIOx, uint8_t pin_number);


#define UART_BUFFER_SIZE 128

volatile char uart2_rx_buffer[UART_BUFFER_SIZE];
volatile uint16_t uart2_rx_index = 0;

void process_uart_command(const char *command);

void USART2_IRQHandler(void) {
  if (USART2->SR & USART_SR_TXE) { // Transmission prête
    // Code pour l'envoi de données
    USART2->DR = 'A'; // Exemple : envoie un caractère 'A'
  }
  if (USART2->SR & USART_SR_RXNE) { // Réception prête
    char received = USART2->DR; // Lire les données reçues
    USART2_SendChar(received); // Envoyer le caractère reçu
    /*if (received == '\n' || received == '\r') { // Fin de ligne
      uart2_rx_buffer[uart2_rx_index] = '\0'; // Terminer la chaîne
      process_uart_command((const char *)uart2_rx_buffer); // Traiter la commande
      uart2_rx_index = 0; // Réinitialiser l'index
    } else {
      if (uart2_rx_index < UART_BUFFER_SIZE - 1) {
        uart2_rx_buffer[uart2_rx_index++] = received; // Ajouter le caractère au buffer
      }
    }*/
  }
}

void process_uart_command(const char *command) {
  // Traiter la commande reçue
  // Exemple : afficher la commande reçue
  printf("Commande reçue : %s\n", command);
}

//==================Main==================
int main(void) {
  /*
  // Initialisation de la Decodage Manchester
  //On n'a pas tester cette partie car notre carte ne fonctionne pas.

  SystemInit(); // Initialisation du système
  // Initialisation de l'USART
  USART1_Init();
  // Initialisation du décodeur Manchester
  ResetManchesterDecoder();
  // Démarrage du décodage
  StartManchesterDecoding();
  */

 /*
  // Initialisation de l'ADXL345 pour détecter les secousses
  //Le module est totalement fonctionnel, mais il n'est pas utilisé
   dans ce programme, une fois que on a pas encore tous les autres modules.

  SPI2_Init(SPI_CLK_DIV_16,1,1,8);
  SPI_ConfigCS(GPIOB, 12);
  int16_t x, y, z;
  
 */

  /*
  // Initialisation des interruptions pour les boutons
  //J'ai commenté cette partie car les boutons génèrent
  plusiers interruptions même avec un debounce configuré
  dans le interrupt handler.

  //configure_bouton_interrupts();
  //configure_interrupt_priorities();

  */

  USART2_init(9600); // Initialisation de l'UART2 à 9600 bauds pour l'ordinateur
  USART3_init(9600); // Initialisation de l'UART3 à 9600 bauds pour l'esp32
  
  init_Servo_TIM1();
  
  init_gpio('a', 7, GPIO_OUT_PP_2MHZ); // Configurer LED (PA7) pour la sortie push-pull 2 MHz

  init_boutons();
  
  log_data.id_machine = MACHINE_ID; // ID de la machine
  set_log_timestamp(&log_data, get_current_time_ms()); // Définir le timestamp du log

  while(1){
    uint32_t current_time = get_current_time_ms();
    //ADXL345_ReadAcceleration(SPI2, GPIOB, 12, &x, &y, &z); // Lire les données de l'accéléromètre

    // Selectionner l'outil GAUCHE
    if (read_bouton(GPIOC,10)==0 && choisir){
      set_servo_angle(800);
      choisir = 0;
      libere = 1;
      last_interrupt_time[4] = current_time;
      log_data.id_caisse = CAISSE_ID_GAUCHE;
      log_data.id_tool = TOOL_ID_GAUCHE;
      delay_ms(DEBOUNCE_TIME_MS);
    }
    // Selectionner l'outil DROITE
    if (read_bouton(GPIOC,12)==0 && choisir){
      while(read_bouton(GPIOC,12)==0);
      set_servo_angle(2200);
      choisir = 0;
      libere = 1;
      last_interrupt_time[4] = current_time;
      log_data.id_caisse = CAISSE_ID_DROITE;
      log_data.id_tool = TOOL_ID_DROITE;
      delay_ms(DEBOUNCE_TIME_MS);
    }
    //Bouton pour Retrait
    if (read_bouton(GPIOC,8)==0){
      while(read_bouton(GPIOC,8)==0);
      bouton_flags[8] = 1;
      delay_ms(DEBOUNCE_TIME_MS);
    }
    //Bouton pour Assistance
    if (read_bouton(GPIOC,6)==0){
      while(read_bouton(GPIOC,6)==0);
      bouton_flags[6] = 1;
      delay_ms(DEBOUNCE_TIME_MS);
    }
    //Bouton pour Emprunt
    if (read_bouton(GPIOC,5)==0){
      while(read_bouton(GPIOC,5)==0);
      bouton_flags[5] = 1;
      delay_ms(DEBOUNCE_TIME_MS);
    }
    //Bouton pour Libérer l'outil (Detecter Presence), Par defaut Presse pour Emprunter et libere pour Retirer
    if (read_bouton(GPIOC,4) && libere){
      while(read_bouton(GPIOC,4));
      bouton_flags[4] = 1;
      GPIOA->ODR &= ~(1 << 7);
      delay_ms(DEBOUNCE_TIME_MS);
    }
    if (bouton_flags[8]) {
      bouton_flags[8] = 0;
      choisir = request_badge_validation(badge_id, MACHINE_ID);
      choisir = 1; // Pour tester
      log_data.operation = OPERATION_RETRAIT;
      GPIOA->ODR |= (1 << 7); // Activer la LED
    }
    if (bouton_flags[6]) {
      bouton_flags[6] = 0;
      log_data.operation = OPERATION_ASSISTANCE;
      send_log(&log_data);
    }
    if (bouton_flags[5]) {
      bouton_flags[5] = 0;
      GPIOA->ODR |= (1 << 7);
      choisir = request_badge_validation(badge_id, MACHINE_ID);
      choisir = 1;
      log_data.operation = OPERATION_EMPRUNT;
      //send_log(&log_data);
    }
    if (bouton_flags[4] ) {
      bouton_flags[4] = 0;
      GPIOA->ODR &= ~(1 << 7);
      libere = 0;
      set_servo_angle(1500);
      set_log_timestamp(&log_data, get_current_time_ms());
      send_log(&log_data);
    }
  }
}
//==================Fonctions==================

//Temps
uint32_t get_current_time_ms(void) {
  return SysTick->VAL / (SystemCoreClock / 1000);
}

//==================Boutons====================
void init_boutons(void) {
  // Activer l'horloge pour les GPIO
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

  // Configurer PC10, PC12, PC8, PC6, PC5 pour les boutons
  init_gpio('c', 10, GPIO_INPUT_PULL);
  init_gpio('c', 12, GPIO_INPUT_PULL);
  init_gpio('c', 8, GPIO_INPUT_PULL);
  init_gpio('c', 6, GPIO_INPUT_PULL);
  init_gpio('c', 5, GPIO_INPUT_PULL);
  GPIOC->ODR |= (1 << 10) | (1 << 12) | (1 << 8) | (1 << 6) | (1 << 5);
}
uint8_t read_bouton(GPIO_TypeDef *GPIOx, uint8_t pin_number) {
  // Retourne 1 si le bouton n'est pas pressé (état pull-up)
  // Retourne 0 si le bouton est pressé (niveau bas)
  return (GPIOx->IDR & (1 << pin_number)) ? 1 : 0;
}

//==================Temporisation==================
void set_log_timestamp(LogType *log, uint32_t current_time) {
  // Fonction pour définir le timestamp du log
  // Exemple : "16012025123045" (JJMMAAAAHHMMSS)
  char timestamp_str[15];
  sprintf(timestamp_str, "%010lu", current_time);
  log_data.timestamp = timestamp_str;
}


//==================Servomoteur==================
void init_Servo_TIM1(void) {
  // Activer l'horloge pour TIM1
  RCC->APB2ENR |= (1 << 11);

  TIM1->PSC = 71;       // Prescaler (72 MHz / (71 + 1) = 1 MHz)
  TIM1->ARR = 20000;    // Auto-reload (20 ms -> 50 Hz)
  TIM1->CCMR1 |= (0x6 << 4) | (1 << 3); // Mode PWM1 + preload
  TIM1->CCER |= (1 << 0); // Activer TIM1_CH1
  TIM1->CCR1 = servo_angle; // Position initiale
  TIM1->CR1 |= (1 << 0);    // Activer le compteur
  TIM1->BDTR |= (1 << 15);  // Activer la sortie principale

  init_gpio('a', 8, GPIO_AF_PP_50MHZ); // Configurer la broche PA8 en mode alternatif 50 MHz
}

// Fonction pour définir l'angle du servo
void set_servo_angle(uint16_t angle) {
  if (angle < SERVO_MIN_PULSE) angle = SERVO_MIN_PULSE;
  if (angle > SERVO_MAX_PULSE) angle = SERVO_MAX_PULSE;
  TIM1->CCR1 = angle;
}

//==================Interruptions==================
void EXTI15_10_IRQHandler(void) {
  uint32_t current_time = get_current_time_ms(); // Temps actuel
  // Vérifier quelle ligne a généré l'interruption et nettoyer le drapeau
  if (EXTI->PR & (1 << 10)) { // PC10
    EXTI->PR |= (1 << 10);  // Nettoyer le drapeau
    bouton_flags[10] = 1; // Signaler que le bouton a été pressé
    set_servo_angle(800);
  }
  if (EXTI->PR & (1 << 12)) { // PC5
    EXTI->PR |= (1 << 12); // Nettoyer le drapeau de l'interruption
    bouton_flags[12] = 1; // Signaler que le bouton a été pressé
    set_servo_angle(2200);
  }
}

void EXTI9_5_IRQHandler(void) {
  uint32_t current_time = get_current_time_ms(); // Temps actuel
  if (EXTI->PR & (1 << 8)) { // PC8
    EXTI->PR |= (1 << 8);  // Nettoyer le drapeau
    GPIOA->ODR |= (1 << 7); // Activer la LED
    bouton_flags[8] = 1; // Signaler que le bouton a été pressé
    choisir = 1;
  }
  if (EXTI->PR & (1 << 6)) { // PC6
    EXTI->PR |= (1 << 6);  // Nettoyer le drapeau
    bouton_flags[6] = 1; // Signaler que le bouton a été pressé
  }
  if (EXTI->PR & (1 << 5)) { // PC5
    EXTI->PR |= (1 << 5); // Nettoyer le drapeau de l'interruption
    GPIOA->ODR |= (1 << 7); // Activer la LED
    bouton_flags[5] = 1;
    choisir = 1;
  }
}

void configure_bouton_interrupts(void) {
  // Activer l'horloge de l'AFIO
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

  // Configurer PC10, PC12, PC8, PC6, PC5 pour les lignes d'interruption
  AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI10_PC; // Ligne 10
  AFIO->EXTICR[3] |= AFIO_EXTICR4_EXTI12_PC; // Ligne 12
  AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI8_PC; // Ligne 8
  AFIO->EXTICR[1] |= AFIO_EXTICR2_EXTI6_PC; // Ligne 6
  AFIO->EXTICR[1] |= AFIO_EXTICR2_EXTI5_PC; // Ligne 5

  // Configurer EXTI pour bord de descente
  EXTI->FTSR |= (1 << 10) | (1 << 12) | (1 << 8) | (1 << 6) | (1 << 5);

  // Désactiver bord de montée
  EXTI->RTSR &= ~((1 << 10) | (1 << 12) | (1 << 8) | (1 << 6) | (1 << 5));

  // Activer les interruptions pour les lignes correspondantes
  EXTI->IMR |= (1 << 10) | (1 << 12) | (1 << 8) | (1 << 6) | (1 << 5);

  // Activer les IRQs dans le NVIC
  NVIC_EnableIRQ(EXTI15_10_IRQn); // Gère les lignes 10-15
  NVIC_EnableIRQ(EXTI9_5_IRQn); // Gère les lignes 5-9
}

void configure_interrupt_priorities(void) {
  NVIC_SetPriority(USART2_IRQn, 1);     // UART avec priorité haute
  NVIC_SetPriority(USART3_IRQn, 1);     // UART avec priorité haute
  //NVIC_SetPriority(EXTI15_10_IRQn, 3); // Boutons (lignes 10-15) avec priorité moindre
  //NVIC_SetPriority(EXTI9_5_IRQn, 3); // Boutons (lignes 10-15) avec priorité moindre
}
