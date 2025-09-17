#include "GameManager.h"
#include <Arduino.h>

// Déclaration externe de la variable globale pour le démarrage depuis le web
extern bool startGameFromWeb; 

// Variables globales globales pour le temps de jeu (devraient être définies ici ou ailleurs globalement si utilisées par d'autres fichiers)
// Si elles sont uniquement utilisées par GameManager, il est plus propre de les y mettre.
int BOMB_GAME_TIME_SECONDS_GLOBAL = 180; // Si besoin d'une valeur globale
char bombCode_GLOBAL[5] = ""; // Si besoin d'une valeur globale
bool vipMode_GLOBAL = false; // Si besoin d'une valeur globale

GameManager::GameManager(InputManager& input, DisplayManager& display, WireManager& wires, BuzzerManager& buzzer, MotionManager& motion)
    : inputManager(input), displayManager(display), wireManager(wires), buzzerManager(buzzer), motionManager(motion), currentState(IDLE_MODE) {}

void GameManager::setup() {
    // Ici, nous initialisons les managers qui nous sont passés en paramètres.
    // Les autres initialisations (comme Wire.begin()) devraient être faites dans le setup() principal.
    
    // Afficher le message de démarrage initial
    displayManager.showMessage("Systeme arme", 0);
    displayManager.showMessage("Prêt pour le jeu", 1);

    // Initialisation des managers nécessaires à GameManager
    inputManager.setup();
    wireManager.setup(); // Assurez-vous que WireManager est bien configuré (par ex. pour les fils)
    buzzerManager.setup();
    motionManager.setup(); // Initialise le capteur de vibration

    // Le code de démarrage Wi-Fi et serveur web est géré dans le setup() principal.
    
    // Initialiser les variables de jeu
    startTime = millis();
    gameEnded = false;
    dominatingTeam = NONE;
    dominationStartTime = 0;
    vipCurrentHealth = vipMode ? VIP_HEALTH_POINTS : 0; // Initialiser la santé VIP si le mode est actif

    // Utiliser les valeurs globales si elles existent, sinon des valeurs par défaut
    BOMB_GAME_TIME_SECONDS = BOMB_GAME_TIME_SECONDS_GLOBAL;
    strncpy(bombCode, bombCode_GLOBAL, sizeof(bombCode) - 1);
    bombCode[sizeof(bombCode) - 1] = '\0';
    vipMode = vipMode_GLOBAL;
}

