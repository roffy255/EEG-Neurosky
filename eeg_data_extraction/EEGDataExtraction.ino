// define variables
#define START_BYTE 0xAA
#define RAW_WAVE_16BIT 0x80
#define ASIC_EEG_POWER_INT 0x83

uint8_t plength;
int16_t rawEEG16Bit;
uint32_t delta, theta, lowAlpha, highAlpha, lowBeta, highBeta, lowGamma, midGamma;

byte calculatedchecksum, checksum;
byte payloaddata[170];

void setup(){
  Serial.begin(57600);    // should be this value
}

void loop(){
  if(readserial() == START_BYTE)
    if(readserial() == START_BYTE){
      // Ignore date if its length > 169
      plength = readserial();
      if(plength > 169)
        return;

      // Check validity of data using checksum byte
      calculatedchecksum = 0;
      for(int i = 0; i < plength; i++){
        payloaddata[i] = readserial();
        calculatedchecksum += payloaddata[i];
       }
       checksum = readserial();
       calculatedchecksum = 255 - calculatedchecksum;

       // Extract RawEEG and Power
       if(checksum == calculatedchecksum)
          CalculateRawEEGnPower();
    }
}

// Extract and calculate raw EEG and ASIC EEG power
void CalculateRawEEGnPower(){
  for(int i = 0; i < plength; i++){
      if(payloaddata[i] == RAW_WAVE_16BIT)
          rawEEG16Bit = Bytes2IntConverter(payloaddata[i+2],payloaddata[i+3]);                      

      if(payloaddata[i] == ASIC_EEG_POWER_INT){
          i++; // bytelength=payloaddata[i]; //extract byte length i.e. 24    
          
          // Calculate EEG power values - 8 Bytes with 3 Bytes for each power level
          delta = Bytes2LongConverter(payloaddata[i+1],payloaddata[i+2],payloaddata[i+3]);
          theta = Bytes2LongConverter(payloaddata[i+4],payloaddata[i+5],payloaddata[i+6]);
          lowAlpha = Bytes2LongConverter(payloaddata[i+7],payloaddata[i+8],payloaddata[i+9]);
          highAlpha = Bytes2LongConverter(payloaddata[i+10],payloaddata[i+11],payloaddata[i+12]);
          lowBeta = Bytes2LongConverter(payloaddata[i+13],payloaddata[i+14],payloaddata[i+15]);
          highBeta = Bytes2LongConverter(payloaddata[i+16],payloaddata[i+17],payloaddata[i+18]);
          lowGamma = Bytes2LongConverter(payloaddata[i+19],payloaddata[i+20],payloaddata[i+21]);
          midGamma = Bytes2LongConverter(payloaddata[i+22],payloaddata[i+23],payloaddata[i+24]);
          SerialPrintEEGPower();          
          return;
     }
  }
}

// convert 2 bytes values to int,could have negative values too
int16_t Bytes2IntConverter(byte value1, byte value2){
  uint16_t value = value1*256 + value2;
  int16_t valueReturn=0;
  if (value > 32768)
    valueReturn = value - 65536;
  else
    valueReturn = value;
    
   return valueReturn;
}

// convert 3 bytes values to long
uint32_t Bytes2LongConverter(byte value1, byte value2, byte value3){
   return (value1*65536 + value2*256 + value3);
}

//Packet reading function
byte readserial(){
  byte packet;
  while(!Serial.available());
  packet = Serial.read();
  return packet;
}

// Serial multiplots
void SerialPrintEEGPower(){
  Serial.print(delta); Serial.print(",");Serial.print(theta);
  Serial.print(lowAlpha); Serial.print(",");Serial.print(highAlpha);
  Serial.print(lowBeta); Serial.print(",");Serial.print(highBeta);
  Serial.print(lowGamma); Serial.print(",");Serial.println(midGamma);  
}

