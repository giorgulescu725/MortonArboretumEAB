// All Drivers which are needed to run code

#include "arduino.h"
#include "driver/pcnt.h"                                                
#include "soc/pcnt_struct.h"
#include "WiFi.h"
#include <HTTPClient.h>
#include "time.h"
//#include <filters.h>

// ------------------------------------------------------------

const char* ntpServer = "pool.ntp.org"; // sets site to get time from
const long  gmtOffset_sec = -21600; // time offset to get CDT
const int   daylightOffset_sec = 0; // daylight savings time offset
// WiFi credentials
const char* ssid = "12mini iPhone";         // change SSID
const char* password = "cH124680";    // change password
// Google script ID and required credentials
String GOOGLE_SCRIPT_ID = "AKfycbzPwv9zahTks-2ReYFqFVcUsuQGO3X-jeGpCAygZBFA-iJzwRxUu4xXPPUcX6EPtEmS";    // change Gscript ID
int count = 0;

/* ------------------------------------------------------------
This section will contain explains for the entirety of the code

How the PCNT works:

 The PCNT (Pulse Counter) module used through PWM pins is designed to count the number of rising and/or falling edges of input signals. ​

 The PCNT code is repeated exactly 3 additional times for each sensor


*/ ------------------------------------------------------------

/*

 Defines the pins to the GPIO of the ESP32
 
*/
#define PCNT_H_LIM_VAL overflow1
#define PCNT_H_LIM_VAL overflow2
#define PCNT_H_LIM_VAL overflow3
#define PCNT_H_LIM_VAL overflow4


#define FREQ_PIN1 34
#define FREQ_PIN2 35
#define FREQ_PIN3 32
#define FREQ_PIN4 33


// ------------------------------------------------------------

/*
Functions are called to establish the PCNT​

Overflow​

Pulse​

Frequency​
*/

bool            flag1          = true;                                   
uint32_t        overflow1      = 20000;                                  
int16_t         pulses1        = 0;                                  
uint32_t        overflow_cnt1  = 0;                                        
volatile double frequency1     = 0;
uint16_t result1 = 0;

bool            flag2          = true;                                   
uint32_t        overflow2      = 20000;                                  
int16_t         pulses2        = 0;                                  
uint32_t        overflow_cnt2  = 0;                                        
volatile double frequency2     = 0;
uint16_t result2 = 0;

bool            flag3          = true;                                   
uint32_t        overflow3      = 20000;                                  
int16_t         pulses3        = 0;                                  
uint32_t        overflow_cnt3  = 0;                                        
volatile double frequency3     = 0;
uint16_t result3 = 0;

bool            flag4          = true;                                   
uint32_t        overflow4      = 20000;                                  
int16_t         pulses4        = 0;                                  
uint32_t        overflow_cnt4  = 0;                                        
volatile double frequency4    = 0;
uint16_t result4 = 0;

volatile double average = 0;

// ------------------------------------------------------------

/*
 
 Timers are created to track cycles​
 
*/
 
esp_timer_create_args_t timer_args1;                                   
esp_timer_handle_t timer_handle1;                                       
portMUX_TYPE timer_mux1 = portMUX_INITIALIZER_UNLOCKED;

esp_timer_create_args_t timer_args2;                                    
esp_timer_handle_t timer_handle2;                                          
portMUX_TYPE timer_mux2 = portMUX_INITIALIZER_UNLOCKED;

esp_timer_create_args_t timer_args3;                                    
esp_timer_handle_t timer_handle3;                                          
portMUX_TYPE timer_mux3 = portMUX_INITIALIZER_UNLOCKED;

esp_timer_create_args_t timer_args4;                                    
esp_timer_handle_t timer_handle4;                                          
portMUX_TYPE timer_mux4 = portMUX_INITIALIZER_UNLOCKED;


// ------------------------------------------------------------

