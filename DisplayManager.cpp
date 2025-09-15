#include "DisplayManager.h"
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>

DisplayManager::DisplayManager(int lcdAddress, int lcdCols, int lcdRows)
    : lcd(lcdAddress, lcdCols, lcdRows) {}

void DisplayManager::setup() {
    lcd.init();
    lcd.backlight();
}

void DisplayManager::showMessage(const char* message, int row) {
    lcd.setCursor(0, row);
    lcd.print("                    "); // Efface la ligne en affichant des espaces
    lcd.setCursor(0, row);
    lcd.print(message);
}

void DisplayManager::showTimeAndCode(int remainingTime, const char* codeBuffer) {
    // Affiche le temps sur la ligne 1
    int minutes = remainingTime / 60;
    int seconds = remainingTime % 60;
    char timeBuffer[21];
    sprintf(timeBuffer, "Temps: %02d:%02d", minutes, seconds);
    showMessage(timeBuffer, 1);
    
    // Affiche le code sur la ligne 2
    char codeDisplay[21];
    sprintf(codeDisplay, "Code: %s", codeBuffer);
    showMessage(codeDisplay, 2);
}

void DisplayManager::showWireStatus(int wire1, int wire2, int wire3) {
    char wireStatus[21];
    sprintf(wireStatus, "Fil: %d %d %d", wire1, wire2, wire3);
    showMessage(wireStatus, 3);
}