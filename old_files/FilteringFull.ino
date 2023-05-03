
#include <FreqMeasure.h>
#include <filters.h>


const float cutoff_freq   = 0;  //Cutoff frequency in Hz
const float sampling_time = 0.005; //Sampling time in seconds.
IIR::ORDER  order  = IIR::ORDER::OD1; // Order (OD1 to OD4)

 // Low-pass filter
 Filter f(cutoff_freq, sampling_time, order);
  
void setup() {
  Serial.begin(57600); //Start serial monitor at 57600 baud
  FreqMeasure.begin(); //Begin frequency measurement
   pinMode(8, INPUT);
   //pinMode(7, INPUT);
      // Enable pull-ups if necessary
      digitalWrite(8, HIGH);
     // digitalWrite(7, HIGH);
}


double sum=0;
int count=0;

void loop() {
  int value = analogRead(8);
  //int value1 = analogRead(7);
  float filteredval = f.filterIn(value);
 // float filteredval1 = f.filterIn(value);
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
   Serial.print(value, DEC);
   //Serial.print(",");
  // Serial.print(value1, DEC);
      Serial.print(",");
      Serial.println(filteredval, 4);
    //  Serial.print(",");
      //Serial.println(filteredval1, 3);
      delay(5);// Loop time will approx. match the sampling time.
}
