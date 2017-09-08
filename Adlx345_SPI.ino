///Add the SPI library so we can communicate with the ADXL345 sensor
#include <SPI.h>

//Assign the Chip Select signal to pin 10.

/*Arduino nano Pin---> ADXL345     
pin 13 SCK  ---------> SCL
pin 12 MISO ---------> SDO
pin 11 MOSI ---------> SDA
pin #  CS   ---------> CS*/

///CS pins from arduino
int stack[4]={10,9,8,7}; // CS pin 10(shoulder Right); 9(elbow Right); 8(shoulder Left);7(elbow left);


unsigned long time;

//This is a list of some of the registers available on the ADXL345.
//To learn more about these and the rest of the registers on the ADXL345, read the datasheet!
char POWER_CTL = 0x2D;  //Power Control Register
char DATA_FORMAT = 0x31;
char DATAX0 = 0x32; //X-Axis Data 0
char DATAX1 = 0x33; //X-Axis Data 1
char DATAY0 = 0x34; //Y-Axis Data 0
char DATAY1 = 0x35; //Y-Axis Data 1
char DATAZ0 = 0x36; //Z-Axis Data 0
char DATAZ1 = 0x37; //Z-Axis Data 1

//This buffer will hold values read from the ADXL345 registers.
unsigned char values[10];

//These variables will be used to hold the x,y and z axis accelerometer values.
int counter=0,CS;
//double xg,yg,zg;

void setup(){ 
  //Initiate an SPI communication instance.
  SPI.begin();
  //Configure the SPI connection for the ADXL345.
  SPI.setDataMode(SPI_MODE3);
  //Create a serial connection to display the data on the terminal.
  Serial.begin(9600);
  
  //Set up the Chip Select pin to be an output from the Arduino.
  pinMode(stack[0], OUTPUT);
  pinMode(stack[1], OUTPUT);
  pinMode(stack[2], OUTPUT);
  pinMode(stack[3], OUTPUT);
  
  digitalWrite(stack[0],HIGH);
  digitalWrite(stack[1],HIGH);
  digitalWrite(stack[2],HIGH);
  digitalWrite(stack[3],HIGH);
  
  //Before communication starts, the Chip Select pin needs to be set high.
  
  //Put the first ADXL345 into +/- 16G range by writing the value 0x01 to the DATA_FORMAT register.
  writeRegister(DATA_FORMAT, 0x03);
  //Put the first ADXL345 into Measurement Mode by writing 0x08 to the POWER_CTL register.
  writeRegister(POWER_CTL, 0x08);  //Measurement mode  
  delay(20);

  
}

void loop(){
  //Reading 6 bytes of data starting at register DATAX0 will retrieve the x,y and z acceleration values from the ADXL345.
  //The results of the read operation will get stored to the values[] buffer.
  
  CS = stack[counter];  
   readRegister(DATAX0, 6, values);
  int x = (((int)values[1]) << 8) | values[0];   
  int y= (((int)values[3]) << 8) | values[2];
  int z = (((int)values[5]) << 8) | values[4];
  
  //Print the results to the terminal.
  time=millis();
  Serial.print(x);
  Serial.print(" ");
  Serial.print(y);
  Serial.print(" ");
  Serial.println(z);
  
  
  delay(500); ///// time Printing serial 

  
  switch(counter){
    case 0:
      Serial.println("shoulder Right");  
      break;
    case 1:
      Serial.println("Elbow Right");  
      break;
    case 2:
      Serial.println("shoulder Left");  
      break;
    case 3:
      Serial.println("Elbow Left");  
      break;
  }
    counter++; 
  if (counter ==4){
    counter=0;
  }
  
  ////////////////////////////////second sensor//////////////////////////////////////

}

//This function will write a value to a register on the ADXL345.
//Parameters:
//  char registerAddress - The register to write a value to
//  char value - The value to be written to the specified register.

void writeRegister(char registerAddress, char value){
  //Set Chip Select pin low to signal the beginning of an SPI packet.
  digitalWrite(CS, LOW);
  //Transfer the register address over SPI.
  SPI.transfer(registerAddress);
  //Transfer the desired register value over SPI.
  SPI.transfer(value);
  //Set the Chip Select pin high to signal the end of an SPI packet.
  digitalWrite(CS, HIGH);
}

//This function will read a certain number of registers starting from a specified address and store their values in a buffer.
//Parameters:
//  char registerAddress - The register addresse to start the read sequence from.
//  int numBytes - The number of registers that should be read.
//  char * values - A pointer to a buffer where the results of the operation should be stored.

void readRegister(char registerAddress, int numBytes, unsigned char * values){
  //Since we're performing a read operation, the most significant bit of the register address should be set.
  char address = 0x80 | registerAddress;
  //If we're doing a multi-byte read, bit 6 needs to be set as well.
  if(numBytes > 1)address = address | 0x40;
  
  //Set the Chip select pin low to start an SPI packet.
  
  
  digitalWrite(CS, LOW);
  
  //Transfer the starting register address that needs to be read.
  SPI.transfer(address);
  //Continue to read registers until we've read the number specified, storing the results to the input buffer.
  for(int i=0; i<numBytes; i++){
    values[i] = SPI.transfer(0x00);
  }
  //Set the Chips Select pin high to end the SPI packet.
  digitalWrite(CS, HIGH);
  
}
