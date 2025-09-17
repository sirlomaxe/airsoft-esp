#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "InputManager.h"
#include "DisplayManager.h"
#include "WireManager.h"
#include "BuzzerManager.h"
#include "MotionManager.h"

// Enumérations pour les états de jeu et les équipes
enum GameState {
    IDLE_MODE, MENU_TIME_MODE, MENU_CODE_MODE, STANDBY_MODE, GAME_MODE, VIP_MODE, WIN_MODE, LOSE_MODE, DOMINATION_MODE
};

enum Team {
    NONE, BLUE, RED
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
    MotionManager& motionManager;

    GameState currentState;

    // Variables pour le jeu normal et VIP
    unsigned long startTime = 0;
    int BOMB_GAME_TIME_SECONDS = 180; // Durée par défaut du jeu
    char bombCode[5] = "";
    bool vipMode = false;
    int vipCurrentHealth = 0;
    const int VIP_HEALTH_POINTS = 100; // Points de vie initiaux pour le mode VIP

    // Variables pour les états de victoire/défaite
    bool gameEnded = false;

    // Variables pour le mode domination
    Team dominatingTeam = NONE;
    unsigned long dominationStartTime = 0;
    const int DOMINATION_TIME_SECONDS = 60; // Durée de domination pour gagner
};

#endif