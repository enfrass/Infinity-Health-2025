#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "SEU_WIFI";
const char* password = "SUA_SENHA";

WebServer server(80);

const int LED_PIN = 2;
const int LDR_PIN = 34;  // Um pino analógico do ESP32 (ex: GPIO34)

bool ledLigado = false;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi conectado. IP: " + WiFi.localIP().toString());

  // Liga LED
  server.on("/start", HTTP_GET, []() {
    digitalWrite(LED_PIN, HIGH);
    ledLigado = true;
    server.send(200, "text/plain", "LED ligado");
  });

  // Desliga LED
  server.on("/stop", HTTP_GET, []() {
    digitalWrite(LED_PIN, LOW);
    ledLigado = false;
    server.send(200, "text/plain", "LED desligado");
  });

  // Status do sensor de luz
  server.on("/status", HTTP_GET, []() {
    if (!ledLigado) {
      server.send(200, "text/plain", "Inativo");
      return;
    }

    int leitura = analogRead(LDR_PIN);
    Serial.println("LDR: " + String(leitura));

    String status = (leitura < 1500) ? "Tampado" : "Aberto";
    server.send(200, "text/plain", status);
  });

  server.begin();
  Serial.println("Servidor iniciado.");
}

void loop() {
  server.handleClient();
}
