#include "DisplayManager.h"
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>

DisplayManager::DisplayManager(int lcdCols, int lcdRows, int i2cAddress)
    : lcd(i2cAddress, lcdCols, lcdRows) {}

void DisplayManager::setup() {
    lcd.init();
    lcd.backlight();
}

void DisplayManager::showMessage(const char* message, int row) {
    lcd.setCursor(0, row);
    lcd.print("                    "); // Efface la ligne
    lcd.setCursor(0, row);
    lcd.print(message);
}

void DisplayManager::showMessage(const char* message, int row, int col) {
    lcd.setCursor(col, row);
    lcd.print(message);
}

// Implémentation pour les objets String
void DisplayManager::showMessage(String message, int row) {
    showMessage(message.c_str(), row); // Convertit String en const char*
}

void DisplayManager::showMessage(String message, int row, int col) {
    showMessage(message.c_str(), row, col); // Convertit String en const char*
}

void DisplayManager::clear() {
    lcd.clear();
}

void DisplayManager::clearRow(int row) {
    lcd.setCursor(0, row);
    lcd.print("                    ");
}

void DisplayManager::showTimeAndCode(int remainingTime, const char* codeBuffer) {
    char timeMessage[21];
    sprintf(timeMessage, "Temps: %02d:%02d", remainingTime / 60, remainingTime % 60);
    showMessage(timeMessage, 0);

    char codeMessage[21];
    sprintf(codeMessage, "Code: %s", codeBuffer);
    showMessage(codeMessage, 1);
}