#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "InputManager.h"
#include "DisplayManager.h"
#include "WireManager.h"
#include "BuzzerManager.h"
#include "MotionManager.h" 

extern bool vipMode;

enum BombState {
    IDLE_MODE,
    MENU_TIME_MODE,
    MENU_CODE_MODE,
    STANDBY_MODE,
    GAME_MODE,
    VIP_MODE,
    WIN_MODE,
    LOSE_MODE
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
    BombState currentState;
    
    unsigned long lastKeyPressTime = 0;
    const int VIP_PENALTY = 60;
    
    char tempTimeBuffer[5] = "";
    char tempCodeBuffer[5] = "";
    int tempBufferIndex = 0;
    int timeEntered = 0; 
    
    const int VIP_GAME_TIME = 180;
    const int VIP_HEALTH_POINTS = 100;
    int vipCurrentHealth;
};

#endif