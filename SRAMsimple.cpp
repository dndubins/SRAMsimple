/*  SRAMsimple.cpp - Library for reading and writing data from an Arduino Uno to a 23LC1024 chip.
 *  Original sketch created by J. B. Gallaher on 07/09/2016. 
 *  Library created by David Dubins, November 12th, 2018.
 *  Released into the public domain.
 */

#include "Arduino.h"
#include "SRAMsimple.h"

SRAMsimple::SRAMsimple(){/*nothing to construct*/}
SRAMsimple::~SRAMsimple(){/*nothing to destruct*/}

byte CS=10; // default CS global variable

/*  Set up the memory chip to either single byte or sequence of bytes mode **********/
void SRAMsimple::SetMode(byte CSpin, char Mode){            // Select for single or multiple byte transfer
  CS=CSpin; // set global variable CS to user-defined CS pin
  pinMode(CS, OUTPUT);	                        // set CS pin to output mode
  digitalWrite(CS, LOW);                        // set SPI slave select LOW
  SPI.transfer(WRMR);                           // command to write to mode register
  SPI.transfer(Mode);                           // set for sequential mode
  digitalWrite(CS, HIGH);                       // release chip select to finish command
}

/************ Byte transfer functions ***************************/
void SRAMsimple::WriteByte(uint32_t address, byte data_byte) {
  SetMode(CS,ByteMode);                 // set to send/receive single byte of data
  digitalWrite(CS, LOW);                         // set SPI slave select LOW;
  SPI.transfer(WRITE);                           // send WRITE command to the memory chip
  SPI.transfer((byte)(address >> 16));           // send high byte of address
  SPI.transfer((byte)(address >> 8));            // send middle byte of address
  SPI.transfer((byte)address);                   // send low byte of address
  SPI.transfer(data_byte);                       // write the data to the memory location
  digitalWrite(CS, HIGH);                        //set SPI slave select HIGH
}

byte SRAMsimple::ReadByte(uint32_t address) {
  SetMode(CS,ByteMode);                 // set to send/receive single byte of data
  char read_byte;
  digitalWrite(CS, LOW);                         // set SPI slave select LOW;
  SPI.transfer(READ);                            // send READ command to memory chip
  SPI.transfer((byte)(address >> 16));           // send high byte of address
  SPI.transfer((byte)(address >> 8));            // send middle byte of address
  SPI.transfer((byte)address);                   // send low byte of address
  read_byte = SPI.transfer(0x00);                // read the byte at that address
  digitalWrite(CS, HIGH);                        // set SPI slave select HIGH;
  return read_byte;                              // send data back to the calling function
}

/*********** Sequential data transfer functions using Arrays ************************/
void SRAMsimple::WriteByteArray(uint32_t address, byte *data, uint16_t big){
  SetMode(CS,Sequential);                // set to send/receive multiple bytes of data
  digitalWrite(CS, LOW);                          // start new command sequence
  SPI.transfer(WRITE);                            // send WRITE command
  SPI.transfer((byte)(address >> 16));            // send high byte of address
  SPI.transfer((byte)(address >> 8));             // send middle byte of address
  SPI.transfer((byte)address);                    // send low byte of address
  SPI.transfer(data, big);                        // transfer an array of data => needs array name & size
  digitalWrite(CS, HIGH);                         // set SPI slave select HIGH
}

void SRAMsimple::ReadByteArray(uint32_t address, byte *data, uint16_t big){
  SetMode(CS,Sequential);                         // set to send/receive multiple bytes of data
  digitalWrite(CS, LOW);                          // start new command sequence
  SPI.transfer(READ);                             // send READ command
  SPI.transfer((byte)(address >> 16));            // send high byte of address
  SPI.transfer((byte)(address >> 8));             // send middle byte of address
  SPI.transfer((byte)address);                    // send low byte of address
  for(uint16_t i=0; i<big; i++){
    data[i] = SPI.transfer(0x00);                 // read the data byte
  }
  digitalWrite(CS, HIGH);                         // set SPI slave select HIGH
}

