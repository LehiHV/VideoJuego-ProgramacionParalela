#ifndef MicroSD_h
#define MicroSD_h

#include <SPI.h>
#include <SD.h>
#include <ArduinoJson.h>

#define MICROSD_PIN 5
#define MAX_HIGH_SCORES 3

class MicroSD {
public:
    String filename = "/highscores.json";
    
    void init();
    bool saveHighScore(int score);
    void readHighScores();
    bool isHighScore(int score);
    String getHighScoresString();

private:
    StaticJsonDocument<200> doc;
    
    void updateHighScores(int newScore);
    void writeHighScoresToFile();
    void readHighScoresFromFile();
};

void MicroSD::init() {
    while (!SD.begin(MICROSD_PIN)) {
        Serial.println(F("Falló la inicialización del módulo MicroSD"));
        delay(1000);
    }
    Serial.println(F("La librería MicroSD ha inicializado con éxito"));
    readHighScoresFromFile();
}

bool MicroSD::saveHighScore(int score) {
    if (isHighScore(score)) {
        updateHighScores(score);
        writeHighScoresToFile();
        return true;
    }
    return false;
}

void MicroSD::readHighScores() {
    Serial.println(F("Puntajes más altos:"));
    for (int i = 0; i < MAX_HIGH_SCORES; i++) {
        Serial.print(i + 1);
        Serial.print(". ");
        Serial.println(doc["scores"][i].as<int>());
    }
}

bool MicroSD::isHighScore(int score) {
    return score > doc["scores"][MAX_HIGH_SCORES - 1].as<int>();
}

String MicroSD::getHighScoresString() {
    String result = "";
    for (int i = 0; i < MAX_HIGH_SCORES; i++) {
        result += String(doc["scores"][i].as<int>());
        if (i < MAX_HIGH_SCORES - 1) {
            result += ",";
        }
    }
    return result;
}

void MicroSD::updateHighScores(int newScore) {
    for (int i = 0; i < MAX_HIGH_SCORES; i++) {
        if (newScore > doc["scores"][i].as<int>()) {
            for (int j = MAX_HIGH_SCORES - 1; j > i; j--) {
                doc["scores"][j] = doc["scores"][j - 1].as<int>();
            }
            doc["scores"][i] = newScore;
            break;
        }
    }
}

void MicroSD::writeHighScoresToFile() {
    File file = SD.open(filename, FILE_WRITE);
    if (file) {
        serializeJson(doc, file);
        file.close();
    } else {
        Serial.print(F("Error opening "));
        Serial.println(filename);
    }
}

void MicroSD::readHighScoresFromFile() {
    File file = SD.open(filename);
    if (file) {
        DeserializationError error = deserializeJson(doc, file);
        if (error) {
            Serial.println(F("Failed to parse JSON, initializing with default values"));
            doc["scores"][0] = 0;
            doc["scores"][1] = 0;
            doc["scores"][2] = 0;
        }
        file.close();
    } else {
        Serial.print(F("Error opening "));
        Serial.println(filename);
        doc["scores"][0] = 0;
        doc["scores"][1] = 0;
        doc["scores"][2] = 0;
    }
}

#endif
