#include "GameManager.h"
#include <Arduino.h>

// Définitions des variables globales
extern bool startGameFromWeb; // Déclaration de la variable du fichier principal
extern int BOMB_GAME_TIME_SECONDS;
extern char bombCode[5];
const int WIRE_SEQUENCE[3] = {1, 3, 2};
int wiresCutCount = 0;
int lastBeepTime = 0;
char codeBuffer[5] = "";
int bufferIndex = 0;

unsigned long startTime = 0;
unsigned long elapsedTime = 0;
unsigned long lastKeyPressTime = 0;

// **Définissez vipMode ici (supprimez la ligne extern)**
extern bool vipMode;

GameManager::GameManager(InputManager& input, DisplayManager& display, WireManager& wires, BuzzerManager& buzzer, MotionManager& motion)
    : inputManager(input), displayManager(display), wireManager(wires), buzzerManager(buzzer), motionManager(motion) {
    currentState = IDLE_MODE;
    vipCurrentHealth = VIP_HEALTH_POINTS;
}

void GameManager::setup() {
    displayManager.showMessage("Configurez la bombe", 0);
    displayManager.showMessage(" 'B' pour le menu", 1);
}

void GameManager::loop() {
    char key = inputManager.getKey();

    switch (currentState) {
        case IDLE_MODE: {
            if (key == 'B') {
                currentState = MENU_TIME_MODE;
                displayManager.showMessage("Entrez le temps (s)", 0);
                displayManager.showMessage("   (ex: 180)", 1);
            }
        }
        break;

        case MENU_TIME_MODE: {
            if (key >= '0' && key <= '9' && tempBufferIndex < 4) {
                tempTimeBuffer[tempBufferIndex] = key;
                tempBufferIndex++;
                tempTimeBuffer[tempBufferIndex] = '\0';
                displayManager.showMessage(tempTimeBuffer, 2);
            } else if (key == '#') {
                BOMB_GAME_TIME_SECONDS = atoi(tempTimeBuffer);
                tempBufferIndex = 0;
                memset(tempTimeBuffer, 0, sizeof(tempTimeBuffer));
                currentState = MENU_CODE_MODE;
                displayManager.showMessage("Temps configure!", 0);
                displayManager.showMessage("Entrez le code", 1);
            }
        }
        break;

        case MENU_CODE_MODE: {
            if (key >= '0' && key <= '9' && tempBufferIndex < 4) {
                tempCodeBuffer[tempBufferIndex] = key;
                tempBufferIndex++;
                tempCodeBuffer[tempBufferIndex] = '\0';
                displayManager.showMessage(tempCodeBuffer, 2);
            } else if (key == '#') {
                strcpy(bombCode, tempCodeBuffer);
                tempBufferIndex = 0;
                memset(tempCodeBuffer, 0, sizeof(tempCodeBuffer));
                currentState = STANDBY_MODE;
                displayManager.showMessage("Code configure!", 0);
                displayManager.showMessage("Pret a jouer", 1);
            }
        }
        break;
        
        case STANDBY_MODE: {
            if (key == '*') {
                if (vipMode) { // Utilise vipMode
                    currentState = VIP_MODE;
                    startTime = millis();
                    displayManager.showMessage("MODE VIP !", 0);
                    vipCurrentHealth = VIP_HEALTH_POINTS;
                } else {
                    currentState = GAME_MODE;
                    startTime = millis();
                    displayManager.showMessage("BOMBE ACTIVEE", 0);
                }
            }
            // Nouvelle condition pour démarrer depuis le web
            if (startGameFromWeb) {
                if (vipMode) {
                    currentState = VIP_MODE;
                    startTime = millis();
                    displayManager.showMessage("MODE VIP !", 0);
                  vipCurrentHealth = VIP_HEALTH_POINTS;
                } else {
                    currentState = GAME_MODE;
                  startTime = millis();
                  displayManager.showMessage("BOMBE ACTIVEE", 0);
                }
                startGameFromWeb = false; // Réinitialiser le drapeau pour éviter des démarrages multiples
            }
        }
        break;
        
        case GAME_MODE: {
            elapsedTime = (millis() - startTime) / 1000;
            int remainingTime = BOMB_GAME_TIME_SECONDS - elapsedTime;

            if (remainingTime <= 0) {
                currentState = LOSE_MODE;
                displayManager.showMessage("BOOM !!!", 0);
                buzzerManager.playExplosion();
                return;
            }

            displayManager.showTimeAndCode(remainingTime, codeBuffer);

            int cutWire = wireManager.checkCutWire();
            if (cutWire != 0) {
                if (cutWire == WIRE_SEQUENCE[wiresCutCount]) {
                    wiresCutCount++;
                    displayManager.showMessage("Fil correct!", 3);
                    delay(1000);
                    displayManager.showMessage("                    ", 3);

                    if (wiresCutCount >= 3) {
                        currentState = WIN_MODE;
                    }
                } else {
                    currentState = LOSE_MODE;
                    displayManager.showMessage("Mauvais fil!", 3);
                    delay(1000);
                    displayManager.showMessage("BOOM !!!", 0);
                }
            }

            if (key != NO_KEY) {
                if (key == '#') {
                    if (strcmp(codeBuffer, bombCode) == 0) {
                        currentState = WIN_MODE;
                        displayManager.showMessage("BOMBE DESAMORCEE!", 0);
                    } else {
                        startTime += 30 * 1000;
                        displayManager.showMessage("CODE FAUX !", 3);
                        delay(1000);
                        displayManager.showMessage(" ", 3);
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
            
            if (remainingTime % 5 == 0 && remainingTime != lastBeepTime) {
                buzzerManager.playBeep();
                lastBeepTime = remainingTime;
            }
        }
        break;

        case VIP_MODE: {
            elapsedTime = (millis() - startTime) / 1000;
            int remainingTime = VIP_GAME_TIME - elapsedTime;

            if (remainingTime <= 0 || vipCurrentHealth <= 0) {
                currentState = LOSE_MODE;
                displayManager.showMessage("BOOM !!!", 0);
                buzzerManager.playExplosion();
                return;
            }

            if (motionManager.checkVibration()) {
                vipCurrentHealth -= 10;
                displayManager.showMessage("CHOC DETECTE !", 3);
                delay(500);
            }

            char healthMessage[21];
            sprintf(healthMessage, "VIE: %d", vipCurrentHealth);
            displayManager.showMessage(healthMessage, 1);

            char timeMessage[21];
            sprintf(timeMessage, "Temps: %02d:%02d", remainingTime / 60, remainingTime % 60);
            displayManager.showMessage(timeMessage, 2);

            int cutWire = wireManager.checkCutWire();
            if (cutWire != 0) {
                 if (cutWire == WIRE_SEQUENCE[wiresCutCount]) {
                    wiresCutCount++;
                    displayManager.showMessage("Fil correct!", 3);
                    delay(1000);
                    displayManager.showMessage("                    ", 3);

                    if (wiresCutCount >= 3) {
                        currentState = WIN_MODE;
                    }
                } else {
                    currentState = LOSE_MODE;
                    displayManager.showMessage("Mauvais fil!", 3);
                    delay(1000);
                    displayManager.showMessage("BOOM !!!", 0);
                }
            }
            
            if (key != NO_KEY) {
                if (key == '#') {
                    startTime += VIP_PENALTY * 1000; 
                    displayManager.showMessage("Penalite : +60s !", 3);
                    delay(2000);
                    displayManager.showMessage(" ", 3);
                }
            }

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