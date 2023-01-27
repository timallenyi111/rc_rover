//this program can be configured to program either a transmitter or 
//reciever and is for testing purposes only


#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const int CE = 7;
const int CSN = 8;
byte registerByte;
byte newByte;

RF24 radio(CE,CSN);

const bool receiver = true; //should this device be a transmitter or receiver

const byte address[6] = "00011";  //can be anything

void setup() {
  Serial.begin(9600);

  radio.begin();
  if(receiver){
    radio.openReadingPipe(0,address);
    radio.startListening();  
  } else {
    radio.openWritingPipe(address);
    radio.setPALevel(RF24_PA_MIN);
    radio.stopListening();
  }
  
  printSettings();
}

void loop() {
  if(receiver){
    while(1){
      if(radio.available()) {
        char text[32] = "";
        radio.read(&text, sizeof(text));
        Serial.print("input: ");
        Serial.println(text);
      }
    }  
  } else {
    while(1){
      const char text[] = "Hello World";
      radio.write(&text, sizeof(text));
      delay(2000);  
    }
  }
}

void pulseCSN(){
  digitalWrite(CSN,HIGH);
  delay(10);
  digitalWrite(CSN,LOW);
  return;
}

unsigned char readRegister(unsigned char address){
  unsigned char data;
  pulseCSN();
  SPI.transfer(address);
  delay(100);
  data = SPI.transfer(0x00);
  return(data);
}

void printSettings(){
  Serial.print("CONFIG: ");
  Serial.println(readRegister(0x00),BIN);
  Serial.print("EN_AA: ");
  Serial.println(readRegister(0x01),BIN);
  Serial.print("EN_RXADDR: ");
  Serial.println(readRegister(0x02),BIN);
  Serial.print("SETUP_AW: ");
  Serial.println(readRegister(0x03),BIN);
  Serial.print("SETUP_RETR: ");
  Serial.println(readRegister(0x04),BIN);
  Serial.print("RF_CH: ");
  Serial.println(readRegister(0x05),BIN);
  Serial.print("RF_SETUP: ");
  Serial.println(readRegister(0x06),BIN);
  Serial.print("STATUS: ");
  Serial.println(readRegister(0x07),BIN);
  Serial.print("OBSERVE_TX: ");
  Serial.println(readRegister(0x08),BIN);
  Serial.print("RPD: ");
  Serial.println(readRegister(0x09),BIN);
  if(receiver){
    Serial.print("RX_ADDR_P0: ");
    int i = 0;
    pulseCSN();
    SPI.transfer(0x0A);
    delay(100);
    while(i<5){
      Serial.print(SPI.transfer(0x00),HEX);
      i++;
    }
    Serial.println(); 
  } else {
    Serial.print("TX_ADDR: ");
    int i = 0;
    pulseCSN();
    SPI.transfer(0x10);
    delay(100);
    while(i<5){
      Serial.print(SPI.transfer(0x00),HEX);
      i++;
    }
    Serial.println();  
  }
  
}
