#include "API_Manager.h"

// Configuration
const char* WIFI_SSID = "Magneto";
const char* WIFI_PASSWORD = "837sbsoehs8w9";
const String API_URL = "http://192.168.175.1:8000";

//PushOver COnfigs
const char* PO_apiToken = "API_TOKEN";
const char* PO_userToken = "USER_TOKEN";

//Pushover API endpoint
const char* pushoverApiEndpoint = "https://api.pushover.net/1/messages.json";

//Pushover root certificate (valid from 11/10/2006 to 15/01/2038)
const char *PUSHOVER_ROOT_CA = "-----BEGIN CERTIFICATE-----\n"
                  "MIIDjjCCAnagAwIBAgIQAzrx5qcRqaC7KGSxHQn65TANBgkqhkiG9w0BAQsFADBh\n"
                  "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
                  "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\n"
                  "MjAeFw0xMzA4MDExMjAwMDBaFw0zODAxMTUxMjAwMDBaMGExCzAJBgNVBAYTAlVT\n"
                  "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
                  "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IEcyMIIBIjANBgkqhkiG\n"
                  "9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuzfNNNx7a8myaJCtSnX/RrohCgiN9RlUyfuI\n"
                  "2/Ou8jqJkTx65qsGGmvPrC3oXgkkRLpimn7Wo6h+4FR1IAWsULecYxpsMNzaHxmx\n"
                  "1x7e/dfgy5SDN67sH0NO3Xss0r0upS/kqbitOtSZpLYl6ZtrAGCSYP9PIUkY92eQ\n"
                  "q2EGnI/yuum06ZIya7XzV+hdG82MHauVBJVJ8zUtluNJbd134/tJS7SsVQepj5Wz\n"
                  "tCO7TG1F8PapspUwtP1MVYwnSlcUfIKdzXOS0xZKBgyMUNGPHgm+F6HmIcr9g+UQ\n"
                  "vIOlCsRnKPZzFBQ9RnbDhxSJITRNrw9FDKZJobq7nMWxM4MphQIDAQABo0IwQDAP\n"
                  "BgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUTiJUIBiV\n"
                  "5uNu5g/6+rkS7QYXjzkwDQYJKoZIhvcNAQELBQADggEBAGBnKJRvDkhj6zHd6mcY\n"
                  "1Yl9PMWLSn/pvtsrF9+wX3N3KjITOYFnQoQj8kVnNeyIv/iPsGEMNKSuIEyExtv4\n"
                  "NeF22d+mQrvHRAiGfzZ0JFrabA0UWTW98kndth/Jsw1HKj2ZL7tcu7XUIOGZX1NG\n"
                  "Fdtom/DzMNU+MeKNhJ7jitralj41E6Vf8PlwUHBHQRFXGU7Aj64GxJUTFy8bJZ91\n"
                  "8rGOmaFvE7FBcf6IKshPECBV1/MUReXgRPTqh5Uykw7+U0b6LJ3/iyK5S9kJRaTe\n"
                  "pLiaWN0bfVKfjllDiIGknibVb63dDcY3fe0Dkhvld1927jyNxF1WW6LZZm6zNTfl\n"
                  "MrY=\n"
                  "-----END CERTIFICATE-----\n";
                  

APIManager api(WIFI_SSID, WIFI_PASSWORD, API_URL);

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600);
  api.begin();
}
StaticJsonDocument<JSON_SIZE> prepareJSON_LOG(int id_c, float valeur);
void loop() {
  // Read logs from UART2
  if (Serial2.available()) {
    String log = readLineFromSerial(Serial2);
    processLog(log);
  }

  // Relay data from PC Serial to UART2
  if (Serial.available()) {
    while (Serial.available()) {
      char c = Serial.read();
      Serial2.write(c);
    }
  }
}

StaticJsonDocument<JSON_SIZE> prepareJSON_LOG(int id_c, float valeur){
  StaticJsonDocument<JSON_SIZE> jsonDoc;
  jsonDoc["id_c"] = id_c;
  jsonDoc["valeur"] = valeur;
  return jsonDoc;
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
  // Example: Parse a command like "VALIDER 123456789ABCDEF0" 16 nibbles
  if (log.startsWith("VALIDER ")) {
    String badgeId = log.substring(8); // Extract badge ID
    bool valid = checkActif checkBadge(badgeId);

    // Respond back to the machine
    Serial2.println(valid ? "OUI" : "NON");
    return;
  }

  if (!log.startsWith("<") || !log.endsWith(">")) {
    Serial.println("Invalid log format");
    return;
  }

  // Remove the surrounding '<' and '>'
  String cleanLog = log.substring(1, log.length() - 1);

  // Split the log into parts
  int comma1 = cleanLog.indexOf(',');
  int comma2 = cleanLog.indexOf(',', comma1 + 1);
  int comma3 = cleanLog.indexOf(',', comma2 + 1);

  String idMachine = cleanLog.substring(0, comma1);
  String operation = cleanLog.substring(comma1 + 1, comma2);
  String data = (comma3 != -1) ? cleanLog.substring(comma2 + 1, comma3) : cleanLog.substring(comma2 + 1);

  // Prepare JSON object
  StaticJsonDocument<JSON_SIZE> jsonDoc;
  jsonDoc["id_machine"] = idMachine.toInt(); // Convert to integer
  jsonDoc["operation"] = operation;
  jsonDoc["timestamp"] = millis(); // ESP32-generated timestamp

  // Handle different operations
  if (operation == "E" || operation == "R") {
    // For E (Entrée) and R (Retour) operations
    int subComma = data.indexOf(',');
    String idBadge = data.substring(0, subComma);
    String idCaisse = data.substring(subComma + 1);

    jsonDoc["data"]["id_badge"] = idBadge;
    jsonDoc["data"]["id_caisse"] = idCaisse.toInt();
  } else if (operation == "A") {
    // For A (Anomalie) operations
    String typeAnomalie = data;
    jsonDoc["data"]["type_anomalie"] = (typeAnomalie == "B") ? "BOUTTON" : (typeAnomalie == "S") ? "SERVO" : "UNKNOWN";
  } else if (operation == "D") {
    // For D (Déconnecté) operations
    jsonDoc["data"] = nullptr; // No additional data for D
  } else {
    Serial.println("Unknown operation type");
    return;
  }

  // Send the formatted JSON to the API
  bool success = sendData(jsonDoc, "/logs", "POST");
  if (success) {
    Serial.println("Log sent successfully");
  } else {
    Serial.println("Failed to send log");
  }
}