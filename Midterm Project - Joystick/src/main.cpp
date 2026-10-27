#include <Arduino.h>
#include <math.h>
#include <Adafruit_BNO08x.h>
#include <AccelStepper.h>

bool isMainBoard = true;

int pinStep = 9;
int pinDir = 10;
int pinRx = 5;
int pinTx = 6;


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
int movementState = 0;

Adafruit_BNO08x bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;

AccelStepper onlyAxis(1,pinStep,pinDir);

void setup(void) {
  if (isMainBoard) {
    Serial.begin(115200);
    while (!Serial) {
      ;
    }
  }

  Serial1.setPins(pinRx, pinTx);
  Serial1.begin(9600);

  while (!Serial1)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial1.println("Adafruit BNO08x test!");

  // Try to initialize!
  if (!bno08x.begin_I2C()) {
    Serial1.println("Failed to find BNO08x chip");
    while (1) {
      delay(10);
    }
  } 
  Serial1.println("BNO08x Found!");

  setReports();

  pinMode(pinStep, OUTPUT);
  pinMode(pinDir, OUTPUT);

  onlyAxis.setMaxSpeed(1000);
  
  onlyAxis.setAcceleration(8000);

  delay(2000);
  if (bno08x.wasReset()) {
    Serial1.print("sensor was reset ");
    setReports();
  }
}


void RunMainBoardLoop() {
  onlyAxis.run();
  
  if (bno08x.getSensorEvent(&sensorValue)); {

    float x = sensorValue.un.accelerometer.x;
    float y = sensorValue.un.accelerometer.y;
    float z = sensorValue.un.accelerometer.z;

    float alpha = atan2(x,sqrt(y*y+z*z)) * RAD_TO_DEG;
    float beta = atan2(y,z) * RAD_TO_DEG;

    if (abs(alpha) < 30 && abs(beta) < 30) {
      curMovement = JoystickMode::RESTING;
      onlyAxis.stop();
    }

    if (abs(alpha) < 30 && beta < -30) {
      curMovement = JoystickMode::FORWARD;
      onlyAxis.setSpeed(700);
    }

    if (abs(alpha) < 30 && beta > 30) {
      curMovement = JoystickMode::BACKWARD;
      onlyAxis.setSpeed(-700);
    }

    if (alpha > 30 && abs(beta) < 30) {
      curMovement = JoystickMode::LEFT;
      onlyAxis.stop();
    }

    if (alpha < -30 && abs(beta) < 30) {
      curMovement = JoystickMode::RIGHT;
      onlyAxis.stop();
    }

    if (alpha > 30 && beta < -30) {
      curMovement = JoystickMode::FRONT_LEFT;
      onlyAxis.setSpeed(700);
    }

    if (alpha < -30 && beta < -30) {
      curMovement = JoystickMode::FRONT_RIGHT;
      onlyAxis.setSpeed(700);
    }

    if (alpha > 30 && beta > 30) {
      curMovement = JoystickMode::BACK_LEFT;
      onlyAxis.setSpeed(-700);
    }

    if (alpha < -30 && beta > 30) {
      curMovement = JoystickMode::BACK_RIGHT;
      onlyAxis.setSpeed(-700);
    }

    movementState = int(curMovement);
    Serial1.println(movementState);
  }
}

void RunSecondBoardLoop() {
  onlyAxis.run();

  //while (Serial1.available()) {
  movementState = Serial1.read();
  //}
  Serial.println(movementState);
  Serial1.println(movementState);

  if (movementState = 3) {
    onlyAxis.setSpeed(700);
  }

  if (movementState = 4) {
    onlyAxis.setSpeed(-700);
  }

  if (movementState = 5) {
    onlyAxis.setSpeed(700);
  }

  if (movementState = 6) {
    onlyAxis.setSpeed(-700);
  }

  if (movementState = 7) {
    onlyAxis.setSpeed(700);
  }

  if (movementState = 8) {
    onlyAxis.setSpeed(-700);
  }
}

void loop() {
  if (isMainBoard) {
    RunMainBoardLoop();
  } else {
    RunSecondBoardLoop();
  }
}

void setReports(void) {
  Serial.println("Setting desired reports");
  if (!bno08x.enableReport(SH2_ACCELEROMETER,1000)) {
    Serial.println("Could not enable accelerometer");
  } else {
    Serial.println("Set accelerometer report...success!");
  }
}


