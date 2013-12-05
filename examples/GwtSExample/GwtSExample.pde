#include <GwtS.h>

//#define DEBUG

GWTS ears;

void setup() {
#ifdef DEBUG
	Serial.begin(57600);
#endif
}

void loop() {

#ifdef DEBUG
  Serial.println("Setting white...");
#endif
  ears.set_colors(255,255,255,255,255,255);
  delay(1000);

#ifdef DEBUG
  Serial.println("Setting red...");
#endif
  ears.set_colors(255,0,0,255,0,0);
  delay(1000);

#ifdef DEBUG
  Serial.println("Setting green...");
#endif
  ears.set_colors(0,255,0,0,255,0);
  delay(1000);

#ifdef DEBUG
  Serial.println("Setting blue...");
#endif
  ears.set_colors(0,0,255,0,0,255);
  delay(1000);

#ifdef DEBUG
  Serial.println("Setting r/g");
#endif
  ears.set_colors(255,0,0,0,255,0);
  delay(1000);

#ifdef DEBUG
  Serial.println("Setting g/b");
#endif
  ears.set_colors(0,255,0,0,0,255);
  delay(1000);

#ifdef DEBUG
  Serial.println("Setting b/r");
#endif
  ears.set_colors(0,0,255,255,0,0);
  delay(1000);
}