/*
 
Sets the pulse input GPIO pin for the pulse counter to FREQ_PIN1.

Sets the control GPIO pin to -1, indicating that no control GPIO is used.

Sets the low and high control modes to PCNT_MODE_KEEP, indicating that the control mode will not be used.

Sets the positive and negative counting modes to PCNT_COUNT_INC, indicating that the pulse counter will count up on both edges of the pulse.

Sets the upper counter limit to 20000.

Sets the lower counter limit to 0.

Sets the pulse counter unit to PCNT_UNIT_0.

Sets the pulse counter channel to PCNT_CHANNEL_0.
​
*/
 
pcnt_config_t pcnt_config1 = {
  .pulse_gpio_num    = FREQ_PIN1,
  .ctrl_gpio_num     = -1,
  .lctrl_mode        = PCNT_MODE_KEEP,
  .hctrl_mode        = PCNT_MODE_KEEP,
  .pos_mode          = PCNT_COUNT_INC,
  .neg_mode          = PCNT_COUNT_INC,
  .counter_h_lim     = 20000,
  .counter_l_lim     = 0,
  .unit              = PCNT_UNIT_0, 
  .channel           = PCNT_CHANNEL_0
  };

  pcnt_config_t pcnt_config2 = 
  {
  .pulse_gpio_num    = FREQ_PIN2,
  .ctrl_gpio_num     = -1,
  .lctrl_mode        = PCNT_MODE_KEEP,
  .hctrl_mode        = PCNT_MODE_KEEP,
  .pos_mode          = PCNT_COUNT_INC,
  .neg_mode          = PCNT_COUNT_INC,
  .counter_h_lim     = 20000,
  .counter_l_lim     = 0,
  .unit              = PCNT_UNIT_1, 
  .channel           = PCNT_CHANNEL_0
  };

  pcnt_config_t pcnt_config3 = 
  {
  .pulse_gpio_num    = FREQ_PIN3,
  .ctrl_gpio_num     = -1,
  .lctrl_mode        = PCNT_MODE_KEEP,
  .hctrl_mode        = PCNT_MODE_KEEP,
  .pos_mode          = PCNT_COUNT_INC,
  .neg_mode          = PCNT_COUNT_INC,
  .counter_h_lim     = 20000,
  .counter_l_lim     = 0,
  .unit              = PCNT_UNIT_2, 
  .channel           = PCNT_CHANNEL_0
  };

  pcnt_config_t pcnt_config4 = 
  {
  .pulse_gpio_num    = FREQ_PIN4,
  .ctrl_gpio_num     = -1,
  .lctrl_mode        = PCNT_MODE_KEEP,
  .hctrl_mode        = PCNT_MODE_KEEP,
  .pos_mode          = PCNT_COUNT_INC,
  .neg_mode          = PCNT_COUNT_INC,
  .counter_h_lim     = 20000,
  .counter_l_lim     = 0,
  .unit              = PCNT_UNIT_3, 
  .channel           = PCNT_CHANNEL_0
  };


// ------------------------------------------------------------


/*

PCNT is configured to the corresponding GPIO pin​

​

The code creates a one-shot timer using an ESP timer API (esp_timer_create) with a callback function (pcnt_get_counter1) that reads the current value of the counter. ​

​

The timer handle is stored in timer_handle1.​

 
*/

void IRAM_ATTR pcnt_event_handler1(void *arg)
{
  portENTER_CRITICAL_ISR(&timer_mux1);
  overflow_cnt1++; 
  PCNT.int_clr.val = BIT(PCNT_UNIT_0);
  portEXIT_CRITICAL_ISR(&timer_mux1);
}     

void IRAM_ATTR pcnt_event_handler2(void *arg) 
{
  portENTER_CRITICAL_ISR(&timer_mux2); 
  overflow_cnt2++;  
  PCNT.int_clr.val = BIT(PCNT_UNIT_1); 
  portEXIT_CRITICAL_ISR(&timer_mux2); 
}  

