#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <LiquidCrystal.h>

Adafruit_MPU6050 mpu;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

float offsetX = 0, offsetY = 0, offsetZ = 0;

float accelX, accelY, accelZ;
float velocityX = 0, velocityY = 0, velocityZ = 0;
float displacementX = 0, displacementY = 0, displacementZ = 0;

unsigned long previousTime, currentTime;
float deltaTime;

const float LIGHT_TREMOR_THRESHOLD = 0.2;
const float MODERATE_TREMOR_THRESHOLD = 0.8;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.clear();

  if (!mpu.begin()) {
    Serial.println("MPU6050 initialization failed!");
    while (1);
  }
  Serial.println("MPU6050 initialized successfully.");

  calibrateSensor();
  previousTime = millis();
}

void loop() {
  currentTime = millis();
  deltaTime = (currentTime - previousTime) / 1000.0;
  previousTime = currentTime;
  
  sensors_event_t accelEvent;
  mpu.getAccelerometerSensor()->getEvent(&accelEvent);

  accelX = accelEvent.acceleration.x - offsetX;
  accelY = accelEvent.acceleration.y - offsetY;
  accelZ = accelEvent.acceleration.z - offsetZ;

  velocityX += accelX * deltaTime;
  velocityY += accelY * deltaTime;
  velocityZ += accelZ * deltaTime;

  displacementX += velocityX * deltaTime;
  displacementY += velocityY * deltaTime;
  displacementZ += velocityZ * deltaTime;

  Serial.print("A:"); Serial.print(accelZ); Serial.print(", ");
  Serial.print("V:"); Serial.print(velocityZ); Serial.print(", ");
  Serial.print("D:"); Serial.println(displacementZ);

  float totalAccel = sqrt(accelX * accelX + accelY * accelY + accelZ * accelZ);
  float maccelZ;
  if (accelZ < 0) {
    maccelZ = -accelZ;
  }
  else {
    maccelZ = accelZ;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  if (maccelZ < LIGHT_TREMOR_THRESHOLD) {
    lcd.print("No earthquake");
  } else if (maccelZ < MODERATE_TREMOR_THRESHOLD) {
    lcd.print("Light tremor");
  } else {
    lcd.print("Severe tremor!");
  }

  lcd.setCursor(0, 1);
  lcd.print("Accel: ");
  lcd.print(totalAccel, 2);
  lcd.print(" g");

  delay(100);
}

void calibrateSensor() {
  const int numReadings = 100;
  float sumX = 0, sumY = 0, sumZ = 0;

  Serial.println("Calibrating... Please keep the device still.");
  for (int i = 0; i < numReadings; i++) {
    sensors_event_t accelEvent;
    mpu.getAccelerometerSensor()->getEvent(&accelEvent);
    sumX += accelEvent.acceleration.x;
    sumY += accelEvent.acceleration.y;
    sumZ += accelEvent.acceleration.z;
    delay(10);
  }

  offsetX = sumX / numReadings;
  offsetY = sumY / numReadings;
  offsetZ = sumZ / numReadings;
  Serial.println("Calibration complete.");
  Serial.print("Offset X: "); Serial.println(offsetX);
  Serial.print("Offset Y: "); Serial.println(offsetY);
  Serial.print("Offset Z: "); Serial.println(offsetZ);
}