/*********** Write and Read an Integer (2 bytes) ************************/
void SRAMsimple::WriteInt(uint32_t address, int data){
  SetMode(CS,Sequential);                         // set to send/receive multiple bytes of data
  byte temp[2]; 				  // temporary array of bytes with 2 elements
  temp[0]=(byte)(data>>8);                        // high byte of integer
  temp[1]=(byte)(data);                           // low byte of integer
  digitalWrite(CS, LOW);                          // start new command sequence
  SPI.transfer(WRITE);                            // send WRITE command
  SPI.transfer((byte)(address >> 16));            // send high byte of address
  SPI.transfer((byte)(address >> 8));             // send middle byte of address
  SPI.transfer((byte)address);                    // send low byte of address
  SPI.transfer(temp, 2);                          // transfer an array of data => needs array name & size (2 elements)
  digitalWrite(CS, HIGH);                         // set SPI slave select HIGH
}

int SRAMsimple::ReadInt(uint32_t address){
  SetMode(CS,Sequential);                         // set to send/receive multiple bytes of data
  byte temp[2]; 				  // temporary array of bytes with 2 elements
  int data=0;
  digitalWrite(CS, LOW);                          // start new command sequence
  SPI.transfer(READ);                             // send READ command
  SPI.transfer((byte)(address >> 16));            // send high byte of address
  SPI.transfer((byte)(address >> 8));             // send middle byte of address
  SPI.transfer((byte)address);                    // send low byte of address
  for(uint16_t i=0; i<2; i++){
    temp[i] = SPI.transfer(0x00);                 // read the data byte
  }
  digitalWrite(CS, HIGH);                         // set SPI slave select HIGH
  data=((int)temp[0]<<8)+temp[1];                      // data=high byte & low byte
  return data;
}

/*********** Write and Read an Integer Array ************************/
void SRAMsimple::WriteIntArray(uint32_t address, int *data, uint16_t big){
  SetMode(CS,Sequential);                         // set to send/receive multiple bytes of data
  byte temp[big*2]; 				  // temporary array of bytes with 2 elements
  uint16_t j=0;                                   // byte counter
  for(uint16_t i=0; i<big; i++){                   // take apart integers into high and low bytes
    temp[j]=(byte)(data[i]>>8);                   // high byte of integer
    temp[j+1]=(byte)(data[i]);                    // low byte of integer
    j+=2;                                         // increment counter
  }
  digitalWrite(CS, LOW);                          // start new command sequence
  SPI.transfer(WRITE);                            // send WRITE command
  SPI.transfer((byte)(address >> 16));            // send high byte of address
  SPI.transfer((byte)(address >> 8));             // send middle byte of address
  SPI.transfer((byte)address);                    // send low byte of address
  SPI.transfer(temp, big*2);                      // transfer an array of data => needs array name & size (2 elements)
  digitalWrite(CS, HIGH);                         // set SPI slave select HIGH
}

void SRAMsimple::ReadIntArray(uint32_t address, int *data, uint16_t big){
  SetMode(CS,Sequential);                         // set to send/receive multiple bytes of data
  byte temp[big*2]; 				  // temporary array of bytes with 2 elements
  uint16_t j=0;                                   // byte counter
  digitalWrite(CS, LOW);                          // start new command sequence
  SPI.transfer(READ);                             // send READ command
  SPI.transfer((byte)(address >> 16));            // send high byte of address
  SPI.transfer((byte)(address >> 8));             // send middle byte of address
  SPI.transfer((byte)address);                    // send low byte of address
  for(uint16_t i=0; i<(big*2); i++){
    temp[i] = SPI.transfer(0x00);                 // read the data byte
  }
  digitalWrite(CS, HIGH);                         // set SPI slave select HIGH
  for(uint16_t i=0; i<big; i++){
    data[i]=((int)temp[j]<<8)+temp[j+1];          // data=high byte & low byte
    j+=2;                                         // increment counter
  }
}

