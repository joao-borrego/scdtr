/** 
 * @file cli.ino
 * @brief Simple command-line interface over Serial example sketch
 * @author António Almeida
 * @author João Borrego
 */

/** Baudrate value for Serial communication */ 
#define BAUDRATE 115200
/** Size of buffer for Serial communication */
#define BUFFER_SIZE 30

/* Global variables */

/** Reference lux value */
float reference {0};
/** PWM output to LED */
float output {0};

char cmd_buffer[BUFFER_SIZE];

/* Pinout */

/* @brief LDR analog in pin */
int pin_ldr = A0;

void setup() {
    /* Setup serial communication */
    Serial.begin(BAUDRATE);
    /* Setup timer interrupt */
    setupTimerInt();
}

void loop() {
    // Low CPU usage
    delay(1000000000);
}

bool readCommand(){

}

/**
 * @brief      Setup timer interrupts
 * 
 * @note       The body of this funtion was generated automatically at
 *             <a href="http://www.8bit-era.cz/arduino-timer-interrupts-calculator.html">
 *             Arduino Timer Interrupt Calculator</a> 
 */
void setupTimerInt(){
    // TIMER 1 for interrupt frequency 1 Hz:
    cli(); // stop interrupts
    TCCR1A = 0; // set entire TCCR1A register to 0
    TCCR1B = 0; // same for TCCR1B
    TCNT1  = 0; // initialize counter value to 0
    // set compare match register for 1 Hz increments
    OCR1A = 62499; // = 16000000 / (256 * 1) - 1 (must be <65536)
    // turn on CTC mode
    TCCR1B |= (1 << WGM12);
    // Set CS12, CS11 and CS10 bits for 256 prescaler
    TCCR1B |= (1 << CS12) | (0 << CS11) | (0 << CS10);
    // enable timer compare interrupt
    TIMSK1 |= (1 << OCIE1A);
    sei(); // allow interrupts
}

/**
 * @brief      Timer1 interrupt callback function
 *
 * @param[in]  <unnamed>    Timer/Counter1 Compare Match A
 */
ISR(TIMER1_COMPA_vect){
   Serial.println(++reference);
}