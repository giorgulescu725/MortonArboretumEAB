/* FreqMeasure - Example with serial output
 * http://www.pjrc.com/teensy/td_libs_FreqMeasure.html


  *https://github.com/PaulStoffregen/FreqMeasure 
 
 *
 * This example code is in the public domain.
 */
#include <FreqMeasure.h>

void setup() {
  Serial.begin(57600); //Start serial monitor at 57600 baud
  FreqMeasure.begin(); //Begin frequency measurement
}

double sum=0;
int count=0;

void loop() {
  if (FreqMeasure.available()) { //Returns the number of measurements available to read, or 0 (false) if none are unread.
    // average several reading together
    sum = sum + FreqMeasure.read(); //Read a measurement. An unsigned long (32 bits) containing the number of timer cycles that elapsed during one cycle of the waveform. Each measurement begins immediately after the prior one without any delay, so several measurements may be averaged together for better resolution
    count = count + 1;
    if (count > 30) {
      float frequency = FreqMeasure.countToFrequency(sum / count); //Convert the 32 bit unsigned long numbers from read() to actual frequency
      Serial.println(frequency);
      sum = 0;
      count = 0;
    }
  }
}