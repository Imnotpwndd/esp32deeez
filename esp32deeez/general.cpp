#include <Arduino.h>
#include "definitions.h"

#ifdef LED
// Function to blink the LED for a given number of times and duration
void blink_led(int num_times, int blink_duration) {
  for (int i = 0; i < num_times; i++) {
    digitalWrite(LED, HIGH);           // Turn LED on
    delay(blink_duration / 2);         // Wait for half of the blink duration
    digitalWrite(LED, LOW);            // Turn LED off
    delay(blink_duration / 2);         // Wait for the other half of the blink duration
  }
}
#endif