void IRAM_ATTR pcnt_event_handler3(void *arg)
{
  portENTER_CRITICAL_ISR(&timer_mux3); 
  overflow_cnt3++; 
  PCNT.int_clr.val = BIT(PCNT_UNIT_2); 
  portEXIT_CRITICAL_ISR(&timer_mux3);
}

void IRAM_ATTR pcnt_event_handler4(void *arg)
{
  portENTER_CRITICAL_ISR(&timer_mux4); 
  overflow_cnt4++; 
  PCNT.int_clr.val = BIT(PCNT_UNIT_3); 
  portEXIT_CRITICAL_ISR(&timer_mux4);
}

// ------------------------------------------------------------ 

/*

Establishes PWM Channels for the Traffic light to give it a voltage when conditions are met
 
*/

int pwmChannel0 = 0; // Selects channel 0
int pwmChannel1 = 1; // Selects channel 1
int pwmChannel2 = 2; // Selects channel 2
int frequence = 1000; // PWM frequency of 1 KHz
int resolution = 8; // 8-bit resolution, 256 possible values

int pwmPin0 = 23; //Will be assigned to Red Light
int pwmPin1 = 22; //Will be assigned to Yellow Light
int pwmPin2 = 21; //Will be assigned to Green Light

//Code for filtering (Not necesssary until the range of EAB Larvae noise is known)
 
//
//const float cutoff_freq = 0; //Cutoff frequency in Hz
//const float sampling_time = 0.001; //Sampling time in seconds.
//IIR::ORDER order = IIR::ORDER::OD4; // Order (OD1 to OD4)
 
// Low-pass filter
// Filter f(cutoff_freq, sampling_time, order);

void setup()
{
  Serial.begin(115200);

   delay(1000);
  // connect to WiFi
  Serial.println();
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  Serial.flush();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  pcnt_init1();
  pcnt_init2();
  pcnt_init3();
  pcnt_init4();

 pinMode(pwmPin0, OUTPUT);
 pinMode(pwmPin1, OUTPUT);
 pinMode(pwmPin2, OUTPUT);
}


/*
 
Sets the pin mode of FREQ_PIN1 to INPUT using pinMode() function.

Configures the PCNT (Pulse Counter) unit 0 with the settings specified in pcnt_config1 using the pcnt_unit_config() function.

Registers the interrupt service routine (ISR) pcnt_event_handler1() to be called when the pulse counter reaches its high limit using the pcnt_isr_register() function.

Sets the filter value of the pulse counter to 1000 using the pcnt_set_filter_value() function.

Enables the filter on the pulse counter using the pcnt_filter_enable() function.

Pauses the pulse counter using the pcnt_counter_pause() function.

Clears the counter value of the pulse counter using the pcnt_counter_clear() function.

Enables the pulse counter to trigger the interrupt when the counter reaches the high limit using the pcnt_event_enable() function.

Resumes the pulse counter using the pcnt_counter_resume() function.

Initializes a timer timer_args1 with the callback function pcnt_get_counter1() and a null argument using the esp_timer_create() function. The timer is named "one shot timer".

Checks if the timer was created successfully. If not, logs an error message to the console using ESP_LOGE().

Initializes the same timer timer_args1 with the same callback function pcnt_get_counter1() using esp_timer_create() function and saves the handle to the timer in timer_handle1.


 
*/

void pcnt_init1(void)                                                     
{  
  pinMode(FREQ_PIN1, INPUT);

  pcnt_unit_config(&pcnt_config1);
  pcnt_isr_register(pcnt_event_handler1, NULL, 0, NULL);                   
  pcnt_set_filter_value(PCNT_UNIT_0, 1000);
  pcnt_filter_enable(PCNT_UNIT_0); 
  pcnt_counter_pause(PCNT_UNIT_0);                                       
  pcnt_counter_clear(PCNT_UNIT_0);                                       
  pcnt_event_enable(PCNT_UNIT_0, PCNT_EVT_H_LIM);                        
  pcnt_counter_resume(PCNT_UNIT_0);                                       

  timer_args1.callback = pcnt_get_counter1;
  timer_args1.arg      = NULL;
  timer_args1.name     = "one shot timer";

  if(esp_timer_create(&timer_args1, &timer_handle1) != ESP_OK) 
  {
    ESP_LOGE(TAG, "timer create");
  }

  timer_args1.callback = pcnt_get_counter1; 
  esp_timer_create(&timer_args1, &timer_handle1);                           
}

