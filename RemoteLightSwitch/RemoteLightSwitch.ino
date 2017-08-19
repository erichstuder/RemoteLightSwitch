#include <avr/sleep.h>

#define LED_PIN 13 
#define RELAY_PIN 12
#define SWITCH_PIN 2
bool lampState = false;
volatile bool switchState = false;
volatile bool switch_interrupt_happened = false;

 
void setup(){
  pinMode(RELAY_PIN, OUTPUT);
  
  digitalWrite(RELAY_PIN, HIGH);//switch lamp off
  lampState = false;
  
  pinMode(SWITCH_PIN, INPUT_PULLUP);


  if(digitalRead(SWITCH_PIN) == false){
    attachInterrupt(digitalPinToInterrupt(SWITCH_PIN), switch_interrupt_rising, RISING);
  } else {
    attachInterrupt(digitalPinToInterrupt(SWITCH_PIN), switch_interrupt_falling, FALLING);
  }

  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_enable();
}
  
void loop(){
  static unsigned long last_interrupt_time_ms = 0;
  static unsigned long interrupt_time_ms = 0;
  
  //digitalWrite(LED_PIN, HIGH);//debug

  do{
    switch_interrupt_happened = false;
    //interrupts();
    sleep_mode(); //go to sleep
    //the processore wakes up every millisecond as the timer for millis() is still running
    sleep_disable();
  }while(switch_interrupt_happened == false);

  // it has been found that short peaks may occure which trigger the interrupts
  // to prevent this peaks to switch the lamp a delay is set
  delay(10);

  if ( switchState == digitalRead(SWITCH_PIN) ){ //if after delay the port has the same state it is ok to switch the lamp
    toggleLampState();
    //last_interrupt_time_ms = interrupt_time_ms;

    if(switchState == false){
      attachInterrupt(digitalPinToInterrupt(SWITCH_PIN), switch_interrupt_rising, RISING);
    } else {
      attachInterrupt(digitalPinToInterrupt(SWITCH_PIN), switch_interrupt_falling, FALLING);
    }
  }
  delay(200); //next switch is not possible before this delay
}

void switch_interrupt_rising(){
  detachInterrupt(switch_interrupt_rising);
  switchState = true;
  switch_interrupt_happened = true;
}

void switch_interrupt_falling(){
  detachInterrupt(switch_interrupt_falling);
  switchState = false;
  switch_interrupt_happened = true;
}


void toggleLampState(){
  lampState = !lampState;
  
  if(lampState == true){
    //turn on
    digitalWrite(RELAY_PIN, LOW);
  }
  else{
    //turn off
    digitalWrite(RELAY_PIN, HIGH);  
  }
}
