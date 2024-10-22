#ifndef Panel_h
#define Panel_h

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Jostick.h"
#include <vector>
#include "MicroSD.h"

// Definición de notas
#define NOTE_C4  262
#define NOTE_G3  196
#define NOTE_A3  220
#define NOTE_B3  247

// Definición del caracter personalizado
byte caracter2[] = {
  B00100,
  B01110,
  B11111,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100
};

byte caracter1[] = {
  B11110,
  B10101,
  B11111,
  B01110,
  B01111,
  B11110,
  B01110,
  B01010
};

// Logo del juego (carita feliz)
byte logoChar[8] = {
  B00000,
  B10001,
  B00000,
  B00000,
  B10001,
  B01110,
  B00000,
  B00000
};

byte DChar[8] = {
  B11110,
  B10011,
  B11001,
  B11001,
  B11001,
  B11001,
  B10011,
  B01110
};

byte IChar[8] = {
  B11110,
  B01111,
  B01100,
  B01100,
  B01100,
  B01100,
  B01110,
  B11111
};

byte NChar[8] = {
  B10011,
  B11001,
  B11101,
  B11101,
  B10111,
  B10111,
  B10011,
  B11001
};

byte OChar[8] = {
  B11110,
  B01010,
  B10011,
  B10001,
  B10001,
  B10001,
  B01011,
  B01110
};

byte rightSign[8] = {
  B00001,
  B00010,
  B00100,
  B01000,
  B01000,
  B11000,
  B11000,
  B11000
};

byte leftSign[8] = {
  B10000,
  B01000,
  B00100,
  B00010,
  B00010,
  B00011,
  B00011,
  B00011
};

// Melodía de intro (ajusta según tus preferencias)
const int MELODY_SIZE = 8;
const int melody[MELODY_SIZE] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};
const int noteDurations[MELODY_SIZE] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

class Panel {
private:
    LiquidCrystal_I2C lcd;
    Joystick joystick;
    MicroSD microSD;
    int characterX;
    int characterY;
    unsigned long lastMoveTime;
    unsigned long lastDebugTime;
    unsigned long lastNoteTime;
    int currentNote;
    static const int buzzerPin = 27;
    static const int CENTER_VALUE = 2048;
    static const int THRESHOLD = 1000;
    bool isMovingUp;
    int upMovePhase;

    //Menu options
    int currentMenuOption;
    static const int NUM_MENU_OPTIONS = 2;
    const char* menuOptions[NUM_MENU_OPTIONS] = {"Iniciar Juego", "Ver Puntuajes"};
    bool inMainMenu;

    // Variables for obstacle management
    std::vector<int> obstacles;
    unsigned long lastObstacleMove;
    unsigned long lastObstacleSpawn;
    static const unsigned long OBSTACLE_MOVE_INTERVAL = 250; // Move every 0.25 seconds
    static const unsigned long MIN_SPAWN_INTERVAL = 2000; // Minimum 2 seconds between spawns
    static const unsigned long MAX_SPAWN_INTERVAL = 5000; // Maximum 5 seconds between spawns

    // Variables for score and game state
    int score;
    bool gameOver;

    // New variables for pause functionality
    bool isPaused;
    bool lastButtonState;

    void showMenu() {
        lcd.clear();
        for (int i = 0; i < NUM_MENU_OPTIONS; i++) {
            lcd.setCursor(0, i);
            if (i == currentMenuOption) {
                lcd.print("> ");
            } else {
                lcd.print("  ");
            }
            lcd.print(menuOptions[i]);
        }
    }

    void updateMenu() {
        int y = joystick.getY();
        if (y > CENTER_VALUE + THRESHOLD) {
            currentMenuOption = (currentMenuOption + 1) % NUM_MENU_OPTIONS;
            showMenu();
            delay(200);
        } else if (y < CENTER_VALUE - THRESHOLD) {
            currentMenuOption = (currentMenuOption - 1 + NUM_MENU_OPTIONS) % NUM_MENU_OPTIONS;
            showMenu();
            delay(200);
        }

        if (joystick.isPressed()) {
            selectMenuOption();
            delay(200);
        }
    }

    void selectMenuOption() {
        if (currentMenuOption == 0) {
            inMainMenu = false;
            lcd.clear();
            resetGame();
        } else if (currentMenuOption == 1) {
            showHighScores();
        }
    }

