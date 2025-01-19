#include "LOG_Manager.h"

/**
 * @brief Constructeur de la classe LOGManager
 */
LOGManager::LOGManager(uint8_t pin, uint8_t type, unsigned long interval)
  : pin(pin), type(type),{}

/**
 * @brief Initialise le capteur LOG
 */
void LOGManager::begin() {
  
}

/**
 * @brief Lit et met à jour les données du capteur si l'intervalle de temps est dépassé
 * 
 * @return true si les nouvelles données sont différentes des précédentes, false sinon
 */
bool LOGManager::update() {
    return true;
}

String readLineFromSerial(HardwareSerial& serial) {
  String line = "";
  while (serial.available()) {
    char c = serial.read();
    if (c == '\\n' || c == '\\r') break;
    line += c;
  }
  return line;
}

void processLog(const String& log) {
  // Example: Parse a command like "VALIDER 0x12345678"
  if (log.startsWith("VALIDER ")) {
    String badgeId = log.substring(8); // Extract badge ID
    bool valid = checkBadge(badgeId);

    // Respond back to the machine
    Serial2.println(valid ? "OUI" : "NON");
    return;
  }

  // Parse standard log messages
  StaticJsonDocument<JSON_SIZE> jsonDoc;
  deserializeJson(jsonDoc, log);

  String operation = jsonDoc["operation"];
  String data = jsonDoc["data"];

  // Validate and send to API
  bool success = sendLogToAPI(operation, data);
  if (success) {
    Serial.println("Log sent successfully");
  } else {
    Serial.println("Failed to send log");
  }
}
