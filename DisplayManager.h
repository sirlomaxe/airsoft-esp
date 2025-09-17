#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class DisplayManager {
public:
    DisplayManager(int lcdCols, int lcdRows, int i2cAddress);
    void setup();
    void showMessage(const char* message, int row);
    void showMessage(const char* message, int row, int col); // Surcharge pour positionnement
    void showMessage(String message, int row);             // Surcharge pour String objects
    void showMessage(String message, int row, int col);     // Surcharge pour String objects avec position
    void clear(); 
    void clearRow(int row);
    void showTimeAndCode(int remainingTime, const char* codeBuffer);

private:
    LiquidCrystal_I2C lcd;
};

#endif