#include <M5Stack.h>
#include <Arduino.h>

struct pos
{
	int x;
	int y;
};

float h_in = 0; // sound volume
float r_in = 0; // heartbeat rate

float r_min = 60;
float r_max = 240;
float h_min = 0; //!
float h_max = 0; //!

// Motor submodule
const int amp_pin = 16; // pin for output 1
const int freq_pin = 21;
// const int freq = 1000;		// the frequency itws going to
const int freq_channel = 0; // channel which we are going to broadcast the frequency on
const int amp_channel = 1;
const int resolution = 8; // how much the voltage jumps in steps (3.3v / resolutions = steps per second)

// Heartbeat submodule

const int pResistor = 15; //photot resistor assignedto pin 15 
const int size = 10;
int counter;
const int delayMs = 1;
float val_old = 0, val_new = 0, time_old = 0, time_new = 1;
float threshold = 1200;
float period = 0;
float freq = 0, old_freq = 0,avg_freq = 0, out_freq = 0;
int freq_array[size] = {};
int int_freq = 0,int_out_freq = 0;
float tempreading;
float min_v = 5000, max_v = 0;

//Algorithm submodule
int time_stab = 2*1000; // time to stabilize
int tolerance = 10;

float frequency[] = {0.05, 0.2, 0.4, 0.6, 0.8};
float amp[] = {0.05, 0.2, 0.4, 0.6, 0.8};

pos pos_cur{4, 4};
pos pos_prev{4, 4};

bool DOWN = 1;
bool LEFT = 1;

float stress_cur = 0;  // current stress level
float stress_prev = 0; // previous stress level

void motor_control(int pwm, float freq)
{
	ledcWrite(freq_channel, pwm * 256);
	ledcWrite(amp_channel, freq * 256);
}

// float sound() {};

int heartbeat()
{
    val_new = analogRead(pResistor); // reads value on pin

    if (val_new >= threshold && val_old <= threshold)
    {                        // if value previously was below threshold and now is over it
        time_old = time_new; // register it as a peak of the wave
        time_new = millis();

        period = time_new - time_old; // calculates the period of the wave
        freq = 60 / (period / 1000);
        // Glitch detection
        freq_array[counter] = freq;

        /*if(freq_array[counter]+50<old_freq || freq_array[counter]-50>old_freq){ //
          freq_array[counter] = 0;
          if(counter>0){
            counter--;
          }
        }*/
        old_freq = freq;
        if (counter == size - 1)
        {
            avg_freq = 0;
            out_freq = 0;
            for (int i = 0; i < size - 1; i++)
            {
                avg_freq += freq_array[i];
            }
            out_freq = avg_freq / counter;
            counter = 0;
        }
        else
        {
            counter++;
        }
        int_freq = (int)freq;
        int_out_freq = (int)out_freq;
        M5.Lcd.setCursor(0, 0);
        M5.Lcd.printf("FREQ = %06.2f\n", freq);
        M5.Lcd.printf("Period = %06.2f\n", period);
        // M5.Lcd.printf("int_FREQ = %04i\n", int_freq);
        M5.Lcd.printf("int_FREQ = %04i\n", int_out_freq);
    }
    val_old = val_new;
    delay(delayMs);
    return int_out_freq;
}
/*
float stress_level()
{
	float h_in = sound_vol();
    float r_in = 0;
    float s_r = 0;
	float s_h = 0;

	//process sound volume
	if (h_in <= h_min) s_h = 10;
	else if (h_in <= hmax) s_h = 40/100*h_in + 10;
	else {
        r_in = hearbeat();
        if (r_in <= r_min) s_r = 10;
	    else s_r = (90 * r_in + (10 * r_max - 100 * r_min)) / (r_max - r_min);
        return s_r;
	}
    return s_h;

}
*/
float stress_level(){
    float s_r = 0;
	int r_in = heartbeat();
	//* only process heartbeat rate 
	if (r_in <= r_min) s_r = 10;
	else s_r = (90 * r_in + (10 * r_max - 100 * r_min)) / (r_max - r_min);
	return s_r;
}

void trying()
{
    pos_prev.x = pos_cur.x;
    pos_prev.y = pos_cur.y;

	if (pos_cur.y - 1 < 0)
		DOWN = 0;
	if (pos_cur.x - 1 < 0)
		LEFT = 0;
	// if (pos_cur.x + 1 > 4) RIGHT = 0;

	if (DOWN)
	{
		pos_cur.y -= 1;
		DOWN = 0;
	}
	else if (LEFT)
	{
		pos_cur.x -= 1;
		LEFT = 0;
	}
	// else if (RIGHT)
	// {
	//     pos_cur.x += 1;
	//     RIGHT = 0;
	// }
}

void setup()
{
	M5.begin(); // Init M5Core. Initialize M5Core
	M5.Power.begin();
	// Heartbeat submodule
	pinMode(pResistor, INPUT); // Set pResistor as INPUT
    Serial.begin(9600); //Serial setup for terminal monitoring
    M5.Lcd.setTextSize(2);
    M5.Lcd.drawLine(0, 90, 320, 90, WHITE);
	// Motor submodule
	ledcSetup(freq_channel, 1000, resolution);
	ledcAttachPin(freq_pin, freq_channel);
	ledcSetup(amp_channel, 1000, resolution);
	ledcAttachPin(amp_pin, amp_channel);

}

void loop()
{
	stress_cur = stress_level();
	//! figure out the state

	stress_prev = stress_cur;
    delay(delayMs*5);
    stress_cur = stress_prev;
	// if stress not increasing
	if (stress_cur - stress_prev < tolerance)
	{
        DOWN = 1;
        LEFT = 1;
        //if baby comes to rest at (0, 0)
		if (pos_cur.x + pos_cur.y == 0) return;
        trying();
	}
	// if stress is increasing/unstable
	else
	{
		pos_cur.x = pos_prev.x;
        pos_cur.y = pos_prev.y;
        //go back to previous mode to calm baby and try left
        motor_control(amp[pos_cur.y], frequency[pos_cur.x]);
        delay(time_stab);
        trying();
	}

	motor_control(amp[pos_cur.y], frequency[pos_cur.x]);
}