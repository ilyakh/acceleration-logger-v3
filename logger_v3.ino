#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "FlexiTimer2.h"

MPU6050 mpu(0x69);

VectorInt16 acceleration;
VectorInt16 rotation;


int FREQUENCY_DELAY = 10;
int LOGGER_BAUDRATE = 115200;



void setup() {
    
    Wire.begin();
    TWBR = 12; // change I2C to 400 mHz
    
    Serial2.begin( 115200 );
    mpu.initialize();

        
    mpu.setRate( FREQUENCY_DELAY );
    mpu.setFullScaleGyroRange( MPU6050_GYRO_FS_500 );
    mpu.setFullScaleAccelRange( MPU6050_ACCEL_FS_8 );
    
    FlexiTimer2::set( FREQUENCY_DELAY, store );
    FlexiTimer2::start();
    
}

void store() {
  
  Serial2.print( millis() );
  Serial2.print( "," );
  
  Serial2.print( acceleration.x );
  Serial2.print( "," );
  Serial2.print( acceleration.y );
  Serial2.print( "," );
  Serial2.print( acceleration.z );
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
  
    mpu.getAcceleration( &acceleration.x, &acceleration.y, &acceleration.z );
    mpu.getRotation( &rotation.x, &rotation.y, &rotation.z );
    
}
