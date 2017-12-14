/**
 * @file main.ino
 * @brief Multiple board system main application
 * @author António Almeida
 * @author João Borrego
 */

#include <EEPROM.h>
#include <Wire.h>

#include "constants.hpp"
#include "utils.hpp"
#include "pi.hpp"
#include "communication.hpp"
#include "calibration.hpp"
#include "consensus.hpp"

/* Global variables */

/** Device id */
uint8_t id{0};

/* System calibration */

/** Row i of K parameter matrix */
float k_i[N]    {0.0};
/** LUX value for complete darkness */
float o_i       {0.0};

/* System I/O */

/** LDR input ADC value */
volatile float ldr_in       {0.0};
/** LDR input in Volt */
volatile float v_in         {0.0};
/** LDR input converted to LUX units */
volatile float lux          {0.0};
/** LED duty cycle */
volatile float out          {0.0};

/* Real-time control */

/** Lux lower bound */
volatile float lower_bound  {0.0};
/** Lux reference */
volatile float ref          {50.0};
/** Occupancy */
volatile bool occupancy     {false};

/** Proportional Integrator controller */
PIController::Controller controller(
  &lux, &out, &ref, K_P, K_I, T);

/** Whether to use feedforward for initial estimate */
bool use_feedforward {true};

/** Latest elapsed milliseconds since startup */
unsigned long current_millis    {0};
/** Previously recorded elapsed milliseconds since startup */
unsigned long last_millis       {0};

/* Simple state machine */

/** Current node state */
volatile int state {CONTROL};

/**
 * @brief      Arduino setup
 */
void setup() {

    // Setup serial communication
    Serial.begin(BAUDRATE);

    // Read device ID from EEPROM
    id = EEPROM.read(ID_ADDR);
    Serial.print("[I2C] Registered with id ");
    Serial.println((int) id);

    // Setup I2C communication
    Communication::setDeviceId(id);
    Wire.begin(id);

    // Setup timer interrupt
    setupTimerInt();

    // Configure controller features
    controller.configureFeatures(use_feedforward, true, true);
    /*
    // Determine K matrix and external illuminance
    Wire.onReceive(Calibration::onReceive);
    Wire.onRequest(Calibration::onRequest);
    Calibration::execute(k_i, &o_i, id);
    // Setup I2C communication for main loop
    Wire.onReceive(Communication::onReceive);
    */

}

/**
 * @brief      Arduino loop
 */
void loop() {

    // Packets have to be acknowledged in order to be sniffed:
    // Each device acks a single packet

    /*
    Communication::sendInfo((id + 1) % N,
        lux, out / 255.0, lower_bound, o_i, ref, occupancy);
    */

     current_millis = millis();
    if (current_millis - last_millis >= STATUS_DELAY){
        last_millis = current_millis;
        
        Communication::sendInfo((id + 1) % N,
        lux, out / 255.0, 30.0, 20.0, 50.0, id == 0);
        
        // DEBUG
        Serial.print(ref);
        Serial.print("\t");
        Serial.print((floor(out)) / 255.0 );
        Serial.print("\t");
        Serial.print(lux);
        Serial.println(current_millis);
    }

    // if start consensus

    /*
    Wire.onReceive(Communication::nop);

    float k_i_tmp[N][N] = {{2.0, 1.0}, {1.0, 2.0}};
    int d_best;
    if (id == 0){
        d_best = Consensus::solve(id, 150.0, k_i_tmp[0], 30.0);
    }else{
        d_best = Consensus::solve(id, 80.0,  k_i_tmp[1],  0.0);
    }
    Serial.println(d_best);
    */
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
    out = (out >= 0)?   out : 255;
    out = (out <= 255)? out : 0;
    analogWrite(pin_led, (int) out);
}

/**
 * @brief      Timer1 interrupt callback function
 *
 * @param[in]  <unnamed>    Timer/Counter1 Compare Match A
 */
ISR(TIMER1_COMPA_vect){
    
    if (state == CONTROL){
        
        // Sample input
        ldr_in =  analogRead(pin_ldr);
        v_in = ldr_in * (VCC / 1023.0);
        lux = Utils::convertToLux(v_in, LUX_A[id], LUX_B[id]);
    
        // Control system
        controller.update(writeToLed);

    } else if (state == CALIBRATION) {
        // Ignore interrupt
    }
}
