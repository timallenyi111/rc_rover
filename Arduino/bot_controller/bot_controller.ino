#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const int CE = 7;   //pin used for the RF24 CE Pin
const int CSN = 8;  //pin used for the RF24 CSN Pin

//enable the rf24 instance
RF24 radio(CE,CSN); 

//this should be the tx address on the bot
const byte rx_address[6] = "00001";  //address used by the controller for recieving

//this should be the rx address on the bot
const byte tx_address[6] = "00011";  //address used for sending on the controller

//used to switch the controller into transmit mode when needed
bool transmit = false; 

//used for data being sent
char payload[32];

//used to count the number of bytes to be sent
int i = 0;
char serialInput = 0;
bool endOfString = false;

//*************temporary for debugging
int x = 0;

void setup() {
  Serial.begin(9600);
  uint8_t details[43] = {0};

  //start the radio instance
  radio.begin(); 

  //set PA level to low for testing*******
  radio.setPALevel(RF24_PA_MIN);

  //open a writing pipe for sending to the bot
  radio.openWritingPipe(tx_address);

  //open reading pipe for listening for messages from the bot
  radio.openReadingPipe(1, rx_address); //uses reading pipe 1

  //radio is in listening mode until it is ready to start sending data
  radio.startListening();
  
  //printSettings();
  Serial.println("SETUP COMPLETE");
}


void loop() {
  //used for debugging radio link
  //writeLoopMessage();
  //delay(1000);
  
  if (Serial.available()){
    //delay so that the buffer has time to fill up
    delay(10);
    
    //read everything in the serial buffer
    i = 0;
    //endOfString = false;
    //Serial.print("Serial Buffer Size: ");
    //Serial.println(Serial.available());
    
    while(Serial.available() > 0) {
      payload[i] = Serial.read();
      i++;
    }
    
    //put the radio in TX mode
    radio.stopListening();
    
    //send the payload
    radio.write(&payload, i+1);
    
    //put the radio back into RX mode
    radio.startListening();
  }
}

void writeLoopMessage(){
  radio.stopListening();
  const char text[] = "Loop..";
  radio.write(&text, sizeof(text));
  radio.startListening();
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
  
  Serial.print("RX_ADDR_P1: ");
  int i = 0;
  pulseCSN();
  SPI.transfer(0x0B);
  delay(100);
  while(i<5){
    Serial.print(SPI.transfer(0x00),HEX);
    i++;
  }
  Serial.println();
  
  Serial.print("TX_ADDR: ");
  i = 0;
  pulseCSN();
  SPI.transfer(0x10);
  delay(100);
  while(i<5){
    Serial.print(SPI.transfer(0x00),HEX);
    i++;
  }
  Serial.println();
}
