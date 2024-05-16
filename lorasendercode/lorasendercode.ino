//#define E32_TTL_1W // Activate this for E32 30d modules
#include "LoRa_E32.h"
#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // PCB version 4.3 and later
LoRa_E32 FixajSS(&mySerial);

#define M0 7
#define M1 6

#define Address 2    // Address range 0--65000, should be unique among modules
#define Channel 20   // Channel range 0--31 (E32 433 frequency = 410 + channel value)

struct Signal {
  float temperature;
  float humidity;
  int mq7Value;
  int rainValue;
  float distance;
} data;

void setup() {
  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);

  Serial.begin(9600);
  FixajSS.begin();

  configureLoRaE32();

  digitalWrite(M0, LOW);
  digitalWrite(M1, LOW);

  delay(500);
  Serial.println("Starting..");
}

void loop() {
  while (FixajSS.available() > 1) {
    ResponseStructContainer rsc = FixajSS.receiveMessage(sizeof(Signal));
    struct Signal data = *(Signal*)rsc.data;
    rsc.close();

    Serial.println("Data received:");
    Serial.print("Temperature: ");
    Serial.println(data.temperature);
    Serial.print("Humidity: ");
    Serial.println(data.humidity);
    Serial.print("MQ-7 CO Level: ");
    Serial.println(data.mq7Value);
    Serial.print("Rain Sensor Value: ");
    Serial.println(data.rainValue);
    Serial.print("Distance: ");
    Serial.println(data.distance);
  }
}

void configureLoRaE32() {
  digitalWrite(M0, HIGH);
  digitalWrite(M1, HIGH);

  ResponseStructContainer c;
  c = FixajSS.getConfiguration();
  Configuration configuration = *(Configuration*)c.data;

  // Update configuration
  configuration.ADDL = lowByte(Address);
  configuration.ADDH = highByte(Address);
  configuration.CHAN = Channel;

  configuration.SPED.airDataRate = AIR_DATA_RATE_010_24;
  configuration.OPTION.transmissionPower = POWER_20;

  configuration.SPED.uartBaudRate = UART_BPS_9600;
  configuration.SPED.uartParity = MODE_00_8N1;
  configuration.OPTION.fec = FEC_0_OFF;
  configuration.OPTION.fixedTransmission = FT_FIXED_TRANSMISSION;
  configuration.OPTION.wirelessWakeupTime = WAKE_UP_250;
  configuration.OPTION.ioDriveMode = IO_D_MODE_PUSH_PULLS_PULL_UPS;

  ResponseStatus rs = FixajSS.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
  c.close();
}