/*********** Write and Read an Unsigned Integer (2 bytes) ************************/
void SRAMsimple::WriteUnsignedInt(uint32_t address, unsigned int data){ 
  SetMode(CS,Sequential);                         // set to send/receive multiple bytes of data
  byte temp[2]; 				  // temporary array of bytes with 2 elements
  temp[0]=(byte)(data>>8);                        // high byte of integer
  temp[1]=(byte)(data);                           // low byte of integer
  digitalWrite(CS, LOW);                          // start new command sequence
  SPI.transfer(WRITE);                            // send WRITE command
  SPI.transfer((byte)(address >> 16));            // send high byte of address
  SPI.transfer((byte)(address >> 8));             // send middle byte of address
  SPI.transfer((byte)address);                    // send low byte of address
  SPI.transfer(temp, 2);                          // transfer an array of data => needs array name & size (2 elements)
  digitalWrite(CS, HIGH);                         // set SPI slave select HIGH
}

unsigned int SRAMsimple::ReadUnsignedInt(uint32_t address){
  SetMode(CS,Sequential);                         // set to send/receive multiple bytes of data
  byte temp[2]; 				  // temporary array of bytes with 2 elements
  unsigned int data=0;
  digitalWrite(CS, LOW);                          // start new command sequence
  SPI.transfer(READ);                             // send READ command
  SPI.transfer((byte)(address >> 16));            // send high byte of address
  SPI.transfer((byte)(address >> 8));             // send middle byte of address
  SPI.transfer((byte)address);                    // send low byte of address
  for(uint16_t i=0; i<2; i++){
    temp[i] = SPI.transfer(0x00);                 // read the data byte
  }
  digitalWrite(CS, HIGH);                         // set SPI slave select HIGH
  data=(temp[0]<<8)+temp[1];                      // data=high byte & low byte
  return data;
}

/*********** Write and Read an Unsigned Integer Array ************************/
void SRAMsimple::WriteUnsignedIntArray(uint32_t address, unsigned int *data, uint16_t big){
  SetMode(CS,Sequential);                         // set to send/receive multiple bytes of data
  byte temp[big*2]; 				  // temporary array of bytes with 2 elements
  uint16_t j=0;                                   // byte counter
  for(uint16_t i=0; i<big; i++){                   // take apart integers into high and low bytes
    temp[j]=(byte)(data[i]>>8);                   // high byte of integer
    temp[j+1]=(byte)(data[i]);                    // low byte of integer
    j+=2;                                         // increment counter
  }
  digitalWrite(CS, LOW);                          // start new command sequence
  SPI.transfer(WRITE);                            // send WRITE command
  SPI.transfer((byte)(address >> 16));            // send high byte of address
  SPI.transfer((byte)(address >> 8));             // send middle byte of address
  SPI.transfer((byte)address);                    // send low byte of address
  SPI.transfer(temp, big*2);                      // transfer an array of data => needs array name & size (2 elements)
  digitalWrite(CS, HIGH);                         // set SPI slave select HIGH
}

void SRAMsimple::ReadUnsignedIntArray(uint32_t address, unsigned int *data, uint16_t big){
  SetMode(CS,Sequential);                         // set to send/receive multiple bytes of data
  byte temp[big*2]; 				  // temporary array of bytes with 2 elements
  uint16_t j=0;                                   // byte counter
  digitalWrite(CS, LOW);                          // start new command sequence
  SPI.transfer(READ);                             // send READ command
  SPI.transfer((byte)(address >> 16));            // send high byte of address
  SPI.transfer((byte)(address >> 8));             // send middle byte of address
  SPI.transfer((byte)address);                    // send low byte of address
  for(uint16_t i=0; i<(big*2); i++){
    temp[i] = SPI.transfer(0x00);                 // read the data byte
  }
  digitalWrite(CS, HIGH);                         // set SPI slave select HIGH
  for(uint16_t i=0; i<big; i++){
    data[i]=((int)temp[j]<<8)+temp[j+1];          // data=high byte & low byte
    j+=2;                                         // increment counter
  }
}

