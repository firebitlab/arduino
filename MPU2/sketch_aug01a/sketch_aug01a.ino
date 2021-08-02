#include <Wire.h>

#include <MPU6050_tockn.h>
MPU6050_tockn mpu6050_1(Wire);

#include <MPU6050.h>
MPU6050 mpu6050_2;
unsigned long timer = 0;
float timeStep = 0.01;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  mpu6050_1.begin();
  mpu6050_1.calcGyroOffsets(true);


  while (!mpu6050_2.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }

}

void loop() {

  //MPU 1
  mpu6050_1.update();
//  Serial.print("aX "); Serial.print(mpu6050_1.getAccX());
//  Serial.print(" aY "); Serial.print(mpu6050_1.getAccY());
//  Serial.print(" aZ "); Serial.print(mpu6050_1.getAccZ());
//
//  Serial.print(" gX "); Serial.print(mpu6050_1.getGyroX());
//  Serial.print(" gY "); Serial.print(mpu6050_1.getGyroY());
//  Serial.print(" gZ "); Serial.print(mpu6050_1.getGyroZ());
//
//  Serial.print(" X "); Serial.print(mpu6050_1.getAngleX());
//  Serial.print(" Y "); Serial.print(mpu6050_1.getAngleY());
//  Serial.print(" Z "); Serial.print(mpu6050_1.getAngleZ());


  //--------------------MPU 2
  Vector normAccel = mpu6050_2.readNormalizeAccel();
  Vector normGyro = mpu6050_2.readNormalizeGyro();

  int pitch = -(atan2(normAccel.XAxis, sqrt(normAccel.YAxis*normAccel.YAxis + normAccel.ZAxis*normAccel.ZAxis))*180.0)/M_PI;
  int roll = (atan2(normAccel.YAxis, normAccel.ZAxis)*180.0)/M_PI;
  int yaw = yaw + normGyro.ZAxis * timeStep;

//  Serial.print("aX ");
//  Serial.print(normAccel.XAxis);
//  Serial.print(" aY ");
//  Serial.print(normAccel.YAxis);
//  Serial.print(" aZ ");
//  Serial.s(normAccel.ZAxis);
//
//  Serial.print(" gX ");
//  Serial.print(normGyro.XAxis);
//  Serial.print(" gY ");
//  Serial.print(normGyro.YAxis);
//  Serial.print(" gZ ");
//  Serial.print(normGyro.ZAxis);
//
//  Serial.print(" X "); Serial.print(pitch);
//  Serial.print(" Y "); Serial.println(roll);

  //Serial Plotter
  Serial.print(mpu6050_1.getAngleX()); Serial.print(" ");
  Serial.print(mpu6050_1.getAngleY()); Serial.print(" ");
  Serial.print(pitch); Serial.print(" ");
  Serial.print(roll); Serial.println(" ");

  delay(100);
}
