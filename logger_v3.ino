#include "FlexiTimer2.h"
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
 

MPU6050 mpu( 0x69 );

VectorInt16 acceleration;
VectorInt16 rotation;





int FREQUENCY_DELAY = 50;

int WRITE_DELAY = 50;
int SAMPLE_DELAY = 10;

int LOGGER_BAUDRATE = 115200;

volatile int counter = 0;
volatile long sum[6];


enum Coordinates {
  ax,
  ay,
  az,
 
  rx,
  ry,
  rz 
};


void store() {

 
    Serial.print( millis() );
    Serial.print( "," );
    Serial.print( counter );
    Serial.print( "," );
    
    Serial.print( sum[ax] / counter );
    Serial.print( "," );
    Serial.print( sum[ay] / counter );
    Serial.print( "," );
    Serial.print( sum[az] / counter );
    Serial.print( "," );
    
    Serial.print( sum[rx] / counter );
    Serial.print( "," );
    Serial.print( sum[ry] / counter );
    Serial.print( "," );
    Serial.print( sum[rz] / counter );
    
    Serial.println();
    
    // reset
    sum[ax] = 0; sum[ay] = 0; sum[az] = 0;
    sum[rx] = 0; sum[ry] = 0; sum[rz] = 0;
    
    counter = 0; 
    
}



void setup() {
    
    Wire.begin();
    
    Serial.begin( 115200 );
    mpu.initialize();
        
    mpu.setRate( SAMPLE_DELAY );
    mpu.setFullScaleGyroRange( MPU6050_GYRO_FS_500 );
    mpu.setFullScaleAccelRange( MPU6050_ACCEL_FS_8 );
    
    FlexiTimer2::set( WRITE_DELAY, store );
    FlexiTimer2::start();
}


void loop() {
    mpu.getAcceleration( &acceleration.x, &acceleration.y, &acceleration.z );
    mpu.getRotation( &rotation.x, &rotation.y, &rotation.z );
    
    // update the sum of axes
    sum[ax] += acceleration.x;
    sum[ay] += acceleration.y;
    sum[az] += acceleration.z;
    
    sum[rx] += rotation.x;
    sum[ry] += rotation.y;
    sum[rz] += rotation.z;
    
    counter++;

}
