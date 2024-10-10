#define VERT_PIN 36  // SVP, mapeado a GPIO36 cafe 
#define HORZ_PIN 39  // SVN, mapeado a GPIO39 amarillo
#define SEL_PIN  32  // Ya está 

#define VALOR_INICIAL 512
#define VALOR_TOPE 1023
#define PWM_MIN 0
#define PWM_MAX 255

#define INCREMENTO( a ) a > VALOR_INICIAL ? map( a, VALOR_INICIAL, VALOR_TOPE, PWM_MIN, PWM_MAX ) : 0
#define DECREMENTO( a ) a < VALOR_INICIAL ? map( a, VALOR_INICIAL, VALOR_TOPE, PWM_MAX, PWM_MIN ) : 0

class Joystick {
  public:
    int getX() {
      return analogRead(HORZ_PIN);
    }

    int getY() {
      return analogRead(VERT_PIN);
    }

    bool isPressed() {
        return digitalRead(SEL_PIN) == LOW; // Asumiendo que el botón se conecta a GND
    }
};
