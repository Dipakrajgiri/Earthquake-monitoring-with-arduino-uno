#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <LiquidCrystal.h>

// MPU6050 and LCD objects
Adafruit_MPU6050 mpu;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Calibration offsets
float offsetX = 0, offsetY = 0, offsetZ = 0;

// Variables for acceleration, velocity, and displacement
float accelX, accelY, accelZ;
float velocityX = 0, velocityY = 0, velocityZ = 0;
float displacementX = 0, displacementY = 0, displacementZ = 0;

// Timing variables
unsigned long previousTime, currentTime;
float deltaTime;

// Thresholds for earthquake severity (in g)
const float LIGHT_TREMOR_THRESHOLD = 0.2;
const float MODERATE_TREMOR_THRESHOLD = 0.8;

void setup() {
  // Serial and LCD initialization
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.clear();

  // Initialize MPU6050 sensor
  if (!mpu.begin()) {
    Serial.println("MPU6050 initialization failed!");
    while (1);
  }
  Serial.println("MPU6050 initialized successfully.");

  // Calibration: collect readings while the device is at rest
  calibrateSensor();
  previousTime = millis();
}

void loop() {
  // Calculate deltaTime
  currentTime = millis();
  deltaTime = (currentTime - previousTime) / 1000.0;
  previousTime = currentTime;
  
  // Get acceleration data from MPU6050
  sensors_event_t accelEvent;
  mpu.getAccelerometerSensor()->getEvent(&accelEvent);

  // Adjusted accelerations
  accelX = accelEvent.acceleration.x - offsetX;
  accelY = accelEvent.acceleration.y - offsetY;
  accelZ = accelEvent.acceleration.z - offsetZ;

  // Calculate velocity and displacement
  velocityX += accelX * deltaTime;
  velocityY += accelY * deltaTime;
  velocityZ += accelZ * deltaTime;

  displacementX += velocityX * deltaTime;
  displacementY += velocityY * deltaTime;
  displacementZ += velocityZ * deltaTime;

  //Serial.print("A:"); Serial.println(accelZ);

  // Display acceleration, velocity, and displacement
  Serial.print("A:"); Serial.print(accelZ); Serial.print(", ");
  Serial.print("V:"); Serial.print(velocityZ); Serial.print(", ");
  Serial.print("D:"); Serial.println(displacementZ);

  // Calculate total acceleration
  float totalAccel = sqrt(accelX * accelX + accelY * accelY + accelZ * accelZ);
  float maccelZ;
  if (accelZ < 0) {
    maccelZ = -accelZ;
  }
  else {
    maccelZ = accelZ;
  }
  // Earthquake detection conditions and LCD display
  lcd.clear();
  lcd.setCursor(0, 0);
  if (maccelZ < LIGHT_TREMOR_THRESHOLD) {
    lcd.print("No earthquake");
    //Serial.print("No earthquake");
  } else if (maccelZ < MODERATE_TREMOR_THRESHOLD) {
    lcd.print("Light tremor");
    //Serial.print("Light tremor");
  } else {
    lcd.print("Severe tremor!");
    //Serial.print("Severe tremor!");
  }

  lcd.setCursor(0, 1);
  lcd.print("Accel: ");
  lcd.print(totalAccel, 2);  // Display acceleration with 2 decimal places
  lcd.print(" g");

  //Serial.print(" | Accel: ");
  //Serial.print(totalAccel, 2);  // Print the acceleration value to Serial with 2 decimal places
  //Serial.println(" g");

  // Delay to control sampling rate
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
