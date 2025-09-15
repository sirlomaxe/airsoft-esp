#include "InputManager.h"
#include "DisplayManager.h"
#include "GameManager.h"
#include "WireManager.h"
#include "BuzzerManager.h"
#include "MotionManager.h"
#include <WiFi.h>
#include <WebServer.h>

// Declare and DEFINES global variables HERE ONCE
int BOMB_GAME_TIME_SECONDS = 0;
char bombCode[5] = "";
bool vipMode = false; // <-- THIS IS THE DEFINITION
volatile bool startGameFromWeb = false;

// Other manager declarations...
byte PIN_ROWS[4] = {27, 26, 25, 33};
byte PIN_COLS[4] = {32, 14, 12, 13};
InputManager inputManager(PIN_ROWS, PIN_COLS);

const int WIRE_PINS[3] = {15, 4, 16};
WireManager wireManager(WIRE_PINS, 3);

const int LCD_ADDRESS = 0x27;
const int LCD_COLS = 20;
const int LCD_ROWS = 4;
DisplayManager displayManager(LCD_ADDRESS, LCD_COLS, LCD_ROWS);

const int BUZZER_PIN = 17;
BuzzerManager buzzerManager(BUZZER_PIN);

// Instanciation du MotionManager avec le nouveau constructeur
const int VIBRATION_PIN = 23;
MotionManager motionManager(VIBRATION_PIN);

// Create the web server on port 80
WebServer server(80);

// The GameManager that handles the main logic
GameManager gameManager(inputManager, displayManager, wireManager, buzzerManager, motionManager);

// Function declarations for web server handlers
void handleRoot();
void handleConfiguration();

void setup() {
    Serial.begin(115200);
    // Initialisation du bus I2C EN PREMIER
    //Wire.begin(21, 22);
    displayManager.setup();
    // --- ÉCRAN DE CHARGEMENT ---
    displayManager.showMessage("Chargement", 0);
    displayManager.showMessage("Bombe Airsoft", 1);    
    // Animation de points qui clignotent
    for (int i = 0; i < 4; i++) {
        displayManager.showMessage(".", 2, i * 2);
        delay(600);
    }
    displayManager.clear(); // Efface l'écran une fois l'animation terminée
    // --- FIN DE L'ÉCRAN DE CHARGEMENT ---
    displayManager.showMessage("Demarrage Wi-Fi...", 0);
    
    // Initialisation du Wi-Fi en mode point d'accès
    WiFi.softAP("Bombe_Airsoft", "pass1234");
    IPAddress IP = WiFi.softAPIP();
    displayManager.showMessage("Connectez-vous a:", 1);
    displayManager.showMessage(IP.toString().c_str(), 2);
    Serial.print("Serveur IP: ");
    Serial.println(IP);

    // Démarrer le serveur Web après que le WiFi est opérationnel
    server.on("/", handleRoot);
    server.on("/config", handleConfiguration);
    server.on("/start", handleStart); // Nouvelle route pour démarrer le jeu
    server.begin();
    //scanI2C();
    // Initialisation des managers après le serveur Web
    motionManager.setup(); 
    wireManager.setup();
    buzzerManager.setup();
    
    // L'initialisation du GameManager se fait en dernier
    gameManager.setup();
    
}

void loop() {
    server.handleClient();
    gameManager.loop();
}

void handleRoot() {
    String html = "<html><head><title>Configuration de la bombe</title></head>";
    html += "<body><h1>Configuration de la bombe</h1>";
    html += "<form action='/config' method='get'>";
    html += "Temps (en secondes): <input type='number' name='time'><br>";
    html += "Code de desamorcage: <input type='text' name='code'><br>";
    html += "<input type='checkbox' name='vip_mode'> Mode VIP<br>";
    html += "<input type='submit' value='Soumettre'>";
    html += "</form></body></html>";
    server.send(200, "text/html", html);
}

void handleConfiguration() {
    if (server.hasArg("time")) {
        BOMB_GAME_TIME_SECONDS = server.arg("time").toInt();
    }
    if (server.arg("code").length() > 0) { // Check if code argument is present and not empty
        String codeStr = server.arg("code");
        codeStr.toCharArray(bombCode, sizeof(bombCode)); // Use sizeof for safety
    }
    
    if (server.hasArg("vip_mode")) {
        vipMode = true;
    } else {
        vipMode = false;
    }
    
    server.send(200, "text/plain", "Configuration terminee. La bombe est prete.");
}
void handleStart() {
    startGameFromWeb = true;
    server.send(200, "text/plain", "Jeu demarre depuis le web!");
}
void scanI2C() {
  byte error, address;
  int nDevices;

  Serial.println("Scanning I2C bus...");

  nDevices = 0;
  for (address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");
      nDevices++;
    } else if (error == 4) {
      Serial.print("Unknow error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
}