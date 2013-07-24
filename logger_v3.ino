#include "FlexiTimer2.h"
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
 

MPU6050 mpu( 0x69 );

VectorInt16 acceleration;
VectorInt16 rotation;



int SENSOR_DELAY = 5; // 200 Hz
int WRITE_DELAY = 10; // 100 Hz

int LOGGER_BAUDRATE = 921600;

volatile int counter = 0;
volatile long sum[6];


enum Coordinates {
  ax, ay, az,
  rx, ry, rz 
};



void setup() {
    
    Wire.begin();
    
    mpu.initialize();
    
    mpu.setRate( SENSOR_DELAY );
    mpu.setFullScaleGyroRange( MPU6050_GYRO_FS_500 );
    mpu.setFullScaleAccelRange( MPU6050_ACCEL_FS_8 );
        
    FlexiTimer2::set( WRITE_DELAY, store );
    Serial2.begin( LOGGER_BAUDRATE );
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

void store() {
 
    Serial2.print( millis() );
    Serial2.print( "," );
    Serial2.print( counter );
    Serial2.print( "," );
    
    Serial2.print( sum[ax] / counter );
    Serial2.print( "," );
    Serial2.print( sum[ay] / counter );
    Serial2.print( "," );
    Serial2.print( sum[az] / counter );
    Serial2.print( "," );
    
    Serial2.print( sum[rx] / counter );
    Serial2.print( "," );
    Serial2.print( sum[ry] / counter );
    Serial2.print( "," );
    Serial2.print( sum[rz] / counter );
    
    Serial2.println();

    
    // reset
    sum[ax] = 0; sum[ay] = 0; sum[az] = 0;
    sum[rx] = 0; sum[ry] = 0; sum[rz] = 0;
    
    counter = 0; 
    
}