/*********** Write and Read a Long (4 bytes) ************************/
void SRAMsimple::WriteLong(uint32_t address, long data){  
  SetMode(CS,Sequential);                         // set to send/receive multiple bytes of data
  byte temp[4]; 				  // temporary array of bytes with 4 elements
  temp[0]=(byte)(data >> 24);                     // high byte of integer
  temp[1]=(byte)(data >> 16);                     // low byte of integer
  temp[2]=(byte)(data >> 8);                      // low byte of integer
  temp[3]=(byte)(data);                           // low byte of integer
  digitalWrite(CS, LOW);                          // start new command sequence
  SPI.transfer(WRITE);                            // send WRITE command
  SPI.transfer((byte)(address >> 16));            // send high byte of address
  SPI.transfer((byte)(address >> 8));             // send middle byte of address
  SPI.transfer((byte)address);                    // send low byte of address
  SPI.transfer(temp, 4);                          // transfer an array of data => needs array name & size (4 elements)
  digitalWrite(CS, HIGH);                         // set SPI slave select HIGH
}

long SRAMsimple::ReadLong(uint32_t address){
  SetMode(CS,Sequential);                         // set to send/receive multiple bytes of data
  byte temp[4]; 				  // temporary array of bytes with 4 elements
  long data=0;
  digitalWrite(CS, LOW);                          // start new command sequence
  SPI.transfer(READ);                             // send READ command
  SPI.transfer((byte)(address >> 16));            // send high byte of address
  SPI.transfer((byte)(address >> 8));             // send middle byte of address
  SPI.transfer((byte)address);                    // send low byte of address
  for(uint16_t i=0; i<4; i++){
    temp[i] = SPI.transfer(0x00);                 // read the data byte
  }
  digitalWrite(CS, HIGH);                         // set SPI slave select HIGH
  data=((long)temp[0]<<24)+((long)temp[1]<<16)+((long)temp[2]<<8)+temp[3];   // reassemble bytes into long
  return data;
}

/*********** Write and Read a Long Array ************************/
void SRAMsimple::WriteLongArray(uint32_t address, long *data, uint16_t big){
  SetMode(CS,Sequential);                         // set to send/receive multiple bytes of data
  byte temp[big*4]; 				  // temporary array of bytes with 2 elements
  uint16_t j=0;                                   // byte counter
  for(uint16_t i=0; i<big; i++){                  // take apart integers into high and low bytes
    temp[j]=(byte)(data[i] >> 24);                // high byte of integer
    temp[j+1]=(byte)(data[i] >> 16);              // low byte of integer
    temp[j+2]=(byte)(data[i] >> 8);               // low byte of integer
    temp[j+3]=(byte)data[i];                      // low byte of integer
    j+=4;                                         // increment counter
  }
  digitalWrite(CS, LOW);                          // start new command sequence
  SPI.transfer(WRITE);                            // send WRITE command
  SPI.transfer((byte)(address >> 16));            // send high byte of address
  SPI.transfer((byte)(address >> 8));             // send middle byte of address
  SPI.transfer((byte)address);                    // send low byte of address
  SPI.transfer(temp, big*4);                      // transfer an array of data => needs array name & size (2 elements)
  digitalWrite(CS, HIGH);                         // set SPI slave select HIGH
}

void SRAMsimple::ReadLongArray(uint32_t address, long *data, uint16_t big){
  SetMode(CS,Sequential);                         // set to send/receive multiple bytes of data
  byte temp[big*4]; 				  // temporary array of bytes with 2 elements
  uint16_t j=0;                                   // byte counter
  digitalWrite(CS, LOW);                          // start new command sequence
  SPI.transfer(READ);                             // send READ command
  SPI.transfer((byte)(address >> 16));            // send high byte of address
  SPI.transfer((byte)(address >> 8));             // send middle byte of address
  SPI.transfer((byte)address);                    // send low byte of address
  for(uint16_t i=0; i<(big*4); i++){
    temp[i] = SPI.transfer(0x00);                 // read the data byte
  }
  digitalWrite(CS, HIGH);                         // set SPI slave select HIGH
  for(uint16_t i=0; i<big; i++){
    data[i]=((long)temp[j]<<24)+((long)temp[j+1]<<16)+((long)temp[j+2]<<8)+temp[j+3];  // put together 4 bytes
    j+=4;                                         // increment counter
  }
}

