#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <Keypad.h>

const byte ROWS = 4; // Nombre de lignes
const byte COLS = 4; // Nombre de colonnes

class InputManager {
public:
    // Constructeur : initialise les broches du clavier
    InputManager(byte rowPins[], byte colPins[]);

    // Méthode pour lire la touche pressée
    char getKey();

private:
    char keys[ROWS][COLS] = {
        {'1','2','3', 'H'},
        {'4','5','6', 'B'},
        {'7','8','9', 'G'},
        {'*','0','#', 'D'}
    };
    Keypad keypad;
};

#endif