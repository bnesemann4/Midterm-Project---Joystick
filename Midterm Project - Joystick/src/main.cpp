#include <Arduino.h>
#include <math.h>
#include <Adafruit_BNO08x.h>

void setReports();

#define BNO08X_RESET -1

enum JoystickMode {
  RESTING,       // 0
  FORWARD,       // 1
  BACKWARD,      // 2
  LEFT,          // 3
  RIGHT,         // 4
  FRONT_LEFT,    // 5
  FRONT_RIGHT,   // 6
  BACK_LEFT,     // 7
  BACK_RIGHT,    // 8
};

JoystickMode curMovement = RESTING;

Adafruit_BNO08x bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;

void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10); 

  Serial.println("Adafruit BNO08x test!");

  // Try to initialize!
  if (!bno08x.begin_I2C()) {
    Serial.println("Failed to find BNO08x chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("BNO08x Found!");

  setReports();

}


void loop() {
  delay(10);

  if (bno08x.wasReset()) {
    Serial.print("sensor was reset ");
    setReports();
  }

  if (!bno08x.getSensorEvent(&sensorValue)) {
    return;
  }

  float x = sensorValue.un.accelerometer.x;
  float y = sensorValue.un.accelerometer.y;
  float z = sensorValue.un.accelerometer.z;

  float alpha = atan2(x,sqrt(y*y+z*z)) * RAD_TO_DEG;
  float beta = atan2(y,z) * RAD_TO_DEG;

  if (abs(alpha) < 30 && abs(beta) < 30) {
    curMovement = JoystickMode::RESTING;
    Serial.println("Currently Resting");
  }

  if (abs(alpha) < 30 && beta < -30) {
    curMovement = JoystickMode::FORWARD;
    Serial.println("Moving Forward");
  }

  if (abs(alpha) < 30 && beta > 30) {
    curMovement = JoystickMode::BACKWARD;
    Serial.println("Moving Backward");
  }

  if (alpha > 30 && abs(beta) < 30) {
    curMovement = JoystickMode::LEFT;
    Serial.println("Moving Left");
  }

  if (alpha < -30 && abs(beta) < 30) {
    curMovement = JoystickMode::RIGHT;
    Serial.println("Moving Right");
  }

  if (alpha > 30 && beta < -30) {
    curMovement = JoystickMode::FRONT_LEFT;
    Serial.println("Moving Front Left");
  }

  if (alpha < -30 && beta < -30) {
    curMovement = JoystickMode::FRONT_RIGHT;
    Serial.println("Moving Front Right");
  }

  if (alpha > 30 && beta > 30) {
    curMovement = JoystickMode::BACK_LEFT;
    Serial.println("Moving Back Left");
  }

  if (alpha < -30 && beta > 30) {
    curMovement = JoystickMode::BACK_RIGHT;
    Serial.println("Moving Back Right");
  }
}

void setReports(void) {
  Serial.println("Setting desired reports");
  if (!bno08x.enableReport(SH2_ACCELEROMETER)) {
    Serial.println("Could not enable accelerometer");
  } else {
    Serial.println("Set accelerometer report...success!");
  }
}