/*********** Write and Read an Unsigned Long (4 bytes) ************************/
void SRAMsimple::WriteUnsignedLong(uint32_t address, unsigned long data){
  SetMode(CS,Sequential);                         // set to send/receive multiple bytes of data
  byte temp[4]; 			 	  // temporary array of bytes with 4 elements
  temp[0]=(byte)(data >> 24);                     // high byte of integer
  temp[1]=(byte)(data >> 16);                     // low byte of integer
  temp[2]=(byte)(data >> 8);                      // low byte of integer
  temp[3]=(byte)(data);                           // low byte of integer
  digitalWrite(CS, LOW);                          // start new command sequence
  SPI.transfer(WRITE);                            // send WRITE command
  SPI.transfer((byte)(address >> 16));            // send high byte of address
  SPI.transfer((byte)(address >> 8));             // send middle byte of address
  SPI.transfer((byte)address);                    // send low byte of address
  SPI.transfer(temp, 4);                          // transfer an array of data => needs array name & size (4 elements)
  digitalWrite(CS, HIGH);                         // set SPI slave select HIGH
}

unsigned long SRAMsimple::ReadUnsignedLong(uint32_t address){
  SetMode(CS,Sequential);                         // set to send/receive multiple bytes of data
  byte temp[4]; 				  // temporary array of bytes with 4 elements
  unsigned long data=0;
  digitalWrite(CS, LOW);                          // start new command sequence
  SPI.transfer(READ);                             // send READ command
  SPI.transfer((byte)(address >> 16));            // send high byte of address
  SPI.transfer((byte)(address >> 8));             // send middle byte of address
  SPI.transfer((byte)address);                    // send low byte of address
  for(uint16_t i=0; i<4; i++){
    temp[i] = SPI.transfer(0x00);                 // read the data byte
  }
  digitalWrite(CS, HIGH);                         // set SPI slave select HIGH
  data=((long)temp[0]<<24)+((long)temp[1]<<16)+((long)temp[2]<<8)+temp[3];   // reassemble bytes into long
  return data;
}

/*********** Write and Read a Long Array ************************/
void SRAMsimple::WriteUnsignedLongArray(uint32_t address, unsigned long *data, uint16_t big){
  SetMode(CS,Sequential);                         // set to send/receive multiple bytes of data
  byte temp[big*4]; 				  // temporary array of bytes with 2 elements
  uint16_t j=0;                                   // byte counter
  for(uint16_t i=0; i<big; i++){                  // take apart integers into high and low bytes
    temp[j]=(byte)(data[i] >> 24);                // high byte of integer
    temp[j+1]=(byte)(data[i] >> 16);              // low byte of integer
    temp[j+2]=(byte)(data[i] >> 8);               // low byte of integer
    temp[j+3]=(byte)data[i];                      // low byte of integer
    j+=4;                                         // increment counter
  }
  digitalWrite(CS, LOW);                          // start new command sequence
  SPI.transfer(WRITE);                            // send WRITE command
  SPI.transfer((byte)(address >> 16));            // send high byte of address
  SPI.transfer((byte)(address >> 8));             // send middle byte of address
  SPI.transfer((byte)address);                    // send low byte of address
  SPI.transfer(temp, big*4);                      // transfer an array of data => needs array name & size (2 elements)
  digitalWrite(CS, HIGH);                         // set SPI slave select HIGH
}

void SRAMsimple::ReadUnsignedLongArray(uint32_t address, unsigned long *data, uint16_t big){
  SetMode(CS,Sequential);                         // set to send/receive multiple bytes of data
  byte temp[big*4]; 				  // temporary array of bytes with 2 elements
  uint16_t j=0;                                   // byte counter
  digitalWrite(CS, LOW);                          // start new command sequence
  SPI.transfer(READ);                             // send READ command
  SPI.transfer((byte)(address >> 16));            // send high byte of address
  SPI.transfer((byte)(address >> 8));             // send middle byte of address
  SPI.transfer((byte)address);                    // send low byte of address
  for(uint16_t i=0; i<(big*4); i++){
    temp[i] = SPI.transfer(0x00);                 // read the data byte
  }
  digitalWrite(CS, HIGH);                         // set SPI slave select HIGH
  for(uint16_t i=0; i<big; i++){
    data[i]=((long)temp[j]<<24)+((long)temp[j+1]<<16)+((long)temp[j+2]<<8)+temp[j+3];  // put together 4 bytes
    j+=4;                                         // increment counter
  }
}

