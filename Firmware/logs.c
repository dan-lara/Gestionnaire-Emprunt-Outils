#include "logs.h"

UART_Status_t send_log(const LogType* log) {
    UART_Status_t status;

    status = USART2_SendChar('<');
    if (status != UART_OK) return status;
    // Envoyer ID de la machine
    status = USART2_SendHexString(log->id_machine);
    if (status != UART_OK) return status;

    // Envoyer séparateur ","
    status = USART2_SendChar(',');
    if (status != UART_OK) return status;

    // Envoyer opération
    status = USART2_SendChar((char)log->operation);
    if (status != UART_OK) return status;

    // Envoyer séparateur ","
    status = USART2_SendChar(',');
    if (status != UART_OK) return status;

    // Vérifier le type d'opération
    switch (log->operation) {
        case OPERATION_EMPRUNT:
        case OPERATION_RETRAIT:
            // Envoyer ID_Badge (8 octets)
            for (uint8_t i = 0; i < 8; i++) {
                status = USART2_SendHexString(log->id_badge[i]); // Envoyer chaque octet
                if (status != UART_OK) return status;
            }

            // Envoyer séparateurs et IDs restants
            status = USART2_SendChar(',');
            if (status != UART_OK) return status;

            status = USART2_SendHexString(log->id_caisse);
            if (status != UART_OK) return status;

            status = USART2_SendChar(',');
            if (status != UART_OK) return status;

            status = USART2_SendHexString(log->id_tool);
            if (status != UART_OK) return status;

            break;

        case OPERATION_ANOMALIE:
            // Envoyer type d'anomalie
            status = USART2_SendChar(log->type_anomalie);
            if (status != UART_OK) return status;
            break;

        case OPERATION_ASSISTANCE:
            break;

        default:
            return UART_ERROR; // Opération inconnue
    }

    // Envoyer séparateur ","
    status = USART2_SendChar(',');
    if (status != UART_OK) return status;

    // Envoyer timestamp
    status = USART2_SendString(log->timestamp);
    if (status != UART_OK) return status;

    status = USART2_SendChar('>');
    if (status != UART_OK) return status;

    // Terminer le log avec nouvelle ligne
    status = USART2_SendChar('\n');
    return status;
}

UART_Status_t send_log3(const LogType* log) {
    UART_Status_t status;

    status = USART3_SendChar('<');
    if (status != UART_OK) return status;
    // Envoyer ID de la machine
    status = USART3_SendHexString(log->id_machine);
    if (status != UART_OK) return status;

    // Envoyer séparateur ","
    status = USART3_SendChar(',');
    if (status != UART_OK) return status;

    // Envoyer opération
    status = USART3_SendChar((char)log->operation);
    if (status != UART_OK) return status;

    // Envoyer séparateur ","
    status = USART3_SendChar(',');
    if (status != UART_OK) return status;

    // Vérifier le type d'opération
    switch (log->operation) {
        case OPERATION_EMPRUNT:
        case OPERATION_RETRAIT:
            // Envoyer ID_Badge (8 octets)
            for (uint8_t i = 0; i < 8; i++) {
                status = USART3_SendHexString(log->id_badge[i]); // Envoyer chaque octet
                if (status != UART_OK) return status;
            }

            // Envoyer séparateurs et IDs restants
            status = USART3_SendChar(',');
            if (status != UART_OK) return status;

            status = USART3_SendHexString(log->id_caisse);
            if (status != UART_OK) return status;

            status = USART3_SendChar(',');
            if (status != UART_OK) return status;

            status = USART3_SendHexString(log->id_tool);
            if (status != UART_OK) return status;

            break;

        case OPERATION_ANOMALIE:
            // Envoyer type d'anomalie
            status = USART3_SendChar(log->type_anomalie);
            if (status != UART_OK) return status;
            break;

        case OPERATION_ASSISTANCE:
            break;

        default:
            return UART_ERROR; // Opération inconnue
    }

    // Envoyer séparateur ","
    status = USART3_SendChar(',');
    if (status != UART_OK) return status;

    // Envoyer timestamp
    status = USART3_SendString(log->timestamp);
    if (status != UART_OK) return status;

    status = USART3_SendChar('>');
    if (status != UART_OK) return status;

    // Terminer le log avec nouvelle ligne
    status = USART3_SendChar('\n');
    return status;
}

UART_Status_t request_badge_validation(uint8_t* id_badge, uint8_t id_machine) {
    UART_Status_t status;

    // Envoyer commande de validation
    status = USART2_SendString("<VALIDATE_BADGE,");
    if (status != UART_OK) return status;

    // Envoyer ID de la machine
    status = USART2_SendHexString(id_machine);
    if (status != UART_OK) return status;

    // Envoyer séparateur ","
    status = USART2_SendChar(',');
    if (status != UART_OK) return status;

    // Envoyer ID_Badge (8 octets)
    for (uint8_t i = 0; i < 8; i++) {
        status = USART2_SendHexString(id_badge[i]);
        if (status != UART_OK) return status;
    }
    
    status = USART2_SendChar('>');
    if (status != UART_OK) return status;

    // Terminer avec nouvelle ligne
    status = USART2_SendChar('\n');
    return status;
}
