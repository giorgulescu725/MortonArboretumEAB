const int OUT_PIN = A0;
 const int SAMPLE_TIME = 10;
 unsigned long millisCurrent;
 unsigned long millisLast = 0;
 unsigned long millisElapsed = 0;
 
 int sampleBufferValue = 0;
 
 void setup() {
   Serial.begin(9600);
 }
 
 void loop() {
 
   millisCurrent = millis();
   millisElapsed = millisCurrent - millisLast;
 
   if (digitalRead(OUT_PIN) == LOW) {
     sampleBufferValue++;
   }
 
   if (millisElapsed > SAMPLE_TIME) {
     Serial.println(sampleBufferValue);
     sampleBufferValue = 0;
     millisLast = millisCurrent;
   }
 
 }