void pcnt_init2(void)                                                     
{  
  pinMode(FREQ_PIN2, INPUT);

  pcnt_unit_config(&pcnt_config2);
  pcnt_isr_register(pcnt_event_handler2, NULL, 0, NULL);               
  pcnt_intr_enable(PCNT_UNIT_1);  
  pcnt_set_filter_value(PCNT_UNIT_1, 1000);
  pcnt_filter_enable(PCNT_UNIT_1); 
  pcnt_counter_pause(PCNT_UNIT_1);                                      
  pcnt_counter_clear(PCNT_UNIT_1);                                      
  pcnt_event_enable(PCNT_UNIT_1, PCNT_EVT_H_LIM);                        
  pcnt_counter_resume(PCNT_UNIT_1);                                    

  timer_args2.callback = pcnt_get_counter2;
  timer_args2.arg      = NULL;
  timer_args2.name     = "one shot timer";

  if(esp_timer_create(&timer_args2, &timer_handle2) != ESP_OK) 
  {
    ESP_LOGE(TAG, "timer create");
  }

  timer_args2.callback = pcnt_get_counter2;
  esp_timer_create(&timer_args2, &timer_handle2); 
}

void pcnt_init3(void)                                                     
{  
  pinMode(FREQ_PIN3, INPUT);

  pcnt_unit_config(&pcnt_config3);
  pcnt_isr_register(pcnt_event_handler3, NULL, 0, NULL);
  pcnt_intr_enable(PCNT_UNIT_2);  
  pcnt_set_filter_value(PCNT_UNIT_2, 1000);
  pcnt_filter_enable(PCNT_UNIT_2); 
  pcnt_counter_pause(PCNT_UNIT_2);                                       
  pcnt_counter_clear(PCNT_UNIT_2);                                        
  pcnt_event_enable(PCNT_UNIT_2, PCNT_EVT_H_LIM);                        
  pcnt_counter_resume(PCNT_UNIT_2);                                       

  timer_args3.callback = pcnt_get_counter3;
  timer_args3.arg      = NULL;
  timer_args3.name     = "one shot timer";

  if(esp_timer_create(&timer_args3, &timer_handle3) != ESP_OK) 
  {
    ESP_LOGE(TAG, "timer create");
  }

  timer_args3.callback = pcnt_get_counter3;
  esp_timer_create(&timer_args3, &timer_handle3); 
}

void pcnt_init4(void)                                                     
{  
  pinMode(FREQ_PIN4, INPUT);

  pcnt_unit_config(&pcnt_config4);
  pcnt_isr_register(pcnt_event_handler4, NULL, 0, NULL);
  pcnt_intr_enable(PCNT_UNIT_3);  
  pcnt_set_filter_value(PCNT_UNIT_3, 1000);
  pcnt_filter_enable(PCNT_UNIT_3); 
  pcnt_counter_pause(PCNT_UNIT_3);                                       
  pcnt_counter_clear(PCNT_UNIT_3);                                        
  pcnt_event_enable(PCNT_UNIT_3, PCNT_EVT_H_LIM);                        
  pcnt_counter_resume(PCNT_UNIT_3);                                       

  timer_args4.callback = pcnt_get_counter4;
  timer_args4.arg      = NULL;
  timer_args4.name     = "one shot timer";

  if(esp_timer_create(&timer_args4, &timer_handle4) != ESP_OK) 
  {
    ESP_LOGE(TAG, "timer create");
  }

  timer_args4.callback = pcnt_get_counter4;
  esp_timer_create(&timer_args4, &timer_handle4); 
}

// ------------------------------------------------------------


