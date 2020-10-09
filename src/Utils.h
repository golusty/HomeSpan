
#pragma once

#include <Arduino.h>
#include <driver/timer.h>

namespace Utils {

char *readSerial(char *c, int max);   // read serial port into 'c' until <newline>, but storing only first 'max' characters (the rest are discarded)
String mask(char *c, int n);          // simply utility that creates a String from 'c' with all except the first and last 'n' characters replaced by '*'
  
}

/////////////////////////////////////////////////
// Creates a temporary buffer that is freed after
// going out of scope

template <class bufType>
struct TempBuffer {
  bufType *buf;
  int nBytes;
  
  TempBuffer(size_t len){
    nBytes=len*sizeof(bufType);
    buf=(bufType *)heap_caps_malloc(nBytes,MALLOC_CAP_8BIT);      
   }

  ~TempBuffer(){
    heap_caps_free(buf);
  }

  int len(){
    return(nBytes);
  }
  
};

////////////////////////////////
//         PushButton         //
////////////////////////////////

class PushButton{
  
  int status;
  uint8_t pin;
  uint32_t shortAlarm;
  uint32_t longAlarm;
  boolean isLongPress;

  public:
  
  PushButton();
  PushButton(uint8_t pin);

//  Creates generic pushbutton functionality on specified pin
//  that is wired to connect to ground when the button is pressed.
//
//  In the first form, a PushButton is instantiated without specifying
//  the pin.  In this case the pin must be specified in a subsequent call 
//  to init() before the PushButton can be used.
//
//  In the second form, a PushButton is instantiated and initialized with
//  the specified pin, obviating the need for a separate call to init().
//
//  pin:         Pin mumber to which pushbutton connects to ground when pressed

  void init(uint8_t pin);
  
//  Initializes PushButton, if not configured during instantiation.
//
//  pin:         Pin mumber to which pushbutton connects to ground when pressed
    
  void reset();

//  Resets state of PushButton.  Should be called once before any loops that will
//  repeatedly check the button for a trigger event.

  boolean triggered(uint16_t shortTime, uint16_t longTime);

//  Returns true if button has been triggered by either a Long Press or Short Press, where a
//  Long Press is a press and hold for at least longTime milliseconds, and a Short Press is 
//  a press and release of at least shortTime milliseconds but less than longTime milliseconds.
//
//  shortTime:   the minimum time required for the button to be pressed before releasing to trigger a Short Press
//  longtime:    the minimum time required for the button to be pressed and held to trigger a Long Press
//
//  If shortTime>longTime, only Long Press triggers will occur.  Once triggered() returns true, if will subsequently
//  return false until there is a new trigger.  After a Long Press, the button must be released to permit a subsequent
//  trigger.

  boolean primed();

//  Returns true if button has been pressed and held for greater than shortTime, but has not yet been released.
//  After returning true, subsequent calls will always return false until the button has been released and reset.

  boolean longPress();

//  Returns true if last trigger event was a Long Press, or false if last trigger was a Short Press  

  void wait();

//  Waits for button to be released.  Use after Long Press if button release confirmation is desired

};

////////////////////////////////
//         Blinker            //
////////////////////////////////

class Blinker {
  
  timer_group_t group;
  timer_idx_t idx;
  int pin;

  int nBlinks;
  int onTime;
  int offTime;
  int delayTime;
  int count;

  static void isrTimer(void *arg); 

  public:

  Blinker();
  Blinker(int pin, int timerNum=0);

//  Creates a generic blinking LED on specified pin controlled
//  in background via interrupts generated by an ESP32 Alarm Timer.
//
//  In the first form, a Blinker is instantiated without specifying
//  the pin.  In this case the pin must be specified in a subsequent call 
//  to init() before the Blinker can be used.
//
//  In the second form, a Blinker is instantiated and initialized with
//  the specified pin, obviating the need for a separate call to init().
//
//  pin:         Pin mumber to control.  Blinker will set pinMode to OUTPUT automatically 
//  timerNum:    ESP32 Alarm Timer to use. 0=Group0/Timer0, 1=Group0/Timer1, 2=Group1/Timer0, 3=Group1/Timer1
    
  void init(int pin, int timerNum=0);

//  Initializes Blinker, if not configured during instantiation.
//
//  pin:         Pin mumber to control.  Blinker will set pinMode to OUTPUT automatically 
//  timerNum:    ESP32 Alarm Timer to use. 0=Group0/Timer0, 1=Group0/Timer1, 2=Group1/Timer0, 3=Group1/Timer1

  void start(int period, float dutyCycle=0.5);
    
//  Starts simple ON/OFF blinking.
//
//  period:      ON/OFF blinking period, in milliseconds
//  dutyCycle:   Fraction of period that LED is ON (default=50%)

  void start(int period, float dutyCycle, int nBlinks, int delayTime);

//  Starts ON/OFF blinking pattern.
//
//  period:      ON/OFF blinking period, in milliseconds, used for blinking portion of pattern
//  dutyCycle:   Fraction of period that LED is ON (default=50%)
//  nBlinks:     Number of blinks in blinking portion of pattern
//  delayTime:   delay, in milliseconds, during which LED is off before restarting blinking pattern

  void stop();

//  Stops current blinking pattern.

  void on();

//  Stops current blinking pattern and turns on LED

  void off();

//  Stops current blinking pattern and turns off LED

};
