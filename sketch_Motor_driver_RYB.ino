/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const int ledPin1 = 16; // pin for output 1
const int ledPin2 = 21;
const int freq = 1000; // the frequency itws going to 
const int ledChannelfreq = 0; // channel which we are going to broadcast the frequency on
const int ledChannelampli = 1;
const int resolution = 8; // how much the voltage jumps in steps (3.3v / resolutions = steps per second)
int potValue = 0;
int dutycycle1 = 127; // duty cycle for frequency  in bits from 0 to 256 
int dutycycle2 = 180;
int num1 = dutycycle1100/256;
int num2 = dutycycle2100/256;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <M5Stack.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  M5.begin();  //Init M5Core. Initialize M5Core
  M5.Power.begin();
  Serial.begin(115200);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setTextWrap(true, true);

  //M5.Lcd.printf("PWM WAVE MOTOR"); // 




  ledcSetup(ledChannelfreq, freq, resolution);


  ledcAttachPin(ledPin1, ledChannelfreq);

  ledcSetup(ledChannelampli, freq, resolution);


  ledcAttachPin(ledPin2, ledChannelampli);


}

void loop() {

ledcWrite(ledChannelfreq, dutycycle1);
ledcWrite(ledChannelampli, dutycycle2);

M5.Lcd.setCursor(0, 0);
M5.Lcd.printf("PWM WAVE MOTOR \nnum1 = %02d  \nnum2 = %02d ", num1, num2);
delay(1000);






}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
