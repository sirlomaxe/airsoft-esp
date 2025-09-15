#include "InputManager.h"

InputManager::InputManager(byte rowPins[], byte colPins[])
    : keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS) {
    // Le constructeur de la classe Keypad est appelé ici
}

char InputManager::getKey() {
    return keypad.getKey();
}