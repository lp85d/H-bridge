#include <WiFiManager.h>
#include <WiFi.h>
#include <WebServer.h>
#include <pgmspace.h>

// Пины для двигателей
int motorPins[2][2] = {{27, 26}, {32, 33}}; // Левый и правый двигатель

WebServer server(80);

// Переменные состояния
int selectedMotor = 0;        // Выбранный двигатель
String motorDirection = "STOP"; // Направление
bool motorRunning = false;    // Запущен ли двигатель

// Декларация HTML
extern const char index_html[]; // HTML из PROGMEM

void setup() {
  Serial.begin(115200);

  // Настройка пинов для двигателей
  for (auto& pin : motorPins) {
    pinMode(pin[0], OUTPUT);
    pinMode(pin[1], OUTPUT);
  }

  // Настройка Wi-Fi с помощью WiFiManager
  WiFiManager wifiManager;
  wifiManager.autoConnect("MotorControlAP");

  Serial.println("Wi-Fi подключен!");
  Serial.println(WiFi.localIP());

  // Маршруты
  server.on("/", handleRoot);
  server.on("/select", handleSelectMotor);
  server.on("/direction", handleDirection);
  server.on("/startstop", handleStartStopMotor);
  server.on("/status", handleStatus);

  server.begin();
  Serial.println("HTTP сервер запущен.");
}

void controlMotor(int motor, String direction) {
  if (motorRunning) {
    digitalWrite(motorPins[motor][0], direction == "CW" ? HIGH : LOW);
    digitalWrite(motorPins[motor][1], direction == "CW" ? LOW : HIGH);

    // Отключаем другой двигатель
    for (int i = 0; i < 2; ++i) {
      if (i != motor) {
        digitalWrite(motorPins[i][0], LOW);
        digitalWrite(motorPins[i][1], LOW);
      }
    }
  } else {
    digitalWrite(motorPins[motor][0], LOW);
    digitalWrite(motorPins[motor][1], LOW);
  }
}

// Маршрут для главной страницы
void handleRoot() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send_P(200, "text/html", index_html);
}

// Маршрут выбора двигателя
void handleSelectMotor() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  if (server.hasArg("motor")) {
    selectedMotor = server.arg("motor").toInt();
    Serial.println("Выбран двигатель: " + String(selectedMotor));
    controlMotor(selectedMotor, motorDirection);
    server.send(200, "text/plain", "Выбран двигатель: " + String(selectedMotor));
  }
}

// Маршрут смены направления
void handleDirection() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  if (server.hasArg("dir")) {
    motorDirection = server.arg("dir");
    Serial.println("Направление: " + motorDirection);
    if (motorRunning) {
      controlMotor(selectedMotor, motorDirection);
    }
    server.send(200, "text/plain", "Направление: " + motorDirection);
  }
}

// Маршрут старт/стоп двигателя
void handleStartStopMotor() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  motorRunning = !motorRunning;
  Serial.println(motorRunning ? "Двигатель запущен" : "Двигатель остановлен");
  controlMotor(selectedMotor, motorRunning ? motorDirection : "STOP");
  server.send(200, "text/plain", motorRunning ? "Двигатель запущен" : "Двигатель остановлен");
}

// Маршрут статуса
void handleStatus() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  String status = "{";
  status += "\"selectedMotor\":" + String(selectedMotor) + ",";
  status += "\"motorDirection\":\"" + motorDirection + "\",";
  status += "\"motorRunning\":" + String(motorRunning ? "true" : "false");
  status += "}";
  server.send(200, "application/json", status);
}

void loop() {
  server.handleClient();
}
