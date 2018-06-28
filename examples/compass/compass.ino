#include "AK09918.h"
#include "ICM20600.h"
#include <Wire.h>

AK09918_err_type_t err;
int32_t x, y, z;
AK09918 ak09918;
ICM20600 icm20600(true);
int16_t acc_x, acc_y, acc_z;
int32_t offset_x, offset_y, offset_z;
double roll, pitch;
// Find the magnetic declination at your location
// http://www.magnetic-declination.com/
double declination_shenzhen = -2.2;

void setup()
{
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();

    err = ak09918.initialize();
    icm20600.initialize();
    ak09918.switchMode(AK09918_POWER_DOWN);
    ak09918.switchMode(AK09918_CONTINUOUS_100HZ);
    Serial.begin(9600);

    err = ak09918.isDataReady();
    while (err != AK09918_ERR_OK) 
    {
        Serial.println("Waiting Sensor");
        delay(100);
        err = ak09918.isDataReady();
    }

    Serial.println("Start figure-8 calibration after 2 seconds.");
    delay(2000);
    calibrate(10000, &offset_x, &offset_y, &offset_z);
}

void loop()
{
    // get acceleration
    acc_x = icm20600.getAccelerationX();
    acc_y = icm20600.getAccelerationY();
    acc_z = icm20600.getAccelerationZ();
    // roll/pitch in radian
    roll = atan2((float)acc_y, (float)acc_z);
    pitch = atan2(-(float)acc_x, sqrt((float)acc_y*acc_y+(float)acc_z*acc_z));
    Serial.print("roll: ");
    Serial.print(roll*57.3);
    Serial.print("\tpitch: ");
    Serial.print(pitch*57.3);

    ak09918.getData(&x, &y, &z);
    x = x - offset_x;
    y = y - offset_y;
    z = z - offset_z;

    double Xheading = x * cos(pitch) + y * sin(roll) * sin(pitch) + z * cos(roll) * sin(pitch);
    double Yheading = y * cos(roll) - z * sin(pitch);
    

    double heading = 180 + 57.3*atan2(Yheading, Xheading) + declination_shenzhen;
    // if (Xheading < 0 && Yheading < 0)
    // {
    //     heading = 180.0 - 57.3*atan2(Yheading, Xheading);
    // }
    // else if (Xheading > 0 && Yheading < 0)
    // {
    //     heading = 57.3*atan2(Yheading, Xheading);
    // }
    // else if (Xheading > 0 && Yheading > 0)
    // {
    //     heading = 360.0 - 57.3*atan2(Yheading, Xheading);
    // }
    // else if (Xheading < 0 && Yheading > 0)
    // {
    //     heading = 180.0 + 57.3*atan2(Yheading, Xheading); 
    // }
    // else if (Xheading == 0 && Yheading < 0)
    // {
    //     heading = 90.0;
    // }
    // else if (Xheading == 0 && Yheading > 0)
    // {
    //     heading = 270.0;
    // }


    Serial.print("\tHeading: ");
    Serial.println(heading);
  
    delay(200);
    
}

void calibrate(uint32_t timeout, int32_t *offsetx, int32_t *offsety, int32_t*offsetz)
{
  int32_t value_x_min = 0;
  int32_t value_x_max = 0;
  int32_t value_y_min = 0;
  int32_t value_y_max = 0;
  int32_t value_z_min = 0;
  int32_t value_z_max = 0;
  uint32_t timeStart = 0;

  ak09918.getData(&x, &y, &z);

  value_x_min = x;
  value_x_max = x;
  value_y_min = y;
  value_y_max = y;
  value_z_min = z;
  value_z_max = z;
  delay(100);

  timeStart = millis();
  
  while((millis() - timeStart) < timeout)
  {
    ak09918.getData(&x, &y, &z);
    
    /* Update x-Axis max/min value */
    if(value_x_min > x)
    {
      value_x_min = x;
      // Serial.print("Update value_x_min: ");
      // Serial.println(value_x_min);

    } 
    else if(value_x_max < x)
    {
      value_x_max = x;
      // Serial.print("update value_x_max: ");
      // Serial.println(value_x_max);
    }

    /* Update y-Axis max/min value */
    if(value_y_min > y)
    {
      value_y_min = y;
      // Serial.print("Update value_y_min: ");
      // Serial.println(value_y_min);

    } 
    else if(value_y_max < y)
    {
      value_y_max = y;
      // Serial.print("update value_y_max: ");
      // Serial.println(value_y_max);
    }

    /* Update z-Axis max/min value */
    if(value_z_min > z)
    {
      value_z_min = z;
      // Serial.print("Update value_z_min: ");
      // Serial.println(value_z_min);

    } 
    else if(value_z_max < z)
    {
      value_z_max = z;
      // Serial.print("update value_z_max: ");
      // Serial.println(value_z_max);
    }
    
    Serial.print(".");
    delay(100);

  }

  *offsetx = value_x_min + (value_x_max - value_x_min)/2;
  *offsety = value_y_min + (value_y_max - value_y_min)/2;
  *offsetz = value_z_min + (value_z_max - value_z_min)/2;
}