void GameManager::loop() {
    char key = inputManager.getKey(); // Lire la touche pressée

    switch (currentState) {
        case IDLE_MODE:
            // Mode initial, en attente d'une commande pour démarrer le jeu
            displayManager.showMessage("Systeme pret", 0);
            displayManager.showMessage("Appuyez sur *", 1);
            if (key == '*') {
                currentState = STANDBY_MODE;
                startTime = millis(); // Réinitialise le timer quand on passe en mode veille
            }
            break;

        case MENU_TIME_MODE:
            // Menu pour régler le temps de jeu
            if (key != '\0') { // Si une touche est pressée
                if (key == '#') { // Confirmer et revenir au menu principal
                    currentState = STANDBY_MODE;
                    displayManager.showMessage("Temps regle!", 2);
                    delay(1000);
                } else if (isdigit(key)) {
                    // Logique pour entrer le temps (ex: 3 chiffres)
                    // Ceci est simplifié ici, une implémentation complète serait plus complexe
                    // Pour l'instant, on assume que la saisie est gérée ailleurs ou directement dans le setup
                }
            }
            break;
        
        case MENU_CODE_MODE:
            // Menu pour régler le code de la bombe (si implémenté)
            displayManager.showMessage("Entrez code", 0);
            // ... logique de saisie de code ...
            if (key == '#') {
                currentState = STANDBY_MODE;
            }
            break;

        case STANDBY_MODE:
            // Mode veille, prêt à démarrer le jeu
            displayManager.showMessage("En attente...", 0);
            displayManager.showMessage("Temps: " + String(BOMB_GAME_TIME_SECONDS), 1);
            displayManager.showMessage("VIP:", 2);
            displayManager.showMessage(vipMode ? "OUI" : "NON", 2, 7);

            // Démarrer le jeu standard avec la touche '*'
            if (key == '*') {
                currentState = GAME_MODE;
                startTime = millis();
                displayManager.showMessage("BOMBE DETECTEE", 0);
                displayManager.showMessage("Code: ???", 1); // Afficher un placeholder pour le code
                buzzerManager.startBeeping(1000); // Optionnel: démarrer le bip
                gameEnded = false;
            }

            // Démarrer le jeu VIP avec la touche 'VIP_KEY' (si vous en définissez une)
            // Exemple : si la touche 'V' démarre le mode VIP
            if (key == 'V') { 
                vipMode = true;
                currentState = VIP_MODE;
                startTime = millis();
                vipCurrentHealth = VIP_HEALTH_POINTS; // Réinitialiser la santé VIP
                displayManager.showMessage("MODE VIP!", 0);
                buzzerManager.startBeeping(500); // Optionnel: bip différent pour VIP
                gameEnded = false;
            }

            // Démarrer le mode Domination avec la touche 'A'
            if (key == 'A') { 
                currentState = DOMINATION_MODE;
                startTime = millis(); // Timer principal du jeu
                dominatingTeam = NONE; // Réinitialiser l'équipe dominante
                dominationStartTime = millis(); // Réinitialiser le timer de domination
                displayManager.showMessage("DOMINATION MODE", 0);
                displayManager.showMessage("Attaquez!", 1);
                buzzerManager.startBeeping(700);
                gameEnded = false;
            }
            
            // Démarrage depuis le web (si la variable est mise à true)
            if (startGameFromWeb) {
                // Ici, vous pouvez décider quel mode démarrer par défaut (ex: GAME_MODE)
                // Ou vous pourriez avoir une logique pour choisir le mode depuis la page web
                currentState = GAME_MODE; // Par défaut, on lance le mode jeu normal
                startTime = millis();
                displayManager.showMessage("BOMBE DETECTEE", 0);
                displayManager.showMessage("Code: ???", 1); 
                buzzerManager.startBeeping(1000);
                gameEnded = false;
                startGameFromWeb = false; // Important: réinitialiser la variable
            }
            break;

        case GAME_MODE: {
            int elapsedTime = (millis() - startTime) / 1000;
            int remainingTime = BOMB_GAME_TIME_SECONDS - elapsedTime;
            
            // Affichage du temps restant et du code (placeholder pour l'instant)
            char timeBuffer[20];
            sprintf(timeBuffer, "Temps: %02d:%02d", remainingTime / 60, remainingTime % 60);
            displayManager.showMessage(timeBuffer, 0);
            displayManager.showMessage("Code: ???", 1); // Afficher un placeholder

            // Vérification des fils
            if (wireManager.checkWires()) {
                currentState = WIN_MODE;
                gameEnded = true;
                displayManager.showMessage("Desamorcee!", 0);
                displayManager.showMessage("Felicitation!", 1);
                buzzerManager.playSuccess();
            }
            
            // Vérification si la bombe a explosé
            if (remainingTime <= 0) {
                currentState = LOSE_MODE;
                gameEnded = true;
                displayManager.showMessage("BOMBE EXPLOSÉE!", 0);
                displayManager.showMessage("Dommage!", 1);
                buzzerManager.playExplosion();
            }
        }
        break;

        case VIP_MODE: {
            int elapsedTime = (millis() - startTime) / 1000;
            int remainingTime = BOMB_GAME_TIME_SECONDS - elapsedTime;

            displayManager.showTimeAndCode(remainingTime, bombCode); // Utiliser le code vrai si défini

            // Logique de détection de vibration pour le mode VIP
            if (motionManager.checkVibration()) {
                // Si le capteur détecte une vibration
                vipCurrentHealth -= 10; // Perte de points de vie
                if (vipCurrentHealth < 0) vipCurrentHealth = 0;
                buzzerManager.playHit(); // Jouer un son de "hit"
                displayManager.showMessage("DANGER!", 3); // Message d'alerte
            } else {
                displayManager.showMessage("Securise", 3); // Indication que tout va bien
            }
            
            // Mise à jour de la santé VIP sur l'écran
            displayManager.showMessage("Vie VIP: " + String(vipCurrentHealth), 2);

            // Conditions de fin de jeu pour le mode VIP
            if (vipCurrentHealth <= 0) {
                currentState = LOSE_MODE;
                gameEnded = true;
                displayManager.showMessage("VIP KO!", 0);
                displayManager.showMessage("Bombe explosee", 1);
                buzzerManager.playExplosion();
            } else if (remainingTime <= 0) {
                currentState = WIN_MODE;
                gameEnded = true;
                displayManager.showMessage("BOMBE DESACTIVEE", 0);
                displayManager.showMessage("Bien joue!", 1);
                buzzerManager.playSuccess();
            }
        }
        break;

        case DOMINATION_MODE: {
            int elapsedTime = (millis() - startTime) / 1000;
            int remainingTime = BOMB_GAME_TIME_SECONDS - elapsedTime;
            
            displayManager.showTimeAndCode(remainingTime, "Domination");

            // Détection du capteur de vibration pour la domination
            if (motionManager.checkVibration()) {
                // Changer l'équipe qui domine et réinitialiser le timer de domination
                if (dominatingTeam == RED) {
                    dominatingTeam = BLUE;
                } else {
                    dominatingTeam = RED;
                }
                dominationStartTime = millis(); // Réinitialiser le timer de domination
                displayManager.showMessage("Domination!", 2); // Afficher un message
                buzzerManager.playDominationSwitch(); // Son pour indiquer un changement d'équipe
            }

            // Afficher l'équipe qui domine et le temps de domination
            if (dominatingTeam == RED) {
                displayManager.showMessage("Equipe: ROUGE", 3);
                int dominationElapsedTime = (millis() - dominationStartTime) / 1000;
                displayManager.showMessage("Dom: " + String(DOMINATION_TIME_SECONDS - dominationElapsedTime), 2);
            } else if (dominatingTeam == BLUE) {
                displayManager.showMessage("Equipe: BLEUE", 3);
                int dominationElapsedTime = (millis() - dominationStartTime) / 1000;
                displayManager.showMessage("Dom: " + String(DOMINATION_TIME_SECONDS - dominationElapsedTime), 2);
            } else {
                displayManager.showMessage("Aucune domination", 3); // Si aucune équipe ne domine
                displayManager.showMessage("   ", 2); // Effacer la ligne de temps de domination
            }

            // Vérifier la condition de victoire pour le mode domination
            if (dominatingTeam != NONE && (millis() - dominationStartTime) / 1000 >= DOMINATION_TIME_SECONDS) {
                currentState = WIN_MODE;
                gameEnded = true;
                if (dominatingTeam == BLUE) {
                    displayManager.showMessage("EQUIPE BLEUE GAGNE!", 0);
                } else {
                    displayManager.showMessage("EQUIPE ROUGE GAGNE!", 0);
                }
                buzzerManager.playSuccess();
            }
            
            // Condition de défaite si le temps principal s'écoule
            if (remainingTime <= 0) {
                currentState = LOSE_MODE;
                gameEnded = true;
                displayManager.showMessage("BOMBE EXPLOSÉE!", 0);
                displayManager.showMessage("Temps ecoule", 1);
                buzzerManager.playExplosion();
            }
        }
        break;

        case WIN_MODE:
        case LOSE_MODE:
            // Afficher le résultat final pendant un certain temps
            if (!gameEnded) {
                gameEnded = true; // Marquer comme géré pour éviter de le refaire
                // Ici, vous pourriez jouer un son final spécifique pour victoire/défaite
            }
            
            // Après un délai, retourner au mode IDLE pour recommencer
            if ((millis() - startTime) / 1000 > 10) { // Attendre 10 secondes avant de revenir
                currentState = IDLE_MODE;
                gameEnded = false;
                // Réinitialiser les variables de jeu pour le prochain tour
                dominatingTeam = NONE;
                dominationStartTime = 0;
                vipMode = false; // Réinitialiser le mode VIP
                vipCurrentHealth = 0;
                // ... réinitialiser d'autres variables si nécessaire ...
            }
            break;
    }
}