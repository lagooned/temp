/*
 * temp.ino
 * Takes an average temperature over ten seconds, then either writes powerPin
 * HIGH if it is below targetTemp, or LOW if it is above targetTemp.
 *
 * This program is part of my temperature controlled sous vide cooker.
 * By Jared Engler
 */

#include <OneWire.h>

// global variables
float targetTemp = 30.0;
int sensorPin = 10;
int powerPin = 13;
OneWire ds(sensorPin);

/*
 * run at first power on
 */
void setup() {
    Serial.begin(9600);
    pinMode(powerPin, OUTPUT);
}

/*
 * main loop after setup
 */
void loop() {
    // calculate average
    float total;
    for(int i = 0; i < 10; i++) {
        total += getTemp();
        delay(1000);
    }
    float average = total/10;

    // toggle heat
    if (average < targetTemp) {
        digitalWrite(powerPin, HIGH);
    } else {
        digitalWrite(powerPin, LOW);
    }

    // print average
    Serial.println(average);
}

/*
 * get temp using one-wire library
 */
float getTemp()  {
    byte data[12];
    byte addr[8];

    if (!ds.search(addr)) {
        ds.reset_search();
        return -1000;
    }

    if (OneWire::crc8(addr, 7) != addr[7]) {
        Serial.println("CRC is not valid!");
        return -1000;
    }

    if (addr[0] != 0x10 && addr[0] != 0x28) {
        Serial.print("Device is not recognized");
        return -1000;
    }

    ds.reset();
    ds.select(addr);
    ds.write(0x44,1);

    byte present = ds.reset();
    ds.select(addr);
    ds.write(0xBE);

    for (int i = 0; i < 9; i++) {
        data[i] = ds.read();
    }

    ds.reset_search();

    byte MSB = data[1];
    byte LSB = data[0];

    float tRead = ((MSB<<8) | LSB);
    float temperature = tRead / 16;

    return temperature;
}
