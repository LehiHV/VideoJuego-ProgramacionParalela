#define VERT_PIN A0
#define HORZ_PIN A3
#define SEL_PIN  34

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
      return !digitalRead(SEL_PIN); // El botón es activo bajo, por eso negamos el valor
    }
};
