#include "ICM20600.h"
#include <Wire.h>

ICM20600 icm20600(true);

void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();

    Serial.begin(9600);
    icm20600.initialize();
    // icm20600.reset();
}

void loop() {
    // Serial.print("DeviceID: ");
    // Serial.println(icm20600.getDeviceID(), HEX);

    Serial.print("x = ");
    Serial.print(icm20600.getGyroscopeX());
    Serial.println(" dps");
    Serial.print("y = ");
    Serial.print(icm20600.getGyroscopeY());
    Serial.println(" dps");
    Serial.print("z = ");
    Serial.print(icm20600.getGyroscopeZ());
    Serial.println(" dps");


    Serial.print("x = ");
    Serial.print(icm20600.getAccelerationX());
    Serial.println(" mg");
    Serial.print("y = ");
    Serial.print(icm20600.getAccelerationY());
    Serial.println(" mg");
    Serial.print("z = ");
    Serial.print(icm20600.getAccelerationZ());
    Serial.println(" mg");

    Serial.println("-------------------------");
    delay(600);
}