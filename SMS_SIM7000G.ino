/*
  Sending SMS with SIM7000G module
  Created by Robert Ribeiro, June 28, 2023.
  All rights reserved.
*/

//included library to UART communication
#include <HardwareSerial.h>

//UART speed
#define SERIAL_BAUD_RATE    38400
#define GSM_BAUD_RATE       9600

//UART and PWR pin for GSM
#define GSM_RX_PIN          26
#define GSM_TX_PIN          27
#define GSM_PWR_PIN         4

String phoneNumber = "+9999999999999"; //phone number format=+ZZDDXXXXXXXXX

HardwareSerial SerialGSM(1); //serial port to GSM

void setup() {
  //initialize serial ports
  Serial.begin(SERIAL_BAUD_RATE);
  SerialGSM.begin(GSM_BAUD_RATE, SERIAL_8N1, GSM_RX_PIN, GSM_TX_PIN); //Rx, Tx
  delay(1000);

  //power pin as output
  pinMode(GSM_PWR_PIN, OUTPUT);

  //reset SIM7000G module
  digitalWrite(GSM_PWR_PIN, LOW);
  delay(100);
  digitalWrite(GSM_PWR_PIN, HIGH);
  delay(100);

  Serial.println(F("\n\nSTARTING...")); 
  delay(2000); 

  //initial gsm setup
  gsm_setup();
}

void loop() {

  //send SMS function
  gsm_send_sms();
  delay(3000);
}


void gsm_setup(void) {
  send_at_command("AT", 500); //test if module is ok and response
  send_at_command("AT+IPR=9600", 500); //set the baud rate
  send_at_command("AT+CSQ", 1000);   //signal quality report
  send_at_command("AT+CREG?", 1000); //network registration (0 - disable network registration, 1 - enable network registration, etc)
  send_at_command("AT+COPS?", 1000); //operator selection
  send_at_command("AT+CBC", 500);   //battery charge
}


void gsm_send_sms(void) {
  Serial.println(F("SENDING SMS..."));

  String message = "SMS sent by SIM7000G: "; //message to be sent
  message += millis() / 10000;
  message += "\n";

  send_at_command("AT+CMGF=1", 5000); //select SMS message format (0 - PDU mode, 1 - text mode)
  send_at_command(String("AT+CMGS=\"") + String(phoneNumber) + String("\""), 5000); //send SMS message
  //send_at_command("AT+CMGS=\"+ZZDXXXXXXXXX\"", 5000);
  send_at_command(message, 5000); //message
  SerialGSM.write((byte)0x1A); //ctrl+z
}


void send_at_command(String cmd, unsigned long timeout) {
  //Serial.println(cmd);
  SerialGSM.println(cmd);

  unsigned long startTime = millis();
  while (startTime + timeout > millis()) {
    while (SerialGSM.available()) {
      Serial.write(SerialGSM.read());
    }
  }

  Serial.println("");
}
