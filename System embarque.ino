#include <DHT.h>
#include <Servo.h>

#define DHTPIN 11        // Pin numérique où le DHT22 est connecté
#define DHTTYPE DHT22    // Définir le type de capteur DHT utilisé
#define MOSFET_PIN 9    // Pin de contrôle du MOSFET pour la pompe d'arrosage
#define PHOTO_PIN A0     // Pin pour la photorésistance
#define LED_PIN 2        // Pin pour la LED
#define SERVO_PIN 10      // Pin pour le servo-moteur de ventilation

DHT dht(DHTPIN, DHTTYPE);
Servo myServo;

void setup() {
    Serial.begin(9600);            // Initialiser la communication série
    dht.begin();                   // Initialiser le capteur DHT22
    pinMode(MOSFET_PIN, OUTPUT);  // Configurer la pin du MOSFET comme sortie
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

    // Contrôler le moteur DC pour l'arrosage basé sur l'humidité du sol
    if (humidity < 40) { // Si l'humidité est inférieure à 40%
        analogWrite(MOSFET_PIN, 255); // Activer la pompe d'arrosage
        Serial.println("Pompe d'arrosage: ACTIVÉE");
    } else {
        analogWrite(MOSFET_PIN, 0); // Éteindre la pompe
        Serial.println("Pompe d'arrosage: ÉTEINTE");
    }

    // Contrôler la ventilation en fonction de la température
    int servoPosition; // Variable pour stocker la position du servo
    if (temperature > 25) { // Si la température est supérieure à 25°C
        servoPosition = 90; // Ouvrir le ventilateur (position 90°)
        myServo.write(servoPosition);
        Serial.println("Ventilateur: OUVERT");
    } else {
        servoPosition = 0;  // Fermer le ventilateur (position 0°)
        myServo.write(servoPosition);
        Serial.println("Ventilateur: FERMÉ");
    }

    // Lire la valeur de la photorésistance
    int photoValue = analogRead(PHOTO_PIN);
    Serial.print("Valeur de la photorésistance: ");
    Serial.println(photoValue);

    // Contrôler la LED en fonction de la luminosité
    int ledBrightness; // Variable pour stocker la luminosité de la LED
    if (photoValue < 200) {
        ledBrightness = 255; // Allumer la LED (environnement sombre)
        analogWrite(LED_PIN, ledBrightness);
        Serial.println("LED: ALLUMÉE (environnement sombre)");
    } else if (photoValue > 500) {
        ledBrightness = 0; // Éteindre la LED (lumière suffisante)
        analogWrite(LED_PIN, ledBrightness);
        Serial.println("LED: ÉTEINTE (lumière suffisante)");
    } else {
        // Ajuster la luminosité de la LED en fonction de la photorésistance
        ledBrightness = map(photoValue, 200, 500, 255, 0); // Ajuster la luminosité
        analogWrite(LED_PIN, ledBrightness); // Écrire la valeur PWM à la LED
        Serial.print("LED: INTENSITÉ AJUSTÉE à ");
        Serial.println(ledBrightness);
    }

    // Afficher l'état du servo, du moteur DC et de l'intensité de la LED
    Serial.print("Position du servo: ");
    Serial.print(servoPosition);
    Serial.print("°\tIntensité de la LED: ");
    Serial.println(ledBrightness);

    delay(100); // Petite pause pour la stabilité
}
