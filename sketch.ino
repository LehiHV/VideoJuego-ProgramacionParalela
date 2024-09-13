#include <Jostick.h>

Joystick joystick;

void setup() {
  Serial.begin(115200);
  
  // Configurar pines
  pinMode(VERT_PIN, INPUT);
  pinMode(HORZ_PIN, INPUT);
  pinMode(SEL_PIN, INPUT_PULLUP); 
  
}

void loop() {
  // Leer valores del joystick
  int x = joystick.getX();
  int y = joystick.getY();
  bool pressed = joystick.isPressed();

  // Mostrar valores por el puerto serie
  Serial.print("X: ");
  Serial.print(x);
  Serial.print(" | Y: ");
  Serial.print(y);
  Serial.print(" | Pressed: ");
  Serial.println(pressed);

  delay(1000); // Retraso para evitar que los valores se actualicen demasiado rápido
}
