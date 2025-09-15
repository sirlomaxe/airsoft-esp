#include "GameManager.h"
#include <Arduino.h>

// Définitions des variables globales
// Ces variables seront gérées par le GameManager
extern int BOMB_GAME_TIME_SECONDS;
extern char bombCode[5];
const int WIRE_SEQUENCE[3] = {1, 3, 2};
int wiresCutCount = 0;
int lastBeepTime = 0;
char codeBuffer[5] = "";
int bufferIndex = 0;

// Variables de gestion du temps (temps restant)
unsigned long startTime = 0;
unsigned long elapsedTime = 0;

GameManager::GameManager(InputManager& input, DisplayManager& display, WireManager& wires, BuzzerManager& buzzer, MotionManager& motion)
    : inputManager(input), displayManager(display), wireManager(wires), buzzerManager(buzzer), motionManager(motion) {
    currentState = STANDBY_MODE;
    vipCurrentHealth = VIP_HEALTH_POINTS;
}

void GameManager::setup() {
    displayManager.showMessage("En attente de config", 0);
}

extern bool vipMode; // Déclarez la variable globale ici

void GameManager::loop() {
    char key = inputManager.getKey();

    switch (currentState) {
        case STANDBY_MODE: {
            if (key == '*') {
                if (vipMode) {
                    currentState = VIP_MODE;
                    startTime = millis();
                    displayManager.showMessage("MODE VIP - VIE : 100", 0);
                    vipCurrentHealth = VIP_HEALTH_POINTS; // Réinitialise les points de vie
                } else {
                    currentState = GAME_MODE; // Mode normal
                    startTime = millis();
                    displayManager.showMessage("Temps restant:", 0);
                }
            }
        }
        break;
        
        case GAME_MODE: {
            elapsedTime = (millis() - startTime) / 1000;
            int remainingTime = BOMB_GAME_TIME_SECONDS - elapsedTime;

            // Logique de fin de jeu
            if (remainingTime <= 0) {
                currentState = LOSE_MODE;
                displayManager.showMessage("BOOM !!!", 0);
                buzzerManager.playExplosion();
                return;
            }

            // Logique de désamorçage par les fils
            int cutWire = wireManager.checkCutWire();
            if (cutWire != 0) {
                if (cutWire == WIRE_SEQUENCE[wiresCutCount]) {
                    wiresCutCount++;
                    displayManager.showMessage("Fil correct!", 0);
                    delay(1000);
                    displayManager.showMessage("                ", 0);

                    if (wiresCutCount >= 3) {
                        currentState = WIN_MODE;
                    }
                } else {
                    currentState = LOSE_MODE;
                    displayManager.showMessage("Mauvais fil!", 0);
                    delay(1000);
                    displayManager.showMessage("BOOM !!!", 0);
                }
            }

            // Gestion du clavier
            if (key != NO_KEY) {
                if (key == '#') {
                    if (strcmp(codeBuffer, bombCode) == 0) {
                        currentState = WIN_MODE;
                        displayManager.showMessage("BOMBE DESAMORCEE!", 0);
                    } else {
                        startTime -= 30 * 1000;
                        displayManager.showMessage("CODE FAUX !", 0);
                        delay(1000);
                        displayManager.showMessage("", 0);
                    }
                    bufferIndex = 0;
                    memset(codeBuffer, 0, sizeof(codeBuffer));
                } else if (key == '*') {
                    if (bufferIndex > 0) {
                        bufferIndex--;
                        codeBuffer[bufferIndex] = 0;
                    }
                } else if (key >= '0' && key <= '9' && bufferIndex < 4) {
                    codeBuffer[bufferIndex] = key;
                    bufferIndex++;
                    codeBuffer[bufferIndex] = 0;
                }
            }

            // Affiche le temps et le code
            int minutes = remainingTime / 60;
            int seconds = remainingTime % 60;
            displayManager.showTimeAndCode(remainingTime, codeBuffer);

            // Bip toutes les 5 secondes
            if (seconds % 5 == 0 && seconds != lastBeepTime) {
                buzzerManager.playBeep();
                lastBeepTime = seconds;
            }
        }
        break;
        case VIP_MODE: {
            // Logique du mode VIP
            elapsedTime = (millis() - startTime) / 1000;
            int remainingTime = VIP_GAME_TIME - elapsedTime; // Utilise le temps du mode VIP

            // Vérifie les conditions de défaite
            if (remainingTime <= 0 || vipCurrentHealth <= 0) {
                currentState = LOSE_MODE;
                displayManager.showMessage("BOOM !!!", 0);
                buzzerManager.playExplosion();
                return;
            }

            // Détection des chocs
            if (motionManager.checkVibration()) {
                vipCurrentHealth -= 10; // Réduit la vie de 10 points par choc
                displayManager.showMessage("CHOC DETECTE !", 3);
                delay(500);
            }

            // Affiche la vie et le temps restant
            char healthMessage[21];
            sprintf(healthMessage, "VIE RESTANTE: %d", vipCurrentHealth);
            displayManager.showMessage(healthMessage, 1);

            char timeMessage[21];
            sprintf(timeMessage, "Temps: %02d:%02d", remainingTime / 60, remainingTime % 60);
            displayManager.showMessage(timeMessage, 2);

            // Gestion des fils
            int cutWire = wireManager.checkCutWire();
            if (cutWire != 0) {
                // ... (même logique que le mode normal)
            }
            
            // Gestion du clavier : pas de code possible, mais la pénalité reste
            if (key != NO_KEY) {
                if (key == '#') {
                    // Aucune action si le code est entré
                    startTime -= VIP_PENALTY * 1000; // Pénalité de 60 secondes pour toute tentative de code
                    displayManager.showMessage("Tentative de code, penalite!", 0);
                    delay(2000);
                }
            }

            // Affiche le temps et le code (pas de code dans ce mode)
            displayManager.showTimeAndCode(remainingTime, "");
            
            // Bips sonores plus fréquents en mode VIP (par ex, toutes les 2 secondes)
            if (remainingTime % 2 == 0 && remainingTime != lastBeepTime) {
                buzzerManager.playBeep();
                lastBeepTime = remainingTime;
            }
        }
        break;
        case WIN_MODE: {
            displayManager.showMessage("BOMBE DESAMORCEE!", 0);
            buzzerManager.playDefusal();
        }
        break;
        
        case LOSE_MODE: {
            displayManager.showMessage("BOOM !!!", 0);
            buzzerManager.playExplosion();
        }
        break;
    }
}