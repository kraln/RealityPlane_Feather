/*
 * License: MIT
 * 
 * Started with the Adafruit Example.
 */

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "Adafruit_DRV2605.h"
#include "BluefruitConfig.h"

#define FACTORYRESET_ENABLE         1
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"
Adafruit_DRV2605 drv;


Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

void setup(void)
{
  while (!Serial);
  delay(500);

  Serial.begin(115200);
  Serial.print(F("Initialising Reality Plane."));

  /* Set up BTLE */
  ble.begin(VERBOSE_MODE);
  ble.echo(false);
  ble.verbose(false);  
  
  /* Wait for connection */
  while (! ble.isConnected()) {
      delay(500);
  }

  ble.sendCommandCheckOK("AT+HWModeLED=MODE");
  ble.setMode(BLUEFRUIT_MODE_DATA);

  /* Setup vibration motor driver */
  drv.begin();
  drv.selectLibrary(1);
  drv.setMode(DRV2605_MODE_INTTRIG); 
  
  Serial.println( F("OK!") );
  Serial.println();

}

/* see if we got anything to work on */
void loop(void)
{
   // Check for user input
  char n, inputs[BUFSIZE+1];

  if (Serial.available())
  {
    n = Serial.readBytes(inputs, BUFSIZE);
    inputs[n] = 0;
    // Send input data to host via Bluefruit
    ble.print(inputs);
  }

  // check for ble input
  while ( ble.available() )
  {
    int c = ble.read();

    if (c > 0 && c <= 117)
    {
      drv.setWaveform(0, c);
      drv.setWaveform(1, 0);
      drv.go();
    }
  }
}

