#include <Panel.h>

Panel panel;

void setup() {
  Serial.begin(115200);
  panel.init();
}

void loop() {
  panel.update();
}
