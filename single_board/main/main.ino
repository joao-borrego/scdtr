/** 
 * @file single_board/main/main.ino
 *
 * @brief Single board system main application
 *
 * @author João Borrego
 * @author António Almeida
 */

#include "utils.hpp"
#include "pi.hpp"

/** Baudrate value for Serial communication */ 
const long BAUDRATE = 115200;
/** Size of buffer for Serial communication */
const unsigned int BUFFER_SIZE = 30;

/** Minimum value for the reference */
const float MIN_REF = 0.0;
/** Maximum value for the reference */
const float MAX_REF = 150.0;
/** Minimum value for the output */
const unsigned int MIN_OUT = 0;
/** Maximum value for the output */
const unsigned int MAX_OUT = 255;

/** Minimum interval between status Serial prints (ms) */
const unsigned long STATUS_DELAY = 100;

/* PI controller default parameters */

/** Proportional term coefficient */
const float K_P = 2.051;   // 0.40291;  // 3.0622;
/** Integral term coefficient */
const float K_I = 136.7462; // 26.8604; // 204.1445
/** Sampling time (s) */
const float T   = 0.030;

/* System properties */

/** Aproximate maximum measure for input */
const float MAX_LUX  = 100.0;
/** Value for low illuminance setting */
const float LOW_LUX  = (1.0 / 3.0) * MAX_LUX;
/** Value for high illuminance setting */
const float HIGH_LUX = (2.0 / 3.0) * MAX_LUX;

/* Pinout */

/** LDR analog in pin */
const int pin_ldr = A0;
/** LED PWM out pin */
const int pin_led = 11;

/* Global variables */

/** Reference lux value */
volatile float reference {0};
/** LDR input ADC value */
volatile float ldr_in {0};
/** LDR input converted to LUX units */
volatile float input {0};
/** PWM output to LED */
volatile float output {0};
/** LDR input tension */
volatile float v_in {0}; 

/** Command line buffer */
char cmd_buffer[BUFFER_SIZE] {0};

/** Proportional Integrator controller */
PIController::Controller controller(
  &input,
  &output,
  &reference,
  K_P, //0.4507,
  K_I, //0.1,
  T);

/** Whether to use feedforward for initial estimate */
bool use_feedforward {true};
/** Latest elapsed milliseconds since startup */
unsigned long current_millis {0};
/** Previously recorded elapsed milliseconds since startup */
unsigned long last_millis {0};

// Aux triggers for testing
bool triggered_1 {false};
bool triggered_2 {false};

/**
 * @brief      Arduino setup
 */
void setup() {
    /* Setup serial communication */
    Serial.begin(BAUDRATE);
    /* Setup timer interrupt */
    setupTimerInt();
    /* Configure controller features */
    controller.configureFeatures(use_feedforward, true, true);
    /* Setup initial reference */
    reference = LOW_LUX;
}

/**
 * @brief      Arduino loop
 */
void loop() {
    
    /* Read commands from Serial */
    if (readLine()){
        processCommand();    
    }

    /* Print status every STATUS_DELAY milliseconds */
    current_millis = millis();
    if (current_millis - last_millis >= STATUS_DELAY){
        last_millis = current_millis;

        //if (current_millis >= 4000 && current_millis <= 6000) { listVariables(); }
        listVariables();
    }

    /*
    // Feedforward experiment
    if (millis() >= 5000 && !triggered_1) {
        triggered_1 = true;
        reference = HIGH_LUX;
    }
    */

    /*
    // Anti-windup experiment
    if (millis() >= 5000 && !triggered_1) {
        triggered_1 = true;
        reference = 100;
    }
    if (millis() >= 7000 && !triggered_2) {
        triggered_2 = true;
        reference = LOW_LUX;
    }
    */

    /*
    // Deadzone experiment
    if (millis() >= 5000 && !triggered_1) {
        triggered_1 = true;
        reference = 50;
    }
    */
}

