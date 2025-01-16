#include "servo.h"

void SERVO_Init(SERVO_t servo)
{
    // Initialisation du GPIO pour contrôler le servomoteur
    init_gpio(servo.SERVO_GPIO, servo.SERVO_PIN, GPIO_OUT_PP_50MHZ);

    // Initialisation du PWM sur le timer choisi
    TIM_PWM_Init(servo.TIM_Instance, servo.PWM_TIM_CH, 72000000, 50);  // 50Hz pour un servomoteur standard

    // Calcul des paramètres du timer pour la fréquence de 50Hz
    uint32_t PSC_Value = (uint32_t)(TIM_CLK / 3276800.0);  // Diviseur pour obtenir 50Hz
    uint32_t ARR_Value = (uint32_t)((TIM_CLK / (50.0 * (PSC_Value + 1.0))) - 1.0);  // Calcul de ARR pour 50Hz

    // Configuration du timer
    servo.TIM_Instance->PSC = PSC_Value;  // Paramètre de prescaler
    servo.TIM_Instance->ARR = ARR_Value;  // Paramètre de la valeur d'auto-reload
}


void SERVO_MoveTo(SERVO_t servo, float angle)
{
    // Calculer la largeur de l'impulsion en fonction de l'angle
    float pulse_width_ms = (angle / 180.0f) * (servo.MaxPulse - servo.MinPulse) + servo.MinPulse;
    
    // Convertir la largeur d'impulsion en valeur de la période pour PWM
    uint16_t pulse_value = (uint16_t)((pulse_width_ms / 20.0) * servo.TIM_Instance->ARR);
    
    // Appliquer la largeur d'impulsion au timer
    switch (servo.PWM_TIM_CH) {
        case 1:
            servo.TIM_Instance->CCR1 = pulse_value;
            break;
        case 2:
            servo.TIM_Instance->CCR2 = pulse_value;
            break;
        case 3:
            servo.TIM_Instance->CCR3 = pulse_value;
            break;
        case 4:
            servo.TIM_Instance->CCR4 = pulse_value;
            break;
        default:
            return;  // Canal invalide
    }
}


void SERVO_RawMove(SERVO_t servo, uint16_t pulse)
{
    // Vérification que la valeur d'impulsion est dans les limites valides
    if (pulse >= (uint16_t)(servo.MinPulse * 1000) && pulse <= (uint16_t)(servo.MaxPulse * 1000)) {
        switch (servo.PWM_TIM_CH) {
            case 1:
                servo.TIM_Instance->CCR1 = pulse;
                break;
            case 2:
                servo.TIM_Instance->CCR2 = pulse;
                break;
            case 3:
                servo.TIM_Instance->CCR3 = pulse;
                break;
            case 4:
                servo.TIM_Instance->CCR4 = pulse;
                break;
            default:
                return;  // Canal invalide
        }
    }
}