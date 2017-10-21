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

/** Minimum value for the reference */
#define MIN_REF 0
/** Maximum value for the reference */
#define MAX_REF 1000
/** Minimum value for the output */
#define MIN_OUT 0
/** Maximum value for the output */
#define MAX_OUT 255


/* Global variables */

/** Reference lux value */
float reference {0};
/** PWM output to LED */
float output {0};

char cmd_buffer[BUFFER_SIZE];

volatile int cycles = 0;

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
    
    if(readLine()){
        processCommand();    
    }
    // Lower CPU usage
    delayMicroseconds(1000000000000000);
}

/**
 * @brief      Reads a line from Serial
 *
 * @return     Whether a line was read
 */
bool readLine(){

    static uint8_t offset = 0;

    while(Serial.available()){
        char cur = Serial.read();
        switch(cur) {
            // Carriage return
            case '\r':
            // Line feed
            case '\n':
                // Terminate string
                cmd_buffer[offset] = '\0';
                if (offset > 0){
                    offset = 0;
                    printCommand();
                    return true;
                }
                break;
            default:
                if (offset < BUFFER_SIZE - 1){
                    cmd_buffer[offset] = cur;
                    offset++;
                    cmd_buffer[offset] = '\0';
                }
        }   
    }
    return false;
}

/**
 * @brief      Prints a command to Serial
 */
void printCommand(){
    Serial.print("> ");
    Serial.println(cmd_buffer);
}

/**
 * @brief      Processes a command read from Serial
 */
void processCommand(){

    char *command = strtok(cmd_buffer, " \n");

    if (!strcmp(command, "list")){
        listVariables(); 
    } else if (!strcmp(command, "set")){
        char *params = strtok(NULL, " \n");
        if (!strcmp(params, "reference")){
            char *value_str = strtok(NULL, " \n");
            if (value_str){
                float new_reference = atof(value_str);
                if (new_reference > MIN_REF && new_reference < MAX_REF){
                    reference = new_reference;
                    Serial.print("Reference set to ");
                    Serial.println(reference);
                    return;
                }
            }
            Serial.println("Invalid value provided!");
        } else if (!strcmp(params, "output")){
            char *value_str = strtok(NULL, " \n");
            if (value_str){
                float new_output = atof(value_str);
                if (new_output > MIN_OUT && new_output < MAX_OUT){
                    output = new_output;
                    Serial.print("Output set to ");
                    Serial.println(output);
                    return;
                }
            }
            Serial.println("Invalid value provided!");
        } else {
            Serial.println("Invalid variable!");
        }
    } else {
        Serial.println("Command not found!");
    }
}

/**
 * @brief      Lists important variables to Serial
 */
void listVariables(){
    Serial.print("Reference is set to ");
    Serial.println(reference);
    Serial.print("Output is set to ");
    Serial.println(output);
    Serial.print("[DEBUG] Seconds since launch: ");
    Serial.println(cycles);
}

/**
 * @brief      Sets up timer interrupts
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
   cycles++;
}