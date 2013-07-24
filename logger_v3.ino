#include "MsTimer2.h"
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
 

MPU6050 mpu(0x69);

VectorInt16 acceleration;
VectorInt16 rotation;


int FREQUENCY_DELAY = 500;
int LOGGER_BAUDRATE = 115200;



void store() {

  mpu.getAcceleration( &acceleration.x, &acceleration.y, &acceleration.z );
  mpu.getRotation( &rotation.x, &rotation.y, &rotation.z );   
  
  Serial.print( millis() );
  Serial.print( "," );
  
  Serial.print( acceleration.x );
  Serial.print( "," );
  Serial.print( acceleration.y );
  Serial.print( "," );
  Serial.print( acceleration.z );
  Serial.print( "," );
  
  Serial.print( rotation.x );
  Serial.print( "," );
  Serial.print( rotation.y );
  Serial.print( "," );
  Serial.print( rotation.z );   
  
  // end record
  Serial.println();
  
}



void setup() {
    
    Wire.begin();
    
    Serial.begin( 115200 );
    mpu.initialize();
        
    mpu.setRate( FREQUENCY_DELAY );
    mpu.setFullScaleGyroRange( MPU6050_GYRO_FS_500 );
    mpu.setFullScaleAccelRange( MPU6050_ACCEL_FS_8 );
    
    
    MsTimer2::set( FREQUENCY_DELAY, store );
    MsTimer2::start();
    
}


void loop() {
 Serial.print(".");
}
