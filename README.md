# Embedded-Systems-NYU

This repository contains the project "Just Breathe" as part of the Fall 22 Embedded challenge from the "Real-Time Embedded Systems" course by Prof Matthew Campisi. 

The goal of the project is to use a pulse sensor and STM32F429 microcontroller to
detect lack of breathing for more than 10s and to activate a trigger ( Toggling LEDs ) to inform the
patient/caregiver.

The key steps of the algorithm include:
1. We first calibrate the person's BPM by estimating the total time for 30 peaks of pulses ( when read voltage > 0.85 )
2. Then the monitored BPM is calculated continously by using moving average of elapsed time and when it drops below the calibrated threshold, LEDs are toggled to activate trigger. 
