//Shoban Venugopal Palani, Net ID: sv2244
//Zayaan Mohammed Muthair, Net ID: zmm2009
//Bhautik Ghanshyam Sudani, Net ID: bgs8255
// Santosh Srinivas Ravichandran, Net ID: sr6411

/*
First, we initialise the sensor and disregard the first 10 peaks due to noise. 
Then, we measure elapsed times between consecutive peaks for 30 peaks 
and average it out to get the normal bpm for the current environment. 
Then, we keep calculating the bpm using moving average of elapsed times with a window size 10.
If the bpm drops below the normal bpm minus 10, then we toggle the LEDs which indicates that the person has stopped breathing for 10 seconds  
If at any point, the sensor is misplaced (not reading peaks), then this code will print "Sensor is not in place (disconnected)"
*/

#include <mbed.h>
#include <stdio.h>
#include <stdlib.h>

volatile float read_value; //variable read_value stores the analog value coming from the sensor (range: 0 to 1)
volatile float elapsed_time; //variable elapsed_time stores the elapsed time between consecutive peaks in the signal
volatile float  f;// variable f stores the frequency of the signal (inverse of elapsed time)
volatile float  bpm; // variable bpm stores the calculated beats per minute (f * 60 * 1000)
volatile float elapsed_times[]={500,590,550,530,600,570,580,550,600,580}; // Random initialisation of Array "elapsed_times" which is used to calculate moving average of the elapsed times with a window size 10. Real time values will be stored in the array later.
volatile float normal_sum=0; //variable normal_sum is used to calculate average 
volatile float normal_bpm; //variable normal_bpm stores the normal bpm value calculated for the current environment 
volatile float corner_case_time; // variable corner_case_time stores the elapsed time after a peak such that if the time is too large, an indication is given that the sensor has been disconnected


AnalogIn ain(PA_6); // Sensor's Analog pin is connect to pin PA_6
DigitalOut led1(LED1); //LED1 and LED2 is toggled when the person stops breathing for 10 seconds
DigitalOut led2(LED2);
Timer t; //Timer t is used to calculate the elapsed time between consecutive peaks in the signal
Timer c;// Timer c is used to detect sensor misplacement by storing the elapsed time after a peak such that if the time is too large, an indication is given that the sensor is not in place

int ctr = 0; //counter variable


int main(){
  
  while (ctr<10) // the first 10 peaks are disregarded for allowing the sensor to initialise (first 10 peaks are less accurate) 
  { 
    read_value=ain.read(); // Analog value from the sensor is read

    if(read_value>0.85) // If the read analog value is greater than 0.85, it is detected as a peak. The value 0.85 is set by trial and error method.
    {

      ctr+=1;

    }

  }

  ctr=0;
  printf("Getting averaged normal bpm for the current environment for setting threshold");
  t.start(); //Timer t is started
  c.start();// Timer c is started to detect sensor misplacement
  
  while (ctr<30) //elapsed time between peaks is recorded for 30 peaks for getting the averaged normal bpm
  { 
    read_value=ain.read(); // Analog value from the sensor is read

    if(read_value>0.85) // If the read analog value is greater than 0.85, it is detected as a peak. The value 0.85 is set by trial and error method.
    {
      elapsed_time = t.read_ms(); // The value from timer t is read and stored in the variable elapsed_time 
      printf("etime0: %f \n", elapsed_time);
      normal_sum+=elapsed_time; //elapsed times of between peaks are added to calculate its average

      if (ctr>19){ 
        elapsed_times[ctr-20]=elapsed_time; //storing the last 10 values in the elapsed_times array such that the array contains real time values instead of random values 
      }

      ctr+=1; //counter increment
      t.reset(); //the timer is reset so that the next elapsed time between the next consecutive peaks can be recorded accurately
      c.reset();
      thread_sleep_for(440); //A delay of 440 ms is given to avoid detection of erroneous peaks (double peaks or accidental peaks due to noise)
    }
  }
 
  normal_sum=normal_sum/30; //Averaged Elapsed time between 2 consecutive peaks for 30 peaks
  normal_bpm = (int)60*1000*(1/normal_sum); //Taking inverse of Elapsed time for getting frequency of the signal and multiplying it by 60 (60 seconds in a minute) and 1000 (millisecond to second)
  printf("Average Normal BPM: %f \n",normal_bpm);

  
  
  float sum=0.0;
  t.reset(); // Timer t is reset for calculating bpm in while loop below
  c.reset();

  while(1){
    read_value=ain.read();
    
    corner_case_time=c.read_ms(); // The value from timer c is read and stored in the variable corner_case_time
    if (corner_case_time>3000) //If there is no peak detection for 3 seconds, it means that the sensor is not placed properly/misplaced (removed from the finger)
    {
      printf("Sensor is not in place (disconnected)");
    }

    if(read_value >=0.85)
    {
      
        elapsed_time=t.read_ms();

        for (int i=0;i<9;i++)
        {
          elapsed_times[i]=elapsed_times[i+1]; // Moving the elements in the elapsed_times array by one place to the left with each peak detection such that newer values are stored (Moving average)
          sum=sum+elapsed_times[i]; //summed for average calculation
        }

        elapsed_times[9]=elapsed_time; //Newest elapsed time is stored in the last element of the array
        sum+=elapsed_time;
        sum=sum/10; //average
        printf("elapsed time: %f ", sum );

        f=1/(sum); //frequency
        bpm=60*1000*f; //frequency to bpm conversion
        bpm = (int)bpm;
        printf("bpm: %f \n",bpm);

        sum=0.0; //resetting sum for next average

        if (bpm<=(normal_bpm-10)) //if the calculated bpm is 10 less than the current enviroment bpm under normal breathing, the LEDs are toggled which indicates that the person has stopped breathing (for around 10 seconds)
        {
          printf("Not Breathing \n");
          led2=led1;
          led1=!led1;
          HAL_Delay(5000);
          break; //LEDs are toggled and the loop is exited when the person stops breathing
        }

        t.reset();
        c.reset();//c is reset with peak detection because the sensor is placed right currently
        thread_sleep_for(440);


    }

  
  }
  
}