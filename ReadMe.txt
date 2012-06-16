/*----------------------------------------------------------------------*
 * Test sketch for the Entropy random number library at                 *
 * http://code.google.com/p/avr-hardware-random-number-generation/      *
 * Data is captured to an SD card.                                      *
 *                                                                      *
 * Tested with Adafruit Industries MicroSD card breakout board no. 254, *
 * https://www.adafruit.com/products/254 but should also work with      *
 * the SD card on the Arduino Ethernet Shield.                          *
 *                                                                      *
 * Connect the following (use appropriate current-limiting              *
 * resistors with the LEDs):                                            *
 *     - Tactile button switch from pin 5 to ground (start/stop).       *
 *     - Red LED from pin 7 to ground (error/sketch halt).              *
 *     - Green LED from pin 9 to ground (heartbeat).                    *
 *                                                                      *
 * If using the Adafruit MicroSD breakout board, connect as follows:    *
 *     - 5V to the 5 volt supply                                        *
 *     - GND to ground                                                  *
 *     - CLK to Arduino pin 13                                          *
 *     - DO to Arduino pin 12                                           *
 *     - DI to Arduino pin 11                                           *
 *     - CS to Arduino pin 4                                            *
 *                                                                      *
 * Operation:                                                           *
 *    - When the sketch starts, it turns on the green LED and waits     *
 *      for the button to be pressed.                                   *
 *    - Press the button and the green LED will blink four times, then  *
 *      it will toggle for each random number generated.                *
 *    - Data will be captured to the SD card. The filename is           *
 *      automatically generated with a sequential number.               *
 *    - As the sketch runs, random numbers are written in blocks        *
 *      to the SD card (for I/O efficiency).                            *
 *    - To stop the sketch and close the log file, press and hold the   *
 *      button until the red LED blinks four times. The log file will   *
 *      then be closed, and the red LED will stay on to indicate that   *
 *      the sketch is halted. Any partial block of random numbers       *
 *      generated will be discarded and not written to the SD card.     *
 *    - If an error occurs, the red LED blinks rapidly and an           *
 *      error message is written to the serial monitor.                 *
 *    - Reset the MCU to restart the sketch after an error or halt.     *
 *    - Set the name[] and comment[] arrays to include your name and    *
 *      test details in the log file.                                   *
 *    - To guard against data loss due to power outage, etc., the       *
 *      log file is periodically closed and reopened.                   *
 *                                                                      *
 * Written by Jack Christensen 16Jun2012                                *
 *                                                                      *
 * This work is licensed under the Creative Commons Attribution-        *
 * ShareAlike 3.0 Unported License. To view a copy of this license,     *
 * visit http://creativecommons.org/licenses/by-sa/3.0/ or send a       *
 * letter to Creative Commons, 171 Second Street, Suite 300,            *
 * San Francisco, California, 94105, USA.                               *
 *----------------------------------------------------------------------*/ 
