#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class DisplayManager {
public:
    DisplayManager(int lcdAddress, int lcdCols, int lcdRows);
    void setup();
    void showMessage(const char* message, int row);
    void showMessage(const char* message, int row, int col); // Nouvelle fonction surchargée
    void clear(); // Nouvelle fonction pour effacer l'écran
    // Ajoutez les déclarations des nouvelles fonctions ici
    void showTimeAndCode(int remainingTime, const char* codeBuffer);
    void showWireStatus(int wire1, int wire2, int wire3);
    void clearRow(int row);

private:
    LiquidCrystal_I2C lcd;
};

#endif