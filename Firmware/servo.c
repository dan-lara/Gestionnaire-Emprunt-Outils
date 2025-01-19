#include "servo.h"
#define TIM_CLK 72000000

void SERVO_Init(SERVO_t servo) 
{
    // Inicializar o GPIO para controlar o servo
    init_gpio(servo.SERVO_GPIO, servo.SERVO_PIN, GPIO_OUT_PP_50MHZ);

    // Configurar o timer para gerar PWM de 50 Hz
    uint32_t PSC_Value = (uint32_t)((TIM_CLK / 1000000) - 1);  // Divisor para clock de 1 MHz
    uint32_t ARR_Value = 20000 - 1;  // ARR para 20 ms (50 Hz)

    // Configurar prescaler e ARR
    servo.TIM_Instance->PSC = PSC_Value;  // Prescaler para 1 MHz
    servo.TIM_Instance->ARR = ARR_Value;  // ARR para 20 ms


    // Configurar o PWM para o canal apropriado
    TIM_PWM_Init(servo.TIM_Instance, servo.PWM_TIM_CH, TIM_CLK, 50);

    // Configurar posição inicial do servo (1,5 ms = centro)
    SERVO_RawMove(servo, 1500);  // Pulso de 1,5 ms
}

void SERVO_MoveTo(SERVO_t servo, float angle) 
{
    // Garantir que o ângulo esteja dentro do intervalo válido
    if (angle < -45.0f) angle = -45.0f;
    if (angle > 45.0f) angle = 45.0f;

    // Converter o ângulo para largura de pulso
    float pulse_width_ms = (angle / 90.0f) * (servo.MaxPulse - servo.MinPulse) + 1.5f;

    // Converter largura de pulso em ticks do timer
    uint16_t pulse_value = (uint16_t)((pulse_width_ms * 1000) / (20000.0 / (servo.TIM_Instance->ARR + 1)));

    // Ajustar o PWM para o canal correto
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
            return;  // Canal inválido
    }
}

void SERVO_RawMove(SERVO_t servo, uint16_t pulse)
{
    // Verificar se o pulso está dentro do intervalo permitido
    if (pulse >= 800 && pulse <= 2200) {
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
                return;  // Canal inválido
        }
    }
}