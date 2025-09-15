#include "WireManager.h"

WireManager::WireManager(const int wiresPins[], const int numWires)
    : wiresPins(wiresPins), numWires(numWires) {}

void WireManager::setup() {
    for (int i = 0; i < numWires; i++) {
        pinMode(wiresPins[i], INPUT_PULLUP);
    }
}

int WireManager::checkCutWire() {
    for (int i = 0; i < numWires; i++) {
        // Un fil est coupé quand la broche est à LOW
        if (digitalRead(wiresPins[i]) == LOW && i != lastCutWireIndex) {
            lastCutWireIndex = i;
            return i + 1; // Renvoie le numéro du fil (1, 2, 3...)
        }
    }
    return 0; // Aucun fil n'a été coupé
}