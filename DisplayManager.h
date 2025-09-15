#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class DisplayManager {
public:
    DisplayManager(int lcdAddress, int lcdCols, int lcdRows);
    void setup();
    void showMessage(const char* message, int row);
    
    // Ajoutez les déclarations des nouvelles fonctions ici
    void showTimeAndCode(int remainingTime, const char* codeBuffer);
    void showWireStatus(int wire1, int wire2, int wire3);

private:
    LiquidCrystal_I2C lcd;
};

#endif