/*
 
 Stores counter value as a pointer​

 Grabbing the values from the PCNT and cross referencing​

 
 */
 
void pcnt_get_counter1(void *p) 
{ 
  pcnt_counter_pause(PCNT_UNIT_0);
  pcnt_get_counter_value(PCNT_UNIT_0, (int16_t*) &result1); 
  flag1 = true;
}

void pcnt_get_counter2(void *p) 
{ 
  pcnt_counter_pause(PCNT_UNIT_1);
  pcnt_get_counter_value(PCNT_UNIT_1, (int16_t*) &result2); 
  flag2 = true;
}

void pcnt_get_counter3(void *p) 
{ 
  pcnt_counter_pause(PCNT_UNIT_2);
  pcnt_get_counter_value(PCNT_UNIT_2, (int16_t*) &result3); 
  flag3 = true;
}

void pcnt_get_counter4(void *p) 
{ 
  pcnt_counter_pause(PCNT_UNIT_3);
  pcnt_get_counter_value(PCNT_UNIT_3, (int16_t*) &result4); 
  flag4 = true;
}


/*
 
 The code called in the loop checks the values of the Pulse counter (PCNT) and then proceeds to divide it by two​

​
 It clears the pulse counter and resets overflow to prevent any interference in the next rising edge​
 */


