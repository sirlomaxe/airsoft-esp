#include "InputManager.h"
#include "DisplayManager.h"
#include "GameManager.h"
#include "WireManager.h"
#include "BuzzerManager.h"
#include "MotionManager.h"
#include <WiFi.h>
#include <WebServer.h>

// Déclaration et définition des variables globales
int BOMB_GAME_TIME_SECONDS = 0;
char bombCode[5] = "";
bool vipMode = false; // Ne pas déclarer deux fois !
// Déclarations des managers
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

// Déclaration du MotionManager AVANT le GameManager
MotionManager motionManager;

// Création du serveur Web sur le port 80
WebServer server(80);

// Le GameManager qui gère la logique principale
// Il peut maintenant utiliser motionManager car il est déjà déclaré.
GameManager gameManager(inputManager, displayManager, wireManager, buzzerManager, motionManager);

// Déclaration de la fonction qui gérera les requêtes HTTP
void handleRoot();
void handleConfiguration();

void setup() {
    Serial.begin(115200);
    displayManager.setup();
    displayManager.showMessage("Demarrage Wi-Fi...", 0);
    
    // Initialisation du Wi-Fi en mode point d'accès
    WiFi.softAP("Bombe_Airsoft", "pass1234");
    IPAddress IP = WiFi.softAPIP();
    displayManager.showMessage("Connectez-vous a:", 1);
    displayManager.showMessage(IP.toString().c_str(), 2);
    Serial.print("Serveur IP: ");
    Serial.println(IP);

    // Initialisation du serveur Web
    server.on("/", handleRoot);
    server.on("/config", handleConfiguration);
    server.begin();

    // Initialisation des managers
    wireManager.setup();
    buzzerManager.setup();
    motionManager.setup();
    gameManager.setup();
}

void loop() {
    server.handleClient();
    gameManager.loop();
}

// Fonctions de gestion du serveur web
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
    if (server.hasArg("code")) {
        String codeStr = server.arg("code");
        codeStr.toCharArray(bombCode, codeStr.length() + 1);
    }
    
    if (server.hasArg("vip_mode")) {
        vipMode = true;
    } else {
        vipMode = false;
    }
    
    server.send(200, "text/plain", "Configuration terminee. La bombe est prete.");
}