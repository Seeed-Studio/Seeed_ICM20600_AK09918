#include "AK09918.h"
#include <Wire.h>

AK09918_err_type_t err;
int32_t x, y, z;
AK09918 ak09918;

void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();

    err = ak09918.initialize();
    ak09918.switchMode(AK09918_POWER_DOWN);
    ak09918.switchMode(AK09918_CONTINUOUS_100HZ);
    Serial.begin(9600);

    err = ak09918.isDataReady();
    while (err != AK09918_ERR_OK) {
        Serial.println("Waiting Sensor");
        delay(100);
        err = ak09918.isDataReady();
    }
}

void loop() {
    err = ak09918.isDataReady();
    // err = AK09918_ERR_OK;
    if (err == AK09918_ERR_OK) {
        err = ak09918.isDataSkip();
        if (err == AK09918_ERR_DOR) {
            Serial.println(ak09918.strError(err));
        }
        err = ak09918.getData(&x, &y, &z);
        if (err == AK09918_ERR_OK) {
            Serial.print("X: ");
            Serial.print(x);
            Serial.print("uT");
            Serial.print("\tY: ");
            Serial.print(y);
            Serial.print("uT");
            Serial.print("\tZ: ");
            Serial.print(z);
            Serial.println("uT");
        } else {
            Serial.println(ak09918.strError(err));
        }
    } else {
        Serial.println(ak09918.strError(err));
    }
    delay(100);
}
