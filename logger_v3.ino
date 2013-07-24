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
VectorInt16 rotation;
// VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
// VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
// VectorFloat gravity;    // [x, y, z]            gravity vector
// float euler[3];         // [psi, theta, phi]    Euler angle container
// float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector





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
    
    Serial2.begin( 345600 );
    while (!Serial2); // wait for Leonardo enumeration, others continue immediately
    
    // initialize device
    Serial2.println(F("Initializing I2C devices..."));
    mpu.initialize();



    // verify connection
    Serial2.println(F("Testing device connections..."));
    Serial2.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

    // load and configure the DMP
    Serial2.println(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();
    
    
    
    // ================================================================
    // ===                      SETTINGS                            ===
    // ================================================================
        
    mpu.setRate( 20 );
    
    mpu.setFullScaleGyroRange( MPU6050_GYRO_FS_250 );
    mpu.setFullScaleAccelRange( MPU6050_ACCEL_FS_8 );
    
    Serial2.begin( 345600 ); 
    
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
        Serial2.print( F("DMP Initialization failed (code ") );
        Serial2.print( devStatus );
        Serial2.println( F(")") );           
        
    }
    
    
    FlexiTimer2::set( 20, readSensors );
    FlexiTimer2::start();

    // configure LED for output
    pinMode(LED_PIN, OUTPUT);
}



void readSensors() {

  
  Serial2.print( millis() );
  Serial2.print( "," );
  
  Serial2.print( aa.x );
  Serial2.print( "," );
  Serial2.print( aa.y );
  Serial2.print( "," );
  Serial2.print( aa.z );
  Serial2.print( "," );
  
  Serial2.print( rotation.x );
  Serial2.print( "," );
  Serial2.print( rotation.y );
  Serial2.print( "," );
  Serial2.print( rotation.z );   
  
  // end record
  Serial2.println();
  
  
}

void loop() {
  
    mpu.getAcceleration( &aa.x, &aa.y, &aa.z );
    mpu.getRotation( &rotation.x, &rotation.y, &rotation.z );
    
}
