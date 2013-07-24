#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "FlexiTimer2.h"

MPU6050 mpu(0x69);



#define LED_PIN 13 // (Arduino is 13, Teensy is 11, Teensy++ is 6)
bool blinkState = false;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorInt16 rotation;
VectorFloat gravity;    // [x, y, z]            gravity vector

float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector





// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}



// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();
    
    Serial2.begin( 115200 );
    while (!Serial2); // wait for Leonardo enumeration, others continue immediately
    
    // initialize device
    Serial2.println(F("Initializing I2C devices..."));
    mpu.initialize();



    // verify connection
    Serial2.println(F("Testing device connections..."));
    Serial2.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

    // wait for ready
    /*
    Serial2.println(F("\nSend any character to begin DMP programming and demo: "));
    while (Serial2.available() && Serial2.read()); // empty buffer
    while (!Serial2.available());                 // wait for data
    while (Serial2.available() && Serial2.read()); // empty buffer again
    */

    // load and configure the DMP
    Serial2.println(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();
    
    
    
    // ================================================================
    // ===                      SETTINGS                            ===
    // ================================================================
        
    mpu.setRate( 5 );
    
    mpu.setFullScaleGyroRange( MPU6050_GYRO_FS_250 );
    mpu.setFullScaleAccelRange( MPU6050_ACCEL_FS_8 );
    
    // debug info
    Serial2.print( "Accelerometer sample rate: " );
    Serial2.println( mpu.getRate() );
    
    
    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        Serial2.println(F("Enabling DMP..."));
        mpu.setDMPEnabled( true );

        // enable Arduino interrupt detection
        Serial2.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        attachInterrupt(0, dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial2.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial2.print(F("DMP Initialization failed (code "));
        Serial2.print(devStatus);
        Serial2.println(F(")"));
        
        // OpenLog initialization
  
        Serial2.begin( 115200 );      
        
    }

    // configure LED for output
    pinMode(LED_PIN, OUTPUT);
}



// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
    // if programming failed, don't try to do anything
    if (!dmpReady) return;

    // wait for MPU interrupt or extra packet(s) available
    while (!mpuInterrupt && fifoCount < packetSize) {
        // other program behavior stuff here
        // .
        // .
        // .
        // if you are really paranoid you can frequently test in between other
        // stuff to see if mpuInterrupt is true, and if so, "break;" from the
        // while() loop to immediately process the MPU data
        // .
        // .
        // .
        
        // read and save dmp sensor values
        mpu.dmpGetAccel( &aa, fifoBuffer );
        mpu.dmpGetQuaternion( &q, fifoBuffer );
        mpu.dmpGetYawPitchRoll( ypr, &q, &gravity );
        
        // read and save the raw sensor values
        // mpu.getAcceleration( &aa.x, &aa.y, &aa.z );
        // mpu.getRotation( &rotation.x, &rotation.y, &rotation.z );        
        
        
        Serial2.print( millis() );
        Serial2.print( "," );
        Serial2.print( aa.x );
        Serial2.print( "," );
        Serial2.print( aa.y );
        Serial2.print( "," );
        Serial2.print( aa.z );
        
        Serial2.print( "," );
        Serial2.print( ypr[0] );
        Serial2.print( "," );
        Serial2.print( ypr[1] );
        Serial2.print( "," );
        Serial2.print( ypr[2] );
        
        
        // end record
        Serial2.println();
        
        
    }

    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // get current FIFO count
    fifoCount = mpu.getFIFOCount();

    // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        // reset so we can continue cleanly
        mpu.resetFIFO();
        Serial2.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } else if (mpuIntStatus & 0x02) {
        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

        // read a packet from FIFO
        mpu.getFIFOBytes(fifoBuffer, packetSize);
        
        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        fifoCount -= packetSize;
        
        

        

        
        

        // blink LED to indicate activity
        blinkState = !blinkState;
        digitalWrite(LED_PIN, blinkState);
    }
}
