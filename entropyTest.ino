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
 *    - Set the myName[] and comment[] arrays to include your name and  *
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

#include <Entropy.h>            //http://code.google.com/p/avr-hardware-random-number-generation/
#include <SD.h>

#define grnLED 7                //heartbeat LED
#define redLED 9                //sketch stopped/error indication
#define BUTTON 5                //start/stop button
#define CS_SD 4                 //chip select pin for the SD card
#define CS_W5100 10             //chip select pin for the W5100 Ethernet chip if using Ethernet shield
#define LOG_FILENAME_BASE "log" //base name for the log file. the sketch appends a number in the format "_nnn"
                                //to this base name. since the SD library only deals with "8.3" names, the base
                                //name can be no longer than four characters. the sketch will check the SD card
                                //before creating the log file and will increment "_nnn" as needed to avoid
                                //over-writing prior files that may already exist on the card.
#define LOG_FILENAME_EXT "txt"  //extension to use with the log file name. must be no longer than three characters.
#define LOG_CLOSE_INTERVAL 60   //close and re-open the log file after this many seconds (approx)
#define BLOCK_SIZE 8            //for efficiency, generate this many random numbers, then write to SD card

char myName[] = "Your name here";
char comment[] = "Describe board and other test environment details, e.g. chip part number, date code";
File logFile;                   //the file on the SD card
char logFilename[13];           //SD file name
unsigned long nBlocks;          //number of blocks of random numbers written to SD card
unsigned long ms;               //time from millis()
unsigned long lastClose;        //last time the log file was closed and reopened

void setup( void )
{
    Serial.begin( 115200 );
    Entropy.Initialize();
    logInitialize();
}

void loop( void )
{
    uint32_t r;                     //a random number
    String logData = "";            //block of random numbers to write to the SD card

    ms = millis();
    Serial.print(F("---- BLOCK "));
    Serial.print(nBlocks + 1, DEC);
    Serial.println(F(" ----"));
    for (uint8_t i = 0; i < BLOCK_SIZE; i++) {
        if (digitalRead(BUTTON) == LOW) logClose();
        r = Entropy.random();
        logData += String(r);
        logData += "\n";
        digitalWrite( grnLED, !digitalRead( grnLED ) );
        Serial.println(r, DEC);
    }
    logFile.print(logData);
    ++nBlocks;

    if (ms - lastClose >= LOG_CLOSE_INTERVAL * 1000UL) {
        lastClose = ms;
        Serial.print(F("Closing log file after "));
        Serial.print(nBlocks, DEC);
        Serial.println(F(" blocks"));
        logFile.flush();
        logFile.close();
        logOpen();
  }      
}

void logInitialize(void)
{
    String msg = "";
    
    pinMode(grnLED, OUTPUT);
    pinMode(redLED, OUTPUT);
    pinMode(BUTTON, INPUT);
    pinMode(CS_W5100, OUTPUT);                    //for Arduino Ethernet shield
    digitalWrite( BUTTON, HIGH);                  //enable internal pullup resistor
    digitalWrite( CS_W5100, HIGH);                //deselect the W5100

    Serial.println(F("Waiting for start button"));
    digitalWrite(grnLED, HIGH);
    while (digitalRead(BUTTON) == HIGH) {}         //wait for the button to be pushed to start
    flashLED(grnLED);

    if (!SD.begin(CS_SD)) {
        Serial.println(F("SD.begin FAIL"));
        while (1) flashLED(redLED);                //error, go no further
    }
    Serial.println(F("SD.begin OK"));
    logOpen();

    //write a header to the file
    msg += "**Entropy Library Test\n";
    msg += __FILE__;
    msg += "\nCompiled: ";
    msg += __DATE__;
    msg += " ";
    msg += __TIME__;
    msg += "\nFile: ";
    msg += logFilename;
    msg += "\nName: ";
    msg += myName;
    msg += "\nComment: ";
    msg += comment;
    msg +="\n";
    
    logFile.print(msg);
    Serial.print(msg);
}

//create a new log file and open it
void logOpen(void)
{
    static boolean first = true;
    
    if (first) {
        getFilename();
        first = false;
    }
    logFile = SD.open(logFilename, FILE_WRITE);    //open the log file
    if (logFile) {
        Serial.print(F("Log file opened: "));
        Serial.println(logFilename);
    }
    else {
        Serial.print(F("Could not open log file: "));
        Serial.println(logFilename);
        while (1) flashLED(redLED);                //error, go no further
    }
}
    
//close the log file
void logClose(void)
{
    String msg = "";
    
    msg += "**Log closed, ";
    msg += String(nBlocks);
    msg += " blocks, ";
    msg += String(nBlocks * BLOCK_SIZE);
    msg += " random numbers.";
    msg += "\n";

    logFile.print(msg);
    logFile.flush();
    logFile.close();
    flashLED(redLED);
    digitalWrite(grnLED, LOW);
    digitalWrite(redLED, HIGH);
    Serial.print(msg);
    Serial.println(F("**Sketch halted."));
    Serial.println();
    while (1);                                     //that's all, folks!      
}

//acknowledge button press or indicate error condition
void flashLED(int pin)
{
    for (uint8_t i=0; i<4; i++) {
        digitalWrite(pin, HIGH);
        delay(100);
        digitalWrite(pin, LOW);
        delay(100);
    }
}

//generate the next sequential log filename that does not already exist on the SD card
void getFilename(void)
{
    static int fileNumber;
    char filename[13], cFileNumber[8];
    
    do
    {
        strcpy(logFilename, LOG_FILENAME_BASE);
        strcat(logFilename, "_");
        if (fileNumber<100) strcat(logFilename, "0");    //leading zeroes
        if (fileNumber<10) strcat(logFilename, "0");
        itoa(fileNumber++, cFileNumber, 10);
        strcat(logFilename, cFileNumber);
        strcat(logFilename, ".");
        strcat(logFilename, LOG_FILENAME_EXT);
    }
    while (SD.exists(logFilename));
}

