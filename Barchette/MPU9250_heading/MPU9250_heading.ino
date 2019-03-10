#include <quaternionFilters.h>
#include <MPU9250.h>

#define MPU9260_INT_PIN 2

MPU9250 myIMU(MPU9250_ADDRESS_AD0, Wire, 400000);

float heading, headingDegrees, headingFiltered;

void setup() {
  Wire.begin();
  Serial.begin(38400);

  pinMode(MPU9260_INT_PIN, INPUT);
  digitalWrite(MPU9260_INT_PIN, LOW);

  //test comunicazione
  uint8_t c = myIMU.readByte(MPU9250_ADDRESS_AD0, WHO_AM_I_MPU9250);
  Serial.print(F("MPU9250 I AM 0x"));
  Serial.println(c, HEX);

  //self test and calibration
  myIMU.MPU9250SelfTest(myIMU.selfTest);
  myIMU.calibrateMPU9250(myIMU.gyroBias, myIMU.accelBias);

  //inizializzo l'unità
  myIMU.initMPU9250();

  //test comunicazione con la bussola
  byte d = myIMU.readByte(AK8963_ADDRESS, WHO_AM_I_AK8963);
  Serial.print("AK8963 I AM 0x");
  Serial.println(d, HEX);

  //preparo la bussola
  myIMU.initAK8963(myIMU.factoryMagCalibration);
  //Get sensor resolutions, only need to do this once
  myIMU.getAres();
  myIMU.getGres();
  myIMU.getMres();
}

void loop() {
  if(myIMU.readByte(MPU9250_ADDRESS_AD0, INT_STATUS) & 0x01) {
    myIMU.readAccelData(myIMU.accelCount);
    myIMU.ax = (float)myIMU.accelCount[0] * myIMU.aRes;
    myIMU.ay = (float)myIMU.accelCount[1] * myIMU.aRes;
    myIMU.az = (float)myIMU.accelCount[2] * myIMU.aRes;
    
    myIMU.readGyroData(myIMU.gyroCount);
    myIMU.gx = (float)myIMU.gyroCount[0] * myIMU.gRes;
    myIMU.gy = (float)myIMU.gyroCount[1] * myIMU.gRes;
    myIMU.gz = (float)myIMU.gyroCount[2] * myIMU.gRes;
    
    myIMU.readMagData(myIMU.magCount);

    // Calculate the magnetometer values in milliGauss
    // Include factory calibration per data sheet and user environmental
    // corrections
    // Get actual magnetometer value, this depends on scale being set
    myIMU.mx = (float)myIMU.magCount[0] * myIMU.mRes
               * myIMU.factoryMagCalibration[0] - myIMU.magBias[0];
    myIMU.my = (float)myIMU.magCount[1] * myIMU.mRes
               * myIMU.factoryMagCalibration[1] - myIMU.magBias[1];
    myIMU.mz = (float)myIMU.magCount[2] * myIMU.mRes
               * myIMU.factoryMagCalibration[2] - myIMU.magBias[2];

    //Serial.print("mx = ");  Serial.print((int)myIMU.mx);
      //Serial.print(" my = "); Serial.print((int)myIMU.my);
      //Serial.print(" mz = "); Serial.print((int)myIMU.mz);
      //Serial.println(" mG");
  }
  
  // Must be called before updating quaternions!
  myIMU.updateTime();

  MahonyQuaternionUpdate(myIMU.ax, myIMU.ay, myIMU.az, myIMU.gx * DEG_TO_RAD,
                         myIMU.gy * DEG_TO_RAD, myIMU.gz * DEG_TO_RAD, myIMU.my,
                         myIMU.mx, myIMU.mz, myIMU.deltat);

  // Serial print and/or display at 0.5 s rate independent of data rates
  myIMU.delt_t = millis() - myIMU.count;
  if(myIMU.delt_t > 250) {
    myIMU.yaw   = atan2(2.0f * (*(getQ()+1) * *(getQ()+2) + *getQ()
                  * *(getQ()+3)), *getQ() * *getQ() + *(getQ()+1)
                  * *(getQ()+1) - *(getQ()+2) * *(getQ()+2) - *(getQ()+3)
                  * *(getQ()+3));
    myIMU.pitch = -asin(2.0f * (*(getQ()+1) * *(getQ()+3) - *getQ()
                  * *(getQ()+2)));
    myIMU.roll  = atan2(2.0f * (*getQ() * *(getQ()+1) + *(getQ()+2)
                  * *(getQ()+3)), *getQ() * *getQ() - *(getQ()+1)
                  * *(getQ()+1) - *(getQ()+2) * *(getQ()+2) + *(getQ()+3)
                  * *(getQ()+3));
    myIMU.pitch *= RAD_TO_DEG;
    myIMU.yaw   *= RAD_TO_DEG;
    myIMU.yaw  += 2.51;
    myIMU.roll *= RAD_TO_DEG;

    /*Serial.print("Yaw, Pitch, Roll: ");
    Serial.print(myIMU.yaw, 2);
    Serial.print(", ");
    Serial.print(myIMU.pitch, 2);
    Serial.print(", ");
    Serial.println(myIMU.roll, 2);*/

    myIMU.count = millis();
    myIMU.sumCount = 0;
    myIMU.sum = 0;

    
    heading = atan2(myIMU.my, myIMU.mx);
    heading += 0.0436332;
    //if(heading < 0) heading += PI;
    // Correcting due to the addition of the declination angle
    //if(heading > 2*PI)heading -= 2*PI;
    headingDegrees = heading * 180/PI;
    headingFiltered = headingFiltered*0.5 + headingDegrees*0.5;

    Serial.print("heading: "); Serial.println(headingFiltered);
  }
}
