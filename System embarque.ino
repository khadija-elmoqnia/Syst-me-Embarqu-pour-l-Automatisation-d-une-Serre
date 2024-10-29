#include <DHT.h>
#include <Servo.h>

#define DHTPIN 11        // Pin numérique où le DHT22 est connecté
#define DHTTYPE DHT22    // Définir le type de capteur DHT utilisé
#define MOSFET_PIN 9     // Pin de contrôle du MOSFET pour la pompe d'arrosage
#define PHOTO_PIN A0     // Pin pour la photorésistance
#define LED_PIN 2        // Pin pour la LED
#define SERVO_PIN 10     // Pin pour le servo-moteur de ventilation

DHT dht(DHTPIN, DHTTYPE);
Servo myServo;

void setup() {
    Serial.begin(9600);            // Initialiser la communication série
    dht.begin();                   // Initialiser le capteur DHT22
    pinMode(MOSFET_PIN, OUTPUT);   // Configurer la pin du MOSFET comme sortie
    pinMode(LED_PIN, OUTPUT);      // Configurer la pin de la LED comme sortie
    myServo.attach(SERVO_PIN);     // Attacher le servo-moteur à la pin définie
    myServo.write(0);              // Position initiale du servo (fermé)
}

void loop() {
    delay(2000); // Attendre quelques secondes entre les lectures

    float humidity = dht.readHumidity();      // Lire l'humidité relative
    float temperature = dht.readTemperature(); // Lire la température

    if (isnan(humidity) || isnan(temperature)) {
        Serial.println("Échec de la lecture du DHT22 !");
        return;
    }

    Serial.print("Humidité: ");
    Serial.print(humidity);
    Serial.print(" %\t");
    Serial.print("Température: ");
    Serial.print(temperature);
    Serial.println(" °C");

    // Contrôle gradué de la pompe d'arrosage en fonction de l'humidité
    int pumpIntensity;
    if (humidity < 40) {
        pumpIntensity = map(humidity, 0, 40, 255, 0); // Plus faible humidité = intensité plus forte
        pumpIntensity = constrain(pumpIntensity, 0, 255); // Limiter entre 0 et 255
        analogWrite(MOSFET_PIN, pumpIntensity); // Ajuster l'intensité de la pompe
        Serial.print("Pompe d'arrosage activée avec intensité: ");
        Serial.println(pumpIntensity);
    } else {
        analogWrite(MOSFET_PIN, 0); // Éteindre la pompe si humidité suffisante
        Serial.println("Pompe d'arrosage: ÉTEINTE");
    }

    // Contrôle gradué du ventilateur en fonction de la température
    int servoPosition;
    if (temperature > 20) { // Si la température est supérieure à 20°C
        servoPosition = map(temperature, 20, 40, 0, 90); // Ajuste l'angle de 0° à 90° entre 20°C et 40°C
        servoPosition = constrain(servoPosition, 0, 90); // Limite entre 0 et 90 degrés
        myServo.write(servoPosition);
        Serial.print("Ventilateur ouvert à: ");
        Serial.print(servoPosition);
        Serial.println("°");
    } else {
        myServo.write(0);  // Fermer le ventilateur si température est inférieure à 20°C
        Serial.println("Ventilateur: FERMÉ");
    }

    // Lire la valeur de la photorésistance
    int photoValue = analogRead(PHOTO_PIN);
    Serial.print("Valeur de la photorésistance: ");
    Serial.println(photoValue);

    // Contrôler la LED en fonction de la luminosité
    int ledBrightness;
    if (photoValue < 200) {
        ledBrightness = 255; // Allumer la LED (environnement sombre)
        analogWrite(LED_PIN, ledBrightness);
        Serial.println("LED: ALLUMÉE (environnement sombre)");
    } else if (photoValue > 500) {
        ledBrightness = 0; // Éteindre la LED (lumière suffisante)
        analogWrite(LED_PIN, ledBrightness);
        Serial.println("LED: ÉTEINTE (lumière suffisante)");
    } else {
        ledBrightness = map(photoValue, 200, 500, 255, 0); // Ajuster la luminosité
        analogWrite(LED_PIN, ledBrightness);
        Serial.print("LED: INTENSITÉ AJUSTÉE à ");
        Serial.println(ledBrightness);
    }

    delay(100); // Petite pause pour la stabilité
}
