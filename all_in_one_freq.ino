#include "arduino.h"
#include "driver/pcnt.h"                                                
#include "soc/pcnt_struct.h"
#include "WiFi.h"
#include <HTTPClient.h>
#include "time.h"
//#include <filters.h>

// ------------------------------------------------------------

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -21600;
const int   daylightOffset_sec = 0;
// WiFi credentials
const char* ssid = "12mini iPhone";         // change SSID
const char* password = "cH124680";    // change password
// Google script ID and required credentials
String GOOGLE_SCRIPT_ID = "AKfycbzPwv9zahTks-2ReYFqFVcUsuQGO3X-jeGpCAygZBFA-iJzwRxUu4xXPPUcX6EPtEmS";    // change Gscript ID
int count = 0;

// ------------------------------------------------------------
#define PCNT_H_LIM_VAL overflow_fan1
#define PCNT_H_LIM_VAL overflow_fan2
#define PCNT_H_LIM_VAL overflow_fan3
#define PCNT_H_LIM_VAL overflow_fan4


#define FREQ_PIN_FAN1 34
#define FREQ_PIN_FAN2 35
#define FREQ_PIN_FAN3 32
#define FREQ_PIN_FAN4 33


// ------------------------------------------------------------

bool            flag_fan1          = true;                                   
uint32_t        overflow_fan1      = 20000;                                  
int16_t         pulses_fan1        = 0;                                  
uint32_t        overflow_cnt_fan1  = 0;                                        
volatile double frequency_fan1     = 0;
uint16_t result_fan1 = 0;

bool            flag_fan2          = true;                                   
uint32_t        overflow_fan2      = 20000;                                  
int16_t         pulses_fan2        = 0;                                  
uint32_t        overflow_cnt_fan2  = 0;                                        
volatile double frequency_fan2     = 0;
uint16_t result_fan2 = 0;

bool            flag_fan3          = true;                                   
uint32_t        overflow_fan3      = 20000;                                  
int16_t         pulses_fan3        = 0;                                  
uint32_t        overflow_cnt_fan3  = 0;                                        
volatile double frequency_fan3     = 0;
uint16_t result_fan3 = 0;

bool            flag_fan4          = true;                                   
uint32_t        overflow_fan4      = 20000;                                  
int16_t         pulses_fan4        = 0;                                  
uint32_t        overflow_cnt_fan4  = 0;                                        
volatile double frequency_fan4    = 0;
uint16_t result_fan4 = 0;

volatile double average_fans = 0;

// ------------------------------------------------------------

esp_timer_create_args_t timer_args_fan1;                                   
esp_timer_handle_t timer_handle_fan1;                                       
portMUX_TYPE timer_mux_fan1 = portMUX_INITIALIZER_UNLOCKED;

esp_timer_create_args_t timer_args_fan2;                                    
esp_timer_handle_t timer_handle_fan2;                                          
portMUX_TYPE timer_mux_fan2 = portMUX_INITIALIZER_UNLOCKED;

esp_timer_create_args_t timer_args_fan3;                                    
esp_timer_handle_t timer_handle_fan3;                                          
portMUX_TYPE timer_mux_fan3 = portMUX_INITIALIZER_UNLOCKED;

esp_timer_create_args_t timer_args_fan4;                                    
esp_timer_handle_t timer_handle_fan4;                                          
portMUX_TYPE timer_mux_fan4 = portMUX_INITIALIZER_UNLOCKED;


// ------------------------------------------------------------

