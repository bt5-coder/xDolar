#include "HAL/HAL.h"
#include "Wire.h"

void HAL::I2C_Init(bool startScan)
{
    Wire.begin(CONFIG_MCU_SDA_PIN,CONFIG_MCU_SCL_PIN);

    if(!startScan)
        return;

    uint8_t error, address, data;
    int nDevices;

    Serial.println("I2C: device scanning...");

    nDevices = 0;
    for (address = 1; address < 127; address++ )
    {
        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0)
        {
            Serial.print("I2C: device found at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.print(address, HEX);
            Serial.println(" !");

            // data = read_byte(address,0x70);
            uint8_t data = 0;                         // `data` will store the register data
            Wire.beginTransmission(address);         // Initialize the Tx buffer
            Wire.write(0x70);                       // Put slave register address in Tx buffer
            Wire.endTransmission(false);  // Send the Tx buffer, but send a restart to keep connection alive

            Wire.requestFrom(address, (size_t)1);       // Read one byte from slave register address
            if (Wire.available()) data = Wire.read();  // Fill Rx buffer with result

            Serial.println("I2C: Device 0x75 register dump");
            if (address < 16)
                Serial.print("0");
            Serial.print(data, HEX);
            Serial.println(" !");
            
            nDevices++;
        }
        else if (error == 4)
        {
            Serial.print("I2C: unknow error at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.println(address, HEX);
        }
    }

    Serial.printf("I2C: %d devices was found\r\n", nDevices);
}
