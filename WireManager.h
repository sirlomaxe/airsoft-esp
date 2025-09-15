#ifndef WIRE_MANAGER_H
#define WIRE_MANAGER_H

#include <Arduino.h>

class WireManager {
public:
    // Constructeur: initialise les broches des fils
    WireManager(const int wiresPins[], const int numWires);

    // Initialise les broches
    void setup();

    // Vérifie si un fil a été coupé
    int checkCutWire();
    
private:
    const int* wiresPins;
    const int numWires;
    int lastCutWireIndex = -1; // -1 pour aucun fil coupé
};

#endif