pcnt_config_t pcnt_config_fan1 = {
  .pulse_gpio_num    = FREQ_PIN_FAN1,
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

  pcnt_config_t pcnt_config_fan2 = 
  {
  .pulse_gpio_num    = FREQ_PIN_FAN2,
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

  pcnt_config_t pcnt_config_fan3 = 
  {
  .pulse_gpio_num    = FREQ_PIN_FAN3,
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

  pcnt_config_t pcnt_config_fan4 = 
  {
  .pulse_gpio_num    = FREQ_PIN_FAN4,
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

void IRAM_ATTR pcnt_event_handler_fan1(void *arg)
{
  portENTER_CRITICAL_ISR(&timer_mux_fan1);
  overflow_cnt_fan1++; 
  PCNT.int_clr.val = BIT(PCNT_UNIT_0);
  portEXIT_CRITICAL_ISR(&timer_mux_fan1);
}     

void IRAM_ATTR pcnt_event_handler_fan2(void *arg) 
{
  portENTER_CRITICAL_ISR(&timer_mux_fan2); 
  overflow_cnt_fan2++;  
  PCNT.int_clr.val = BIT(PCNT_UNIT_1); 
  portEXIT_CRITICAL_ISR(&timer_mux_fan2); 
}  

void IRAM_ATTR pcnt_event_handler_fan3(void *arg)
{
  portENTER_CRITICAL_ISR(&timer_mux_fan3); 
  overflow_cnt_fan3++; 
  PCNT.int_clr.val = BIT(PCNT_UNIT_2); 
  portEXIT_CRITICAL_ISR(&timer_mux_fan3);
}

void IRAM_ATTR pcnt_event_handler_fan4(void *arg)
{
  portENTER_CRITICAL_ISR(&timer_mux_fan4); 
  overflow_cnt_fan4++; 
  PCNT.int_clr.val = BIT(PCNT_UNIT_3); 
  portEXIT_CRITICAL_ISR(&timer_mux_fan4);
}

// ------------------------------------------------------------ 

int pwmChannel0 = 0; // Selects channel 0
int pwmChannel1 = 1; // Selects channel 1
int pwmChannel2 = 2; // Selects channel 2
int frequence = 1000; // PWM frequency of 1 KHz
int resolution = 8; // 8-bit resolution, 256 possible values

int pwmPin0 = 23; //Will be assigned to Red Light
int pwmPin1 = 22; //Will be assigned to Yellow Light
int pwmPin2 = 21; //Will be assigned to Green Light

//My Code
 
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

  pcnt_init_fan1();
  pcnt_init_fan2();
  pcnt_init_fan3();
  pcnt_init_fan4();

 pinMode(pwmPin0, OUTPUT);
 pinMode(pwmPin1, OUTPUT);
 pinMode(pwmPin2, OUTPUT);
}

void pcnt_init_fan1(void)                                                     
{  
  pinMode(FREQ_PIN_FAN1, INPUT);

  pcnt_unit_config(&pcnt_config_fan1);
  pcnt_isr_register(pcnt_event_handler_fan1, NULL, 0, NULL);                   
  pcnt_set_filter_value(PCNT_UNIT_0, 1000);
  pcnt_filter_enable(PCNT_UNIT_0); 
  pcnt_counter_pause(PCNT_UNIT_0);                                       
  pcnt_counter_clear(PCNT_UNIT_0);                                       
  pcnt_event_enable(PCNT_UNIT_0, PCNT_EVT_H_LIM);                        
  pcnt_counter_resume(PCNT_UNIT_0);                                       

  timer_args_fan1.callback = pcnt_get_counter_fan1;
  timer_args_fan1.arg      = NULL;
  timer_args_fan1.name     = "one shot timer";

  if(esp_timer_create(&timer_args_fan1, &timer_handle_fan1) != ESP_OK) 
  {
    ESP_LOGE(TAG, "timer create");
  }

  timer_args_fan1.callback = pcnt_get_counter_fan1; 
  esp_timer_create(&timer_args_fan1, &timer_handle_fan1);                           
}

void pcnt_init_fan2(void)                                                     
{  
  pinMode(FREQ_PIN_FAN2, INPUT);

  pcnt_unit_config(&pcnt_config_fan2);
  pcnt_isr_register(pcnt_event_handler_fan2, NULL, 0, NULL);               
  pcnt_intr_enable(PCNT_UNIT_1);  
  pcnt_set_filter_value(PCNT_UNIT_1, 1000);
  pcnt_filter_enable(PCNT_UNIT_1); 
  pcnt_counter_pause(PCNT_UNIT_1);                                      
  pcnt_counter_clear(PCNT_UNIT_1);                                      
  pcnt_event_enable(PCNT_UNIT_1, PCNT_EVT_H_LIM);                        
  pcnt_counter_resume(PCNT_UNIT_1);                                    

  timer_args_fan2.callback = pcnt_get_counter_fan2;
  timer_args_fan2.arg      = NULL;
  timer_args_fan2.name     = "one shot timer";

  if(esp_timer_create(&timer_args_fan2, &timer_handle_fan2) != ESP_OK) 
  {
    ESP_LOGE(TAG, "timer create");
  }

  timer_args_fan2.callback = pcnt_get_counter_fan2;
  esp_timer_create(&timer_args_fan2, &timer_handle_fan2); 
}

void pcnt_init_fan3(void)                                                     
{  
  pinMode(FREQ_PIN_FAN3, INPUT);

  pcnt_unit_config(&pcnt_config_fan3);
  pcnt_isr_register(pcnt_event_handler_fan3, NULL, 0, NULL);
  pcnt_intr_enable(PCNT_UNIT_2);  
  pcnt_set_filter_value(PCNT_UNIT_2, 1000);
  pcnt_filter_enable(PCNT_UNIT_2); 
  pcnt_counter_pause(PCNT_UNIT_2);                                       
  pcnt_counter_clear(PCNT_UNIT_2);                                        
  pcnt_event_enable(PCNT_UNIT_2, PCNT_EVT_H_LIM);                        
  pcnt_counter_resume(PCNT_UNIT_2);                                       

  timer_args_fan3.callback = pcnt_get_counter_fan3;
  timer_args_fan3.arg      = NULL;
  timer_args_fan3.name     = "one shot timer";

  if(esp_timer_create(&timer_args_fan3, &timer_handle_fan3) != ESP_OK) 
  {
    ESP_LOGE(TAG, "timer create");
  }

  timer_args_fan3.callback = pcnt_get_counter_fan3;
  esp_timer_create(&timer_args_fan3, &timer_handle_fan3); 
}

void pcnt_init_fan4(void)                                                     
{  
  pinMode(FREQ_PIN_FAN4, INPUT);

  pcnt_unit_config(&pcnt_config_fan4);
  pcnt_isr_register(pcnt_event_handler_fan4, NULL, 0, NULL);
  pcnt_intr_enable(PCNT_UNIT_3);  
  pcnt_set_filter_value(PCNT_UNIT_3, 1000);
  pcnt_filter_enable(PCNT_UNIT_3); 
  pcnt_counter_pause(PCNT_UNIT_3);                                       
  pcnt_counter_clear(PCNT_UNIT_3);                                        
  pcnt_event_enable(PCNT_UNIT_3, PCNT_EVT_H_LIM);                        
  pcnt_counter_resume(PCNT_UNIT_3);                                       

  timer_args_fan4.callback = pcnt_get_counter_fan4;
  timer_args_fan4.arg      = NULL;
  timer_args_fan4.name     = "one shot timer";

  if(esp_timer_create(&timer_args_fan4, &timer_handle_fan4) != ESP_OK) 
  {
    ESP_LOGE(TAG, "timer create");
  }

  timer_args_fan4.callback = pcnt_get_counter_fan4;
  esp_timer_create(&timer_args_fan4, &timer_handle_fan4); 
}

// ------------------------------------------------------------

void pcnt_get_counter_fan1(void *p) 
{ 
  pcnt_counter_pause(PCNT_UNIT_0);
  pcnt_get_counter_value(PCNT_UNIT_0, (int16_t*) &result_fan1); 
  flag_fan1 = true;
}

void pcnt_get_counter_fan2(void *p) 
{ 
  pcnt_counter_pause(PCNT_UNIT_1);
  pcnt_get_counter_value(PCNT_UNIT_1, (int16_t*) &result_fan2); 
  flag_fan2 = true;
}

void pcnt_get_counter_fan3(void *p) 
{ 
  pcnt_counter_pause(PCNT_UNIT_2);
  pcnt_get_counter_value(PCNT_UNIT_2, (int16_t*) &result_fan3); 
  flag_fan3 = true;
}

void pcnt_get_counter_fan4(void *p) 
{ 
  pcnt_counter_pause(PCNT_UNIT_3);
  pcnt_get_counter_value(PCNT_UNIT_3, (int16_t*) &result_fan4); 
  flag_fan4 = true;
}


void loop() 
{
  if (flag_fan1 == true)
  {
    flag_fan1 = false;
    frequency_fan1 =  (result_fan1 + (overflow_cnt_fan1*20000)) / 2;
    //double freq1 = frequency_fan1 / 2; 
    overflow_cnt_fan1 = 0; 
    pcnt_counter_clear(PCNT_UNIT_0); 
    pcnt_counter_resume(PCNT_UNIT_0); 
    overflow_cnt_fan1 = 0;    
  
    Serial.print(frequency_fan1); //* 15);
    Serial.println(" FREQ PIN 34");

  // float filteredval = f.filterIn(value);
  // 
  // Serial.println(filteredval, 4);
    pcnt_counter_clear(PCNT_UNIT_0);
    esp_timer_start_once(timer_handle_fan1, 1000000);




 

  
  if (flag_fan2 == true)
  {
    flag_fan2 = false;
    frequency_fan2 =  (result_fan2 + (overflow_cnt_fan2 * 20000)) / 2; 
    //double freq2 = frequency_fan2 / 2; 
    overflow_cnt_fan2 = 0; 
    pcnt_counter_clear(PCNT_UNIT_1); 
    pcnt_counter_resume(PCNT_UNIT_1); 
    overflow_cnt_fan2 = 0;    
  
    Serial.print(frequency_fan2);
    Serial.println(" FREQ PIN 35");

    pcnt_counter_clear(PCNT_UNIT_1);
    esp_timer_start_once(timer_handle_fan2, 1000000);
  }

    // check the sensor value and set the traffic light accordingly
   if (1200 <= frequency_fan2 && frequency_fan2 <= 1800)
   {
   digitalWrite(21, HIGH);
   digitalWrite(22, LOW);
   digitalWrite(23, LOW);
   Serial.println("green");
   Serial.print(frequency_fan2);
   }
   if ((1000 <= frequency_fan2 && frequency_fan2 <= 1199) || (1801 <= frequency_fan2 && frequency_fan2 <= 2000))
   {
   digitalWrite(21, LOW);
   digitalWrite(22, HIGH);
   digitalWrite(23, LOW);
   Serial.println("yellow");
   Serial.print(frequency_fan2);
   }
   if ((0 <= frequency_fan2 && frequency_fan2 <= 999) || 2001 <= frequency_fan2)
   {
   digitalWrite(21, LOW);
   digitalWrite(22, LOW);
   digitalWrite(23, HIGH);
   Serial.println("red");
   Serial.print(frequency_fan2);
   }
  }

  if (flag_fan3 == true)
  {
    flag_fan3 = false;
    frequency_fan3 =  (result_fan3 + (overflow_cnt_fan3 * 20000)) / 2;
    //double freq3 = frequency_fan3 / 2; 
    overflow_cnt_fan3 = 0; 
    pcnt_counter_clear(PCNT_UNIT_2); 
    pcnt_counter_resume(PCNT_UNIT_2); 
    overflow_cnt_fan3 = 0;    
  
    Serial.print(frequency_fan3);
    Serial.println(" FREQ PIN 32");

    pcnt_counter_clear(PCNT_UNIT_2);
    esp_timer_start_once(timer_handle_fan3, 1000000);
  }

  if (flag_fan4 == true)
  {
    flag_fan4 = false;
    frequency_fan4 =  (result_fan4 + (overflow_cnt_fan4 * 20000)) / 2;
    //double freq4 = frequency_fan4 / 2; 
    overflow_cnt_fan4 = 0; 
    pcnt_counter_clear(PCNT_UNIT_3); 
    pcnt_counter_resume(PCNT_UNIT_3); 
    overflow_cnt_fan4 = 0;    
  
    Serial.print(frequency_fan4);
    Serial.println(" FREQ PIN 33");

    pcnt_counter_clear(PCNT_UNIT_3);
    esp_timer_start_once(timer_handle_fan4, 1000000);
  }

  
  //average_fans = (frequency_fan1 + frequency_fan2 + frequency_fan3 + frequency_fan4 + frequency_fan5 + frequency_fan6) / 6 * 15;
  //Serial.print(average_fans);
  //Serial.println(" Average RPM'S of 6 fans");
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
    String urlFinal = "https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?"+"Hour=" + hour + "&Minute=" + minute + "&Second=" + second + "&FREQ1=" + (int)frequency_fan1 + "&FREQ2=" + (int)frequency_fan2 + "&FREQ3=" + (int)frequency_fan3 + "&FREQ4=" + (int)frequency_fan4;
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
