#include "FlexiTimer2.h"
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "EEPROM.h"

MPU6050 mpu( 0x69 );

VectorInt16 acceleration;
VectorInt16 rotation;

int axMax, axMin, ayMax, ayMin, azMax, azMin;
int axOffset, ayOffset, azOffset;

int SENSOR_DELAY = 5; // 200 Hz
int WRITE_DELAY = 10; // 100 Hz

#define LOGGER_BAUDRATE 115200

volatile int counter = 0;
volatile long sum[6];

enum Coordinates {
  ax, ay, az,
  rx, ry, rz 
};



void setup() {
    Wire.begin();
    Serial.begin( LOGGER_BAUDRATE );
    
    mpu.initialize();
    
    mpu.setRate( SENSOR_DELAY );
    mpu.setFullScaleGyroRange( MPU6050_GYRO_FS_500 );
    mpu.setFullScaleAccelRange( MPU6050_ACCEL_FS_8 );
    
    delay(100);
    readEEPROM();
    delay(100);
    
    axOffset = 0;
    ayOffset = 0;
    azOffset = 0;
    
    
    FlexiTimer2::set( WRITE_DELAY, store );
    FlexiTimer2::start();
        
}



void loop() {
    
    mpu.getAcceleration( &acceleration.x, &acceleration.y, &acceleration.z );
    mpu.getRotation( &rotation.x, &rotation.y, &rotation.z );
    
    // update the sum of axes
    sum[ax] += (acceleration.x-axOffset);
    sum[ay] += (acceleration.y-ayOffset);
    sum[az] += (acceleration.z-azOffset);
    
    sum[rx] += rotation.x;
    sum[ry] += rotation.y;
    sum[rz] += rotation.z;
    
    counter++;

}

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

void readEEPROM(){
  
  // Serial.println("\nReading EEPROM... ");
  axMin = (EEPROM.read(0) * 256) + EEPROM.read(1) - 32768;
  axMax = (EEPROM.read(2) * 256) + EEPROM.read(3) - 32768;
  ayMin = (EEPROM.read(4) * 256) + EEPROM.read(5) - 32768;
  ayMax = (EEPROM.read(6) * 256) + EEPROM.read(7) - 32768;
  azMin = (EEPROM.read(8) * 256) + EEPROM.read(9) - 32768;
  azMax = (EEPROM.read(10) * 256) + EEPROM.read(11) - 32768;
  axOffset = (int)(axMax + axMin)/2;
  ayOffset = (int)(ayMax + ayMin)/2;
  azOffset = (int)(azMax + azMin)/2;
  
}