/**
 * @brief      Reads a line from Serial
 *
 * @return     Whether a line was read
 */
bool readLine(){

    static uint8_t offset = 0;

    while (Serial.available()){
        char cur = Serial.read();
        switch (cur) {
            // Carriage return
            case '\r':
            // Line feed
            case '\n':
                // Terminate string
                cmd_buffer[offset] = '\0';
                if (offset > 0){
                    offset = 0;
                    //printCommand();
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
    char *param = strtok(NULL, " \n");

    if (!strcmp(command, "reference") && param){     
        float new_reference = atof(param);
        if (new_reference > MIN_REF && new_reference < MAX_REF){    
            reference = new_reference;
        } else if (!strcmp(param, "low")){
            reference = LOW_LUX;
        } else if (!strcmp(param, "high")){
            reference  = HIGH_LUX;
        } else if (!strcmp(param, "off")){
            reference = 0;
        }
    } else if (!strcmp(command, "feedforward") && param){
        if (!strcmp(param, "on")){
            use_feedforward = true;
            controller.useFeedforward(true);
        } else if (!strcmp(param, "off")){
            use_feedforward = false;
            controller.useFeedforward(false);
        }
    } else if (!strcmp(command, "deadzone") && param){
        if (!strcmp(param, "on")){
            controller.useErrorDeadzone(true);
        } else if (!strcmp(param, "off")){
            controller.useErrorDeadzone(false);
        }
    } else if (!strcmp(command, "anti_windup") && param){
        if (!strcmp(param, "on")){
            controller.useAntiWindup(true);
        } else if (!strcmp(param, "off")){
            controller.useAntiWindup(false);
        }
    } else {
        Serial.println("Invalid command!");
    }
}

/**
 * @brief      Lists important variables to Serial
 */
void listVariables(){
    Serial.print(reference);
    Serial.print("\t");
    Serial.print(( floor(output) / 255.0));
    Serial.print("\t");
    Serial.print(input);
    Serial.print("\t");
    Serial.print(int(use_feedforward));
    Serial.print("\t");
    Serial.println(current_millis);
}

/**
 * @brief      Sets up timer interrupts
 * 
 * @note       The body of this funtion was generated automatically at
 *             <a href="http://www.8bit-era.cz/arduino-timer-interrupts-calculator.html">
 *             Arduino Timer Interrupt Calculator</a> 
 */
void setupTimerInt(){
    // TIMER 1 for interrupt frequency 100/3 (33.(3)) Hz:
    cli(); // stop interrupts
    TCCR1A = 0; // set entire TCCR1A register to 0
    TCCR1B = 0; // same for TCCR1B
    TCNT1  = 0; // initialize counter value to 0
    // set compare match register for 100 / 3 Hz increments
    OCR1A = 60000; // = 16000000 / (8 * (100 / 3)) - 1 (must be <65536)
    // turn on CTC mode
    TCCR1B |= (1 << WGM12);
    // Set CS12, CS11 and CS10 bits for 8 prescaler
    TCCR1B |= (0 << CS12) | (1 << CS11) | (0 << CS10);
    // enable timer compare interrupt
    TIMSK1 |= (1 << OCIE1A);
    sei(); // allow interrupts
}

/**
 * @brief      Writes to led.
 */
void writeToLed(){
    /* Constrain output to 8 bits */
    output = (output >= MIN_OUT)? output : MIN_OUT;
    output = (output <= MAX_OUT)? output : MAX_OUT;
    analogWrite(pin_led, (int) output);
}

/**
 * @brief      Timer1 interrupt callback function
 *
 * @param[in]  <unnamed>    Timer/Counter1 Compare Match A
 */
ISR(TIMER1_COMPA_vect){
    /* Sample input */
    ldr_in =  analogRead(pin_ldr);
    v_in = ldr_in * (5.0 / 1023.0);
    input = Utils::convertToLux(v_in);
    /* Control system */
    controller.update(writeToLed);
}
