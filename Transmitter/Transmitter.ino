#include <RF24.h>
#include <RF24_config.h>
#include <nRF24L01.h>
#include <printf.h>

//Transmitter Module
//Before testing: edit #'s directed by comments
//Transciever code example : https://forum.arduino.cc/t/simple-nrf24l01-2-4ghz-transceiver-demo/405123/2

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

char data[13]; //Example: { 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0}; 1: down, 0: up

const byte adress[5] = {'R', 'o', 'b', 'o', 'X'}; //advised to change
RF24 radio(2, 3); //CE, CSN; EDIT #'s

bool connection; //dsiplays if connected to receiver
bool connected;

unsigned long dctime;

unsigned long currentMillis;
unsigned long prevMillis;

unsigned long txIntervalMillis = 20; //Milliseconds between signals sent

void setup() {
  //List of radio methods https://maniacbug.github.io/RF24/classRF24.html
  radio.begin();
  radio.setDataRate( RF24_250KBPS); //RF24_250KBS, RF24_1MBPS, or RF24_2MBPS
  radio.setRetries(3, 5); //delay, count
  radio.openWritingPipe(adress); //starts communication w/ receiver
  data[12] = 1;

//defining buttons; can be optimized by for loop if needed
for(int i = 22; i<34; i++){
  pinMode(i, INPUT);
}
pinMode(34, OUTPUT);
pinMode(35, OUTPUT);
  /* SWAP VALUES IN RECEIVER MODULE
  22-Turn Left
  23-Drive
  24-Speed up
  25-Turn Right
  26-Slow Down
  27-Reverse
  28-Toggle Intake
  29-Intake Up 
  30-Pull
  31-Launch
  32-Push
  33-Intake Down
  */
}

void loop() {
  //EDIT #'s; may need to swap out for loop for other method
  for(int i = 0; i<12; i++){
    if(digitalRead(i+22) == HIGH){ //buton detection
      data[i+1] = 1;
    } else {
      data[i+1] = 0;
    }
  }
  if(connection){
    digitalWrite(34, HIGH);
    digitalWrite(35, HIGH);
  } else {
    digitalWrite(34, LOW);
    digitalWrite(35, LOW);
  }
  if(millis() - prevMillis >= txIntervalMillis){
    send();
    prevMillis = millis();
  }
}

void send() {
  data[0] = 1-data[0]; //flops between 1 and 0
  connected = radio.write(&data, sizeof(data)); //both sends signal to receiver and measures if signal is acknowledged
  if(connected){
    connection = true;
    dctime = millis();
  } else {
    if(dctime + 500 <= millis()){
      connection = false;
    }
  }
}