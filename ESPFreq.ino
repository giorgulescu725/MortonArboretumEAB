#include "arduino.h"
#include "driver/pcnt.h"                                                
#include "soc/pcnt_struct.h"
#include <filters.h>

// ------------------------------------------------------------

#define PCNT_H_LIM_VAL overflow_fan1


#define FREQ_PIN_FAN1 34


// ------------------------------------------------------------

bool            flag_fan1          = true;                                   
uint32_t        overflow_fan1      = 20000;                                  
int16_t         pulses_fan1        = 0;                                  
uint32_t        overflow_cnt_fan1  = 0;                                        
volatile double frequency_fan1     = 0;
uint16_t result_fan1 = 0;



volatile double average_fans = 0;

// ------------------------------------------------------------

esp_timer_create_args_t timer_args_fan1;                                   
esp_timer_handle_t timer_handle_fan1;                                       
portMUX_TYPE timer_mux_fan1 = portMUX_INITIALIZER_UNLOCKED;



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

 

// ------------------------------------------------------------

void IRAM_ATTR pcnt_event_handler_fan1(void *arg)
{
  portENTER_CRITICAL_ISR(&timer_mux_fan1);
  overflow_cnt_fan1++; 
  PCNT.int_clr.val = BIT(PCNT_UNIT_0);
  portEXIT_CRITICAL_ISR(&timer_mux_fan1);
}     



// ------------------------------------------------------------ 

//My Code
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
//const float cutoff_freq   = 0;  //Cutoff frequency in Hz
//const float sampling_time = 0.001; //Sampling time in seconds.
//IIR::ORDER  order  = IIR::ORDER::OD4; // Order (OD1 to OD4)

// Low-pass filter
// Filter f(cutoff_freq, sampling_time, order);


void setup()
{
  Serial.begin(115200);                                                  
  pcnt_init_fan1();


  // My code for PWM Channel
  // Configuration of channel 0 with the chosen frequency and resolution
    ledcSetup(pwmChannel0, frequence, resolution);
    ledcSetup(pwmChannel1, frequence, resolution);
    ledcSetup(pwmChannel2, frequence, resolution);
  
      // Assigns the PWM channel to pin 23/22/21
      ledcAttachPin(pwmPin0, pwmChannel0);
      ledcAttachPin(pwmPin1, pwmChannel1);
      ledcAttachPin(pwmPin2, pwmChannel2);
//  
//  
//      // Create the selected output voltage
//      ledcWrite(pwmChannel0, 127); // 1.65 V
//      ledcWrite(pwmChannel1, 127); // 1.65 V
//      ledcWrite(pwmChannel2, 255); // 1.65 V

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


void pcnt_get_counter_fan1(void *p) 
{ 
  pcnt_counter_pause(PCNT_UNIT_0);
  pcnt_get_counter_value(PCNT_UNIT_0, (int16_t*) &result_fan1); 
  flag_fan1 = true;
}


void loop() 
{
  if (flag_fan1 == true)
  {
    flag_fan1 = false;
    frequency_fan1 =  result_fan1 + (overflow_cnt_fan1*20000); 
    overflow_cnt_fan1 = 0; 
    pcnt_counter_clear(PCNT_UNIT_0); 
    pcnt_counter_resume(PCNT_UNIT_0); 
    overflow_cnt_fan1 = 0;    
    Serial.print(frequency_fan1 / 2); //* 15);
    Serial.println("FREQ PIN");

    
    int value = (frequency_fan1 / 2);
//    float filteredval = f.filterIn(value);
//    
//    Serial.println(filteredval, 4);

    
    pcnt_counter_clear(PCNT_UNIT_0);
    esp_timer_start_once(timer_handle_fan1, 1000000);

  while (Serial.available() == 0);

  // read the value from the serial port
  int sensorValue = Serial.parseInt();
    
  // check the sensor value and set the traffic light accordingly
  if (sensorValue < 0) {
    // invalid sensor value, turn off all lights
   ledcWrite(pwmChannel0, 0); // 1.65 V
   ledcWrite(pwmChannel1, 0); // 1.65 V
   ledcWrite(pwmChannel2, 0); // 1.65 V

  }
  else if (1200 < sensorValue < 1800) {
    ledcWrite(pwmChannel0, 0); // 1.65 V
    ledcWrite(pwmChannel1, 0); // 1.65 V
    ledcWrite(pwmChannel2, 255); // 1.65 V

  }
  else if (sensorValue != 0) {
    ledcWrite(pwmChannel0, 0); // 1.65 V
    ledcWrite(pwmChannel1, 127); // 1.65 V
    ledcWrite(pwmChannel2, 0); // 1.65 V

  }
  else {
    // sensor value is too high, set yellow light
    ledcWrite(pwmChannel0, 0); // 1.65 V
    ledcWrite(pwmChannel1, 0); // 1.65 V
    ledcWrite(pwmChannel2, 255); // 1.65 V
  }

   
    
//    if(1000 < (frequency_fan1 / 2) < 2000)
//    {
//  
//    ledcWrite(pwmChannel0, 255); // 1.65 V
//    
//    }
//    else if ((frequency_fan1 / 2) > 2000 || (frequency_fan1 / 2) < 1000)
//    {
//    
//    ledcWrite(pwmChannel2, 127); // 1.65 V
//    }
//    else {
//    ledcWrite(pwmChannel1, 127); // 1.65 V
//    }
  }
  
  delay(10);
}
