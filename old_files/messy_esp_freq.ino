#include "arduino.h"
#include "driver/pcnt.h"                                                
#include "soc/pcnt_struct.h"

// ------------------------------------------------------------

#define PCNT_H_LIM_VAL overflow_fan1
#define PCNT_H_LIM_VAL overflow_fan2
#define PCNT_H_LIM_VAL overflow_fan3
#define PCNT_H_LIM_VAL overflow_fan4
#define PCNT_H_LIM_VAL overflow_fan5
#define PCNT_H_LIM_VAL overflow_fan6

#define FREQ_PIN_FAN1 34
#define FREQ_PIN_FAN2 35
#define FREQ_PIN_FAN3 32
#define FREQ_PIN_FAN4 33
#define FREQ_PIN_FAN5 25
#define FREQ_PIN_FAN6 26

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

bool            flag_fan5          = false;                                   
uint32_t        overflow_fan5      = 20000;                                  
int16_t         pulses_fan5        = 0;                                  
uint32_t        overflow_cnt_fan5  = 0;                                        
volatile double frequency_fan5     = 0;
uint16_t result_fan5 = 0;

bool            flag_fan6          = false;                                   
uint32_t        overflow_fan6      = 20000;                                  
int16_t         pulses_fan6        = 0;                                  
uint32_t        overflow_cnt_fan6  = 0;                                        
volatile double frequency_fan6     = 0;
uint16_t result_fan6 = 0;

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

esp_timer_create_args_t timer_args_fan5;                                    
esp_timer_handle_t timer_handle_fan5;                                          
portMUX_TYPE timer_mux_fan5 = portMUX_INITIALIZER_UNLOCKED;