/*********** Write and Read a Float (4 bytes) ************************/
void SRAMsimple::WriteFloat(uint32_t address, float data){
  SetMode(CS,Sequential);                         // set to send/receive multiple bytes of data
  byte *temp=(byte *)&data;                       // split float into 4 bytes
  digitalWrite(CS, LOW);                          // start new command sequence
  SPI.transfer(WRITE);                            // send WRITE command
  SPI.transfer((byte)(address >> 16));            // send high byte of address
  SPI.transfer((byte)(address >> 8));             // send middle byte of address
  SPI.transfer((byte)address);                    // send low byte of address
  SPI.transfer(temp, 4);                          // transfer an array of data => needs array name & size (4 elements)
  digitalWrite(CS, HIGH);                         // set SPI slave select HIGH
}

float SRAMsimple::ReadFloat(uint32_t address){
  SetMode(CS,Sequential);                         // set to send/receive multiple bytes of data
  byte temp[4]; 				  // temporary array of bytes with 4 elements
  float data=0;
  digitalWrite(CS, LOW);                          // start new command sequence
  SPI.transfer(READ);                             // send READ command
  SPI.transfer((byte)(address >> 16));            // send high byte of address
  SPI.transfer((byte)(address >> 8));             // send middle byte of address
  SPI.transfer((byte)address);                    // send low byte of address
  for(uint16_t i=0; i<4; i++){
    temp[i] = SPI.transfer(0x00);                 // read the data byte
  }
  digitalWrite(CS, HIGH);                         // set SPI slave select HIGH
  data = *(float *)&temp;                         // 4 bytes to a float.
  return data;                                    // https://www.microchip.com/forums/m590535.aspx
}

/*********** Write and Read a Float Array ************************/
void SRAMsimple::WriteFloatArray(uint32_t address, float *data, uint16_t big){
  byte holder[big*4]; 				  // temporary array of bytes with 2 elements
  uint16_t j=0;
  for(int i=0;i<big;i++){
    byte *temp=(byte *)&data[i];                  // split float into 4 bytes
    holder[j]=temp[0];
    holder[j+1]=temp[1];
    holder[j+2]=temp[2];
    holder[j+3]=temp[3];
    j+=4;
  }
  SetMode(CS,Sequential);                         // set to send/receive multiple bytes of data
  digitalWrite(CS, LOW);                          // start new command sequence
  SPI.transfer(WRITE);                            // send WRITE command
  SPI.transfer((byte)(address >> 16));            // send high byte of address
  SPI.transfer((byte)(address >> 8));             // send middle byte of address
  SPI.transfer((byte)address);                    // send low byte of address
  SPI.transfer(holder, big*4);                    // transfer an array of data => needs array name & size (4 elements)
  digitalWrite(CS, HIGH);                         // set SPI slave select HIGH
}

void SRAMsimple::ReadFloatArray(uint32_t address, float *data, uint16_t big){
  SetMode(CS,Sequential);                         // set to send/receive multiple bytes of data
  byte holder[big*4]; 				  // temporary array of bytes
  byte t[4];                                      // to hold 4 bytes
  digitalWrite(CS, LOW);                          // start new command sequence
  SPI.transfer(READ);                             // send READ command
  SPI.transfer((byte)(address >> 16));            // send high byte of address
  SPI.transfer((byte)(address >> 8));             // send middle byte of address
  SPI.transfer((byte)address);                    // send low byte of address
  for(uint16_t i=0; i<(big*4); i++){
    holder[i] = SPI.transfer(0x00);               // read the data byte
  }
  digitalWrite(CS, HIGH);                         // set SPI slave select HIGH
  uint16_t j=0;
  for(int i=0;i<big;i++){ 
    t[0]=holder[j];
    t[1]=holder[j+1];
    t[2]=holder[j+2];
    t[3]=holder[j+3];
    data[i] = *(float *)&t;                        // 4 bytes to a float.
    j+=4;
  }
}