void loop() 
{
  if (flag1 == true)
  {
    flag1 = false;
    frequency1 =  (result1 + (overflow_cnt1*20000)) / 2;
    //double freq1 = frequency_fan1 / 2; 
    overflow_cnt1 = 0; 
    pcnt_counter_clear(PCNT_UNIT_0); 
    pcnt_counter_resume(PCNT_UNIT_0); 
    overflow_cnt1 = 0;    
  
    Serial.print(frequency1); //* 15);
    Serial.println(" FREQ PIN 34");

  // float filteredval = f.filterIn(value);
  // 
  // Serial.println(filteredval, 4);
    pcnt_counter_clear(PCNT_UNIT_0);
    esp_timer_start_once(timer_handle1, 1000000);




 

  
  if (flag2 == true)
  {
    flag2 = false;
    frequency2 =  (result2 + (overflow_cnt2 * 20000)) / 2; 
    //double freq2 = frequency_fan2 / 2; 
    overflow_cnt2 = 0; 
    pcnt_counter_clear(PCNT_UNIT_1); 
    pcnt_counter_resume(PCNT_UNIT_1); 
    overflow_cnt2 = 0;    
  
    Serial.print(frequency2);
    Serial.println(" FREQ PIN 35");

    pcnt_counter_clear(PCNT_UNIT_1);
    esp_timer_start_once(timer_handle2, 1000000);
  }

    /* 
     * Check the sensor value and set the traffic light accordingly
     * Values can be adjusted according to filter or EAB Noise range
    */
    
   if (1200 <= frequency2 && frequency2 <= 1800)
   {
   digitalWrite(21, HIGH);
   digitalWrite(22, LOW);
   digitalWrite(23, LOW);
   Serial.println("green");
   Serial.print(frequency2);
   }
   if ((1000 <= frequency2 && frequency2 <= 1199) || (1801 <= frequency2 && frequency2 <= 2000))
   {
   digitalWrite(21, LOW);
   digitalWrite(22, HIGH);
   digitalWrite(23, LOW);
   Serial.println("yellow");
   Serial.print(frequency2);
   }
   if ((0 <= frequency2 && frequency2 <= 999) || 2001 <= frequency2)
   {
   digitalWrite(21, LOW);
   digitalWrite(22, LOW);
   digitalWrite(23, HIGH);
   Serial.println("red");
   Serial.print(frequency2);
   }
  }

  if (flag3 == true)
  {
    flag3 = false;
    frequency3 =  (result3 + (overflow_cnt3 * 20000)) / 2;
    //double freq3 = frequency3 / 2; 
    overflow_cnt3 = 0; 
    pcnt_counter_clear(PCNT_UNIT_2); 
    pcnt_counter_resume(PCNT_UNIT_2); 
    overflow_cnt3 = 0;    
  
    Serial.print(frequency3);
    Serial.println(" FREQ PIN 32");

    pcnt_counter_clear(PCNT_UNIT_2);
    esp_timer_start_once(timer_handle3, 1000000);
  }

  if (flag4 == true)
  {
    flag4 = false;
    frequency4 =  (result4 + (overflow_cnt4 * 20000)) / 2;
    //double freq4 = frequency_fan4 / 2; 
    overflow_cnt4 = 0; 
    pcnt_counter_clear(PCNT_UNIT_3); 
    pcnt_counter_resume(PCNT_UNIT_3); 
    overflow_cnt4 = 0;    
  
    Serial.print(frequency4);
    Serial.println(" FREQ PIN 33");

    pcnt_counter_clear(PCNT_UNIT_3);
    esp_timer_start_once(timer_handle4, 1000000);
  }

  
 /*
  
  Checks if WiFi is connected using WiFi.status() == WL_CONNECTED.
  
  If WiFi is connected, a boolean variable flag is initialized to false.
  
  The code then attempts to get the current local time using getLocalTime(&timeinfo). If this function call fails, the program prints an error message and returns.
  
  A character buffer timeStringBuff of size 50 is created to hold the formatted time string.
  
  The strftime() function is used to format the time information obtained in step 3. The formatted time string is stored in timeStringBuff, and the hour, minute, and second information is stored in separate character buffers hourC, minuteC, and secondC, respectively.
  
  The character buffers are converted to String objects using their respective constructors.
  
  The placeholder string is initialized to "holding_place".
  
  A URL string urlFinal is constructed by concatenating various pieces of data, including the hour, minute, second, and frequency information, into a single string.
  
  The constructed URL is printed to the serial monitor using Serial.println().
  
  An HTTPClient object http is created.

  The begin() method of http is called with urlFinal.c_str() as the argument to initiate the HTTP request.
  
  The setFollowRedirects() method of http is called with HTTPC_STRICT_FOLLOW_REDIRECTS as the argument to set the redirect policy.
  
  The GET() method of http is called to send the HTTP GET request to the server.
  
  The HTTP status code returned by the server is stored in httpCode.
  
  The HTTP status code is printed to the serial monitor using Serial.println().
  
  */
  
  
     if (WiFi.status() == WL_CONNECTED) {
    static bool flag = false;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time");
      return;
    }
    char timeStringBuff[50]; //50 chars should be enough
    strftime(timeStringBuff, sizeof(timeStringBuff), "%X", &timeinfo);
    char hourC[10];
    strftime(hourC, sizeof(hourC), "%H", &timeinfo);
    char minuteC[10];
    strftime(minuteC, sizeof(minuteC), "%M", &timeinfo);
    char secondC[10];
    strftime(secondC, sizeof(secondC), "%S", &timeinfo);
    String asString(timeStringBuff);
    String hour(hourC);
    String minute(minuteC);
    String second(secondC);
    //asString.replace(" ", "-");
    //Serial.print("Time:");
    Serial.println(asString);
    String placeholder("holding_place");
    String urlFinal = "https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?"+"Hour=" + hour + "&Minute=" + minute + "&Second=" + second + "&FREQ1=" + (int)frequency1 + "&FREQ2=" + (int)frequency2 + "&FREQ3=" + (int)frequency3 + "&FREQ4=" + (int)frequency4;
    Serial.print("POST data to spreadsheet:");
    Serial.println(urlFinal);
    HTTPClient http;
    http.begin(urlFinal.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET(); 
    Serial.print("HTTP Status Code: ");
    Serial.println(httpCode);
    //---------------------------------------------------------------------
    //getting response from google sheet
    String payload;
    if (httpCode > 0) {
        payload = http.getString();
        Serial.println("Payload: "+payload);    
    }
    //---------------------------------------------------------------------
    http.end();
  }
  count++;
  
  delay(10000);
}