esp_timer_create_args_t timer_args_fan6;                                   
esp_timer_handle_t timer_handle_fan6;                                       
portMUX_TYPE timer_mux_fan6 = portMUX_INITIALIZER_UNLOCKED;

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

  pcnt_config_t pcnt_config_fan5 = 
  {
  .pulse_gpio_num    = FREQ_PIN_FAN5,
  .ctrl_gpio_num     = -1,
  .lctrl_mode        = PCNT_MODE_KEEP,
  .hctrl_mode        = PCNT_MODE_KEEP,
  .pos_mode          = PCNT_COUNT_INC,
  .neg_mode          = PCNT_COUNT_INC,
  .counter_h_lim     = 20000,
  .counter_l_lim     = 0,
  .unit              = PCNT_UNIT_4, 
  .channel           = PCNT_CHANNEL_0
  };

  pcnt_config_t pcnt_config_fan6 = 
  {
  .pulse_gpio_num    = FREQ_PIN_FAN6,
  .ctrl_gpio_num     = -1,
  .lctrl_mode        = PCNT_MODE_KEEP,
  .hctrl_mode        = PCNT_MODE_KEEP,
  .pos_mode          = PCNT_COUNT_INC,
  .neg_mode          = PCNT_COUNT_INC,
  .counter_h_lim     = 20000,
  .counter_l_lim     = 0,
  .unit              = PCNT_UNIT_5, 
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

void IRAM_ATTR pcnt_event_handler_fan5(void *arg)
{
  portENTER_CRITICAL_ISR(&timer_mux_fan5); 
  overflow_cnt_fan5++; 
  PCNT.int_clr.val = BIT(PCNT_UNIT_4); 
  portEXIT_CRITICAL_ISR(&timer_mux_fan5);
}

void IRAM_ATTR pcnt_event_handler_fan6(void *arg)
{
  portENTER_CRITICAL_ISR(&timer_mux_fan6);
  overflow_cnt_fan6++; 
  PCNT.int_clr.val = BIT(PCNT_UNIT_5);
  portEXIT_CRITICAL_ISR(&timer_mux_fan6);
}   

// ------------------------------------------------------------ 

void setup()
{
  Serial.begin(115200);                                                  
  pcnt_init_fan1();
  pcnt_init_fan2();
  pcnt_init_fan3();
  pcnt_init_fan4();
  pcnt_init_fan5();
  pcnt_init_fan6();
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

void pcnt_init_fan5(void)                                                     
{  
  pinMode(FREQ_PIN_FAN5, INPUT);

  pcnt_unit_config(&pcnt_config_fan5);
  pcnt_isr_register(pcnt_event_handler_fan5, NULL, 0, NULL);
  pcnt_intr_enable(PCNT_UNIT_4);  
  pcnt_set_filter_value(PCNT_UNIT_4, 1000);
  pcnt_filter_enable(PCNT_UNIT_4); 
  pcnt_counter_pause(PCNT_UNIT_4);                                       
  pcnt_counter_clear(PCNT_UNIT_4);                                        
  pcnt_event_enable(PCNT_UNIT_4, PCNT_EVT_H_LIM);                        
  pcnt_counter_resume(PCNT_UNIT_4);                                       
  timer_args_fan5.callback = pcnt_get_counter_fan5;
  timer_args_fan5.arg      = NULL;
  timer_args_fan5.name     = "one shot timer";

  if(esp_timer_create(&timer_args_fan5, &timer_handle_fan5) != ESP_OK) 
  {
    ESP_LOGE(TAG, "timer create");
  }

  timer_args_fan5.callback = pcnt_get_counter_fan5;
  esp_timer_create(&timer_args_fan5, &timer_handle_fan5); 
}

void pcnt_init_fan6(void)                                                     
{  
  pinMode(FREQ_PIN_FAN6, INPUT);

  pcnt_unit_config(&pcnt_config_fan6);
  pcnt_isr_register(pcnt_event_handler_fan6, NULL, 0, NULL);                   
  pcnt_set_filter_value(PCNT_UNIT_5, 1000);
  pcnt_filter_enable(PCNT_UNIT_5); 
  pcnt_counter_pause(PCNT_UNIT_5);                                       
  pcnt_counter_clear(PCNT_UNIT_5);                                       
  pcnt_event_enable(PCNT_UNIT_5, PCNT_EVT_H_LIM);                        
  pcnt_counter_resume(PCNT_UNIT_5);                                       

  timer_args_fan6.callback = pcnt_get_counter_fan6;
  timer_args_fan6.arg      = NULL;
  timer_args_fan6.name     = "one shot timer";

  if(esp_timer_create(&timer_args_fan6, &timer_handle_fan6) != ESP_OK) 
  {
    ESP_LOGE(TAG, "timer create");
  }

  timer_args_fan6.callback = pcnt_get_counter_fan6; 
  esp_timer_create(&timer_args_fan6, &timer_handle_fan6);                           
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

void pcnt_get_counter_fan5(void *p) 
{ 
  pcnt_counter_pause(PCNT_UNIT_4);
  pcnt_get_counter_value(PCNT_UNIT_4, (int16_t*) &result_fan5); 
  flag_fan5 = true;
}

void pcnt_get_counter_fan6(void *p) 
{ 
  pcnt_counter_pause(PCNT_UNIT_5);
  pcnt_get_counter_value(PCNT_UNIT_5, (int16_t*) &result_fan6); 
  flag_fan6 = true;
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
    Serial.println(" FREQ PIN 34");

    pcnt_counter_clear(PCNT_UNIT_0);
    esp_timer_start_once(timer_handle_fan1, 1000000);
  }

  if (flag_fan2 == true)
  {
    flag_fan2 = false;
    frequency_fan2 =  result_fan2 + (overflow_cnt_fan2 * 20000); 
    overflow_cnt_fan2 = 0; 
    pcnt_counter_clear(PCNT_UNIT_1); 
    pcnt_counter_resume(PCNT_UNIT_1); 
    overflow_cnt_fan2 = 0;    
  
    Serial.print(frequency_fan2 / 2);
    Serial.println(" FREQ PIN 35");

    pcnt_counter_clear(PCNT_UNIT_1);
    esp_timer_start_once(timer_handle_fan2, 1000000);
  }

  if (flag_fan3 == true)
  {
    flag_fan3 = false;
    frequency_fan3 =  result_fan3 + (overflow_cnt_fan3 * 20000); 
    overflow_cnt_fan3 = 0; 
    pcnt_counter_clear(PCNT_UNIT_2); 
    pcnt_counter_resume(PCNT_UNIT_2); 
    overflow_cnt_fan3 = 0;    
  
    Serial.print(frequency_fan3 / 2);
    Serial.println(" FREQ PIN 32");

    pcnt_counter_clear(PCNT_UNIT_2);
    esp_timer_start_once(timer_handle_fan3, 1000000);
  }

  if (flag_fan4 == true)
  {
    flag_fan4 = false;
    frequency_fan4 =  result_fan4 + (overflow_cnt_fan4 * 20000); 
    overflow_cnt_fan4 = 0; 
    pcnt_counter_clear(PCNT_UNIT_3); 
    pcnt_counter_resume(PCNT_UNIT_3); 
    overflow_cnt_fan4 = 0;    
  
    Serial.print(frequency_fan4 / 2);
    Serial.println(" FREQ PIN 33");

    pcnt_counter_clear(PCNT_UNIT_3);
    esp_timer_start_once(timer_handle_fan4, 1000000);
  }

  if (flag_fan5 == true)
  {
    flag_fan5 = false;
    frequency_fan5 =  result_fan5 + (overflow_cnt_fan5 * 20000); 
    overflow_cnt_fan5 = 0; 
    pcnt_counter_clear(PCNT_UNIT_4); 
    pcnt_counter_resume(PCNT_UNIT_4); 
    overflow_cnt_fan5 = 0;    
  
    Serial.print(frequency_fan5 * 15);
    Serial.println(" RPM FAN5");

    pcnt_counter_clear(PCNT_UNIT_4);
    esp_timer_start_once(timer_handle_fan5, 1000000);
  }

  if (flag_fan6 == true)
  {
    flag_fan6 = false;
    frequency_fan6 =  result_fan6 + (overflow_cnt_fan6*20000); 
    overflow_cnt_fan6 = 0; 
    pcnt_counter_clear(PCNT_UNIT_5); 
    pcnt_counter_resume(PCNT_UNIT_5); 
    overflow_cnt_fan6 = 0;    
  
    Serial.print(frequency_fan6 * 15);
    Serial.println(" RPM FAN6");

    pcnt_counter_clear(PCNT_UNIT_5);
    esp_timer_start_once(timer_handle_fan6, 1000000);
  }
  //average_fans = (frequency_fan1 + frequency_fan2 + frequency_fan3 + frequency_fan4 + frequency_fan5 + frequency_fan6) / 6 * 15;
  //Serial.print(average_fans);
  //Serial.println(" Average RPM'S of 6 fans");
  
  delay(1000);
}
