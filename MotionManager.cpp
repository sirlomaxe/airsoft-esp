#include "MotionManager.h"
#include <Arduino.h>

MotionManager::MotionManager() : previousAcceleration(0.0) {}

void MotionManager::setup() {
    Wire.begin();
    if (!mma.begin(0x1C, &Wire)) {
        Serial.println("Erreur de connexion au MMA8452X !");
        while (1);
    }
    Serial.println("MMA8452X trouve.");

    // Configure la sensibilité du capteur (ici, +/- 2g)
    mma.setRange(MMA8451_RANGE_2_G);
    
    // Met à jour la première valeur pour éviter un faux positif au démarrage
    sensors_event_t event;
    mma.getEvent(&event);
    previousAcceleration = sqrt(
        (event.acceleration.x * event.acceleration.x) +
        (event.acceleration.y * event.acceleration.y) +
        (event.acceleration.z * event.acceleration.z)
    );
}

bool MotionManager::checkVibration() {
    // Lecture des données de l'accéléromètre
    sensors_event_t event;
    mma.getEvent(&event);

    // Calcule l'accélération totale (magnitude)
    float currentAcceleration = sqrt(
        (event.acceleration.x * event.acceleration.x) +
        (event.acceleration.y * event.acceleration.y) +
        (event.acceleration.z * event.acceleration.z)
    );

    // Calcule la variation par rapport à l'état précédent
    float variation = abs(currentAcceleration - previousAcceleration);

    // Met à jour l'accélération précédente pour la prochaine boucle
    previousAcceleration = currentAcceleration;

    // Détecte un choc si la variation est supérieure à un certain seuil
    // Le seuil peut être ajusté pour plus ou moins de sensibilité.
    if (variation > 0.5) {
        return true;
    }
    
    return false;
}