    void showHighScores() {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Mejores Puntajes:");
        lcd.setCursor(0, 1);
        lcd.print(microSD.getHighScoresString());
        microSD.readHighScores();  // This will print high scores to Serial
        delay(5000);  // Show for 5 seconds
        showMenu();
    }

    void playIntroMelody() {
        for (int thisNote = 0; thisNote < MELODY_SIZE; thisNote++) {
            int noteDuration = 1000 / noteDurations[thisNote];
            if (melody[thisNote] != 0) {
                tone(buzzerPin, melody[thisNote], noteDuration);
            } else {
                noTone(buzzerPin);
            }
            delay(noteDuration * 1.3);
            noTone(buzzerPin);
        }
    }

    void showLogo() {
        lcd.clear();
        lcd.createChar(0, DChar);
        lcd.createChar(1, IChar);
        lcd.createChar(2, NChar);
        lcd.createChar(3, OChar);
        lcd.createChar(4, rightSign);
        lcd.createChar(5, leftSign);
        lcd.createChar(6,caracter1);
        lcd.createChar(7,caracter2);
        
        lcd.setCursor(5, 0);  // Centrar en la pantalla (ajustar según el tamaño de tu LCD)
        lcd.write(4);
        lcd.write(0);  // 'D'
        lcd.write(1);  // 'I'
        lcd.write(2);  // 'N'
        lcd.write(3);  // 'O'
        lcd.write(5);
        
        delay(2000);
    }

    void showGameName() {
        lcd.clear();
        lcd.setCursor(3, 0);
        lcd.print("MiJuego!");
        delay(2000);
    }

    void showDevelopers() {
        const char* developers[] = {
            "HernandezVelasco Lehi Alejandro",
            "Torres Lozoya Jorge Antonio",
            "Pinto Gutierrez Luis Roberto",
            "Aguirre Carrazco Marco Antonio",
            "Verdin Aguilar Monica Gabriela"
        };
        
        for (int i = 0; i < 5; i++) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Desarrollado por:");
            lcd.setCursor(0, 1);
            lcd.print(developers[i]);
            delay(2000);
        }
    }

    void showIntro() {
        playIntroMelody();
        showLogo();
        showDevelopers();
        lcd.clear();
    }

    void playBackgroundMusic() {
        unsigned long currentTime = millis();
        if (currentTime - lastNoteTime >= 1000 / noteDurations[currentNote]) {
            if (melody[currentNote] != 0) {
                tone(buzzerPin, melody[currentNote], 1000 / noteDurations[currentNote]);
            } else {
                noTone(buzzerPin);
            }
            lastNoteTime = currentTime;
            currentNote = (currentNote + 1) % MELODY_SIZE;  // Esto hace que la melodía se repita infinitamente
        }
    }

    void updateObstacles() {
        unsigned long currentTime = millis();

        // Move obstacles
        if (currentTime - lastObstacleMove >= OBSTACLE_MOVE_INTERVAL) {
            for (int i = 0; i < obstacles.size(); ++i) {
                // Erase previous obstacle position
                lcd.setCursor(obstacles[i], 1);
                lcd.print(" ");

                obstacles[i]--;
                if (obstacles[i] < 0) {
                    obstacles.erase(obstacles.begin() + i);
                    i--;
                    score++; // Increment score when an obstacle disappears
                } else {
                    // Draw new obstacle position
                    lcd.setCursor(obstacles[i], 1);
                    lcd.write(7); // Use caracter2
                }

                // Check for collision
                if (obstacles[i] == characterX) {
                    // Only lose if the character is in the same row as the obstacle (row 1)
                    if (characterY == 1) {
                        gameOver = true;
                        showGameOver();
                        return;
                    }
                    // No collision if the character is above the obstacle (row 0)
                }
            }
            lastObstacleMove = currentTime;
        }

        // Spawn new obstacle
        if (currentTime - lastObstacleSpawn >= random(MIN_SPAWN_INTERVAL, MAX_SPAWN_INTERVAL + 1)) {
            if (obstacles.empty() || obstacles.back() < 15) { // Ensure no consecutive spawns
                obstacles.push_back(15);
                lcd.setCursor(15, 1);
                lcd.write(7); // Draw new obstacle
                lastObstacleSpawn = currentTime;
            }
        }
    }

    void drawCharacter() {
        static int lastCharX = -1;
        static int lastCharY = -1;

        if (characterX != lastCharX || characterY != lastCharY) {
            // Erase previous character position
            if (lastCharX != -1 && lastCharY != -1) {
                lcd.setCursor(lastCharX, lastCharY);
                lcd.print(" ");
            }

            // Draw new character position
            lcd.setCursor(characterX, characterY);
            lcd.write(6);  // Display the custom character (caracter1)

            lastCharX = characterX;
            lastCharY = characterY;
        }
    }

    void showGameOver() {
        lcd.clear();
        lcd.setCursor(3, 0);
        lcd.print("Perdiste :c");
        lcd.setCursor(0, 1);
        lcd.print("Score: ");
        lcd.print(score);
        
        if (microSD.saveHighScore(score)) {
            delay(2000);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Nuevo record!");
            lcd.setCursor(0, 1);
            lcd.print(microSD.getHighScoresString());
        }
        
        delay(3000);
        inMainMenu = true;
        currentMenuOption = 0;
        showMenu();
    }

    void resetGame() {
        score = 0;
        gameOver = false;
        isPaused = false;
        characterX = 1;
        characterY = 1;
        obstacles.clear();
        lcd.clear();
    }

    void showPauseMenu() {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Juego Pausado");
        lcd.setCursor(0, 1);
        lcd.print("Score: ");
        lcd.print(score);
    }

public:
    Panel() : lcd(0x27, 16, 2), characterX(0), characterY(1), lastMoveTime(0), lastDebugTime(0), 
              isMovingUp(false), upMovePhase(0), lastObstacleMove(0), lastObstacleSpawn(0),
              score(0), gameOver(false), isPaused(false), lastButtonState(false) {}

    void init() {
        lcd.init();
        lcd.backlight();
        lcd.write(6);
        lcd.write(7);
        pinMode(SEL_PIN, INPUT_PULLUP);
        pinMode(buzzerPin, OUTPUT);
        showIntro();
        currentMenuOption = 0;
        inMainMenu = true;
        showMenu();
        characterX = 1;
        characterY = 1; 
        Serial.begin(115200);
        randomSeed(analogRead(0));
        microSD.init();  // Initialize MicroSD
    }

    void updateCharacterPosition() {
        int y = joystick.getY();
        unsigned long currentTime = millis();

        // Depuración
        if (currentTime - lastDebugTime > 200) {
            lastDebugTime = currentTime;
        }

        // Comprobamos si ha pasado suficiente tiempo desde el último movimiento
        if (currentTime - lastMoveTime > 200) {
            bool moved = false;

            if (isMovingUp) {
                // Secuencia de movimiento especial hacia arriba
                switch (upMovePhase) {
                    case 0:
                        characterY = 0;
                        upMovePhase++;
                        break;
                    case 1:
                        upMovePhase++;
                        break;
                    case 2:
                        characterY = 1;
                        upMovePhase = 0;
                        isMovingUp = false;
                        break;
                }
                moved = true;
            } else {
                // Solo permitimos el movimiento hacia arriba
                if (y > CENTER_VALUE + THRESHOLD && characterY > 0) {
                    isMovingUp = true;
                    moved = true;
                }
            }

            if (moved) {
                lastMoveTime = currentTime;
            }
        }
    }

    void update() {
        if (inMainMenu) {
            updateMenu();
        } else {
            // Leer el estado actual del botón
            bool currentButtonState = joystick.isPressed();

            if (currentButtonState && !lastButtonState) {
                isPaused = !isPaused;  // Alternar estado de pausa
                if (isPaused) {
                    showPauseMenu();  // Mostrar menú de pausa
                } else {
                    lcd.clear();  // Limpiar la pantalla al reanudar
                    drawCharacter();  // Mostrar el personaje nuevamente
                }
                delay(200);
            }

            lastButtonState = currentButtonState;

            if (!gameOver && !isPaused) {
                updateCharacterPosition();
                drawCharacter();
                updateObstacles();
                playBackgroundMusic();
            } else if (gameOver) {
                showGameOver();  // Si el juego terminó, mostrar Game Over y regresar al menú
            }
        }
    }
};

#endif
