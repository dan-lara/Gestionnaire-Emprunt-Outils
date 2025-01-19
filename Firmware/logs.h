#ifndef LOGS_H
#define LOGS_H

#include "uart.h"

typedef enum {
    OPERATION_EMPRUNT = 'E',  // Emprunt
    OPERATION_RETRAIT = 'R',  // Retrait
    OPERATION_ANOMALIE = 'A', // Anomalie
    OPERATION_ASSISTANCE = 'D' // Demande d'assistance
} OperationType;

typedef enum {
    ANOMALIE_NULLE = 'N',              // Anomalie nulle
    ANOMALIE_BADGE_INVALIDE = 'B',    // Badge invalide
    ANOMALIE_MACHINE_HORS_SERVICE = 'H', // Machine hors service
    ANOMALIE_OUTIL_NON_DISPONIBLE ='O', // Outil non disponible
    ANOMALIE_ERREUR_INCONNUE = 'I',    // Erreur inconnue
    ANOMALIE_SHAKE = 'S',              // Secousse
} AnomalieType;

typedef struct {
    uint8_t id_machine;     // ID de la machine
    OperationType operation; // Type d'opération
    uint8_t id_badge[8];     // ID du badge (8 octets)
    uint8_t id_caisse;       // ID de la caisse
    uint8_t id_tool;         // ID de l'outil
    AnomalieType type_anomalie;   // Type d'anomalie (seulement pour OPERATION_ANOMALIE)
    const char* timestamp;   // Date et heure au format JJMMYYYYHHMMSS
} LogType;

UART_Status_t send_log(const LogType* log);
UART_Status_t send_log3(const LogType* log);
UART_Status_t request_badge_validation(uint8_t* id_badge, uint8_t id_machine);
/*void example_usage() {
    LogType log = {
        .id_machine = 0x01,
        .operation = OPERATION_EMPRUNT,
        .id_badge = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0},
        .id_caisse = 0x01,
        .id_tool = 0x10,
        .type_anomalie = ANOMALIE_NULLE, // Ignoré pour les opérations autres que ANOMALIE
        .timestamp = "16012025123045"
    };

    // Envoyer log
    UART_Status_t log_status = send_log(&log);
    if (log_status != UART_OK) {
        // Traiter l'erreur
    }

    // Demander validation du badge
    UART_Status_t validation_status = request_badge_validation(log.id_badge, log.id_machine);
    if (validation_status != UART_OK) {
        // Traiter l'erreur
    }
}*/

#endif