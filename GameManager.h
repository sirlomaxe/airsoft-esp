#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "InputManager.h"
#include "DisplayManager.h"
#include "WireManager.h"
#include "BuzzerManager.h"
#include "MotionManager.h" // Ajout du MotionManager

// Déclaration de la variable 'vipMode' comme externe
extern bool vipMode;

// Enumération des états possibles de la bombe
enum BombState {
    CONFIG_MODE,
    STANDBY_MODE,
    GAME_MODE,
    VIP_MODE, // Nouvel état
    WIN_MODE,
    LOSE_MODE
};


// Enumération des étapes du mode de configuration
enum ConfigState {
    CONFIG_TIME,
    CONFIG_CODE
};

class GameManager {
public:
    GameManager(InputManager& input, DisplayManager& display, WireManager& wires, BuzzerManager& buzzer, MotionManager& motion);
    void setup();
    void loop();

private:
    InputManager& inputManager;
    DisplayManager& displayManager;
    WireManager& wireManager;
    BuzzerManager& buzzerManager;
    MotionManager& motionManager; // Référence au MotionManager
    BombState currentState;

    ConfigState currentConfigState; // Nouvelle variable pour la configuration

    // Variables du mode de configuration
    char inputBuffer[10] = "";
    int bufferIndex = 0;
    const int VIP_GAME_TIME = 180; // Par exemple, 3 minutes
    const int VIP_PENALTY = 60; // 60 secondes de pénalité
    const int VIP_HEALTH_POINTS = 1000; // 100 points de vie pour le mode VIP
    int vipCurrentHealth;
};

#endif