#include <M5Stack.h>
#include <Wire.h>
#include <FastLED.h>

#define LED_PIN            2
#define NUM_LEDS_PER_STRIP     29
CRGB Leds[NUM_LEDS_PER_STRIP];

#define LF        0x0A //New Line
#define CR        0x0D //Carriage  return
#define SP        0x20 //Space
#define Sensitivity_PWM1  0x00 //ch2,ch1
#define Sensitivity_PWM2  0x01 //ch4,ch3
#define Sensitivity_PWM3  0x02 //ch6,ch5
#define Sensitivity_PWM4  0x03 //ch21,ch7
#define Sensitivity_PWM5  0x04 //ch9,ch8
#define Sensitivity_PWM6  0x05 //ch11,ch10
#define Sensitivity_PWM7  0x06 //ch13,ch12
#define Sensitivity_PWM8  0x07 //ch15,ch14
#define Sensitivity_PWM9  0x08 //ch17,ch16
#define Sensitivity_PWM10 0x09 //ch19,ch18
#define Sensitivity_PWM11   0x0A //ch20
#define GTRL1     0x0B
#define GTRL2     0x0C
#define Cal_CTRL    0x0D
#define Port_CTRL1    0x0E
#define Port_CTRL2    0x0F
#define Port_CTRL3    0x10
#define Port_CTRL4    0x11
#define Port_CTRL5    0x12
#define Port_CTRL6    0x13
#define Cal_HOLD1   0x14
#define Cal_HOLD2   0x15
#define Cal_HOLD3   0x16
#define Err_CTRL    0x17
#define Output1     0x20
#define Output2     0x21
#define Output3     0x22
#define Ref_wr_H    0x23
#define Ref_wr_L    0x24
#define Ref_wr_CH1    0x25
#define Ref_wr_CH2    0x26
#define Ref_wr_CH3    0x27
#define Sen_RD_CTRL   0x28
#define Sensitivity_RD  0x29
#define Rd_CH1      0x30
#define Rd_CH2      0x31
#define Rd_CH3      0x32
#define Sen_H     0x33
#define Sen_L     0x34
#define Ref_H     0x35
#define Ref_L     0x36
#define Rd_CH1      0x37
#define Rd_CH2      0x38
#define Rd_CH3      0x39
#define TS20_SLAVE_GND  0x6A //0xD4<<1 //ADD Pin = GND
#define TS20_SLAVE_VDD  0x7A //0xF4<<1 //ADD Pin = VDD

void  Init_TS20(void); //Initialize TS20

#define RESET_PIN 7 //Reset pin
#define EN_PIN    6 //I2C Enable Pin
#define Set_Bit(val, bitn)    (val |=(1<<(bitn)))
#define Clr_Bit(val, bitn)     (val&=~(1<<(bitn)))
#define Get_Bit(val, bitn)    (val &(1<<(bitn)) )

byte key[4];

float FaktorLow = 1;
float FaktorHigh = 1;
long ColorLow = (CRGB::Red);
long ColorHigh = (CRGB::Red);
int Tondauer = 100;

void setup() {

  M5.begin();
  Wire.begin(26, 5);       // join i2c bus (address optional for master)
  Serial.begin(115200);  // start serial for output (Spped)
  delay(100); //wait for 100[msec]
  Init_TS20(); //Initialize TS20
  delay(100); //wait for 100[msec]
  key[0] = 0;
  key[1] = 0;
  key[2] = 0;
  key[3] = 0;
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  FastLED.addLeds<NEOPIXEL, LED_PIN>(Leds, NUM_LEDS_PER_STRIP);
  FastLED.setBrightness(10);
  // pixels.begin();

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(50, 10);
  M5.Lcd.setTextColor(TFT_RED, TFT_BLACK);
  M5.Lcd.setTextSize(3);
  M5.Lcd.printf("Make:PIANO");
  M5.Lcd.setTextColor(TFT_GREEN, TFT_BLACK);

  M5.Lcd.setCursor(15,80);
  M5.Lcd.printf("Oktaven-Belegung");
  M5.Lcd.setCursor(60,170);
  M5.Lcd.printf("Umschaltung");
  M5.Lcd.setCursor(45,210);
  M5.Lcd.printf("li");
  M5.Lcd.setCursor(130,210);
  M5.Lcd.printf("back");
  M5.Lcd.setCursor(235,210);
  M5.Lcd.printf("re");

  for (int i = 0; i < 29; i++)
  {
    Leds[i] = CRGB::Red;
    // pixels.setPixelColor(i, pRed);
    delay(20);
    FastLED.show();
  }
  delay(1000);
  for (int i = 0; i < 29; i++)
  {
    Leds[i] = CRGB::Black;
    // FastLED.setPixelColor(i, pBlack);
    delay(10);
    FastLED.show();
  }
}


#define NOTE_D0 -1
#define NOTE_DL1 261
#define NOTE_DL15 277
#define NOTE_DL2 293
#define NOTE_DL25 311
#define NOTE_DL3 329
#define NOTE_DL4 349
#define NOTE_DL45 370
#define NOTE_DL5 392
#define NOTE_DL55 415
#define NOTE_DL6 440
#define NOTE_DL65 466
#define NOTE_DL7 494

#define NOTE_D1 524
#define NOTE_D15 554
#define NOTE_D2 588
#define NOTE_D25 622
#define NOTE_D3 660
#define NOTE_D4 698
#define NOTE_D45 740
#define NOTE_D5 784
#define NOTE_D55 831
#define NOTE_D6 880
#define NOTE_D65 932
#define NOTE_D7 988
#define NOTE_DH1 1048

void loop() {
  int data0, data1, data2, data3, data4, data5;
  delay(5);
  Wire.beginTransmission(TS20_SLAVE_GND); // sned ic slave address
  Wire.write(byte(Output1)); // sends register address
  Wire.endTransmission(); // stop transmitting
  Wire.requestFrom(TS20_SLAVE_GND, 3); // key data read (3 byte)
  data0 = Wire.read();
  data1 = Wire.read();
  data2 = Wire.read();
  Wire.beginTransmission(TS20_SLAVE_VDD); // sned ic slave address
  Wire.write(byte(Output1)); // sends register address
  Wire.endTransmission(); // stop transmitting
  Wire.requestFrom(TS20_SLAVE_VDD, 3); // key data read (3 byte)
  data3 = Wire.read();
  data4 = Wire.read();
  data5 = Wire.read();


  M5.Lcd.setCursor(1, 50);

  for (int i = 0; i < 29; i++)
  {
    Leds[i] = CRGB::Black;
    // pixels.setPixelColor(i, pBlack);
  }

  if ((data0 & 0x10) == 0x10) {
    Set_Bit(key[0], 0);
    Leds[0] = ColorLow;
    M5.Speaker.tone(NOTE_DL1*FaktorLow, Tondauer);
  } else {
    Leds[0] = CRGB::Black;
    Clr_Bit(key[0], 0);
  }
  if ((data0 & 0x20) == 0x20) {
    Set_Bit(key[0], 1);
    Leds[1] = ColorLow;
    M5.Speaker.tone(NOTE_DL15*FaktorLow, Tondauer);
  } else {
    Leds[1] = CRGB::Black;
    Clr_Bit(key[0], 1);
  }
  if ((data0 & 0x40) == 0x40) {
    Set_Bit(key[0], 2);
    Leds[2] = ColorLow;
    M5.Speaker.tone(NOTE_DL2*FaktorLow, Tondauer);
  } else {
    Leds[2] = CRGB::Black;
    Clr_Bit(key[0], 2);
  }
  if ((data1 & 0x01) == 0x01) {
    Set_Bit(key[0], 3);
    Leds[3] = ColorLow;
    M5.Speaker.tone(NOTE_DL25*FaktorLow, Tondauer);
  } else {
    Leds[3] = CRGB::Black;
    Clr_Bit(key[0], 3);
  }
  if ((data1 & 0x02) == 0x02) {
    Set_Bit(key[0], 4);
    Leds[4] = ColorLow;
    M5.Speaker.tone(NOTE_DL3*FaktorLow, Tondauer);
  } else {
    Leds[4] = CRGB::Black;
    Clr_Bit(key[0], 4);
  }

  if ((data2 & 0x04) == 0x04) {
    Set_Bit(key[0], 5);
    Leds[6] = ColorLow;
    M5.Speaker.tone(NOTE_DL4*FaktorLow, Tondauer);
  } else {
    Leds[6] = CRGB::Black;
    Clr_Bit(key[0], 5);
  }
  if ((data2 & 0x08) == 0x08) {
    Set_Bit(key[0], 6);
    Leds[7] = ColorLow;
    M5.Speaker.tone(NOTE_DL45*FaktorLow, Tondauer);
  } else {
    Leds[7] = CRGB::Black;
    Clr_Bit(key[0], 6);
  }
  if ((data2 & 0x10) == 0x10) {
    Set_Bit(key[0], 7);
    Leds[8] = ColorLow;
    M5.Speaker.tone(NOTE_DL5*FaktorLow, Tondauer);
  } else {
    Leds[8] = CRGB::Black;
    Clr_Bit(key[0], 7);
  }

  if ((data0 & 0x01) == 0x01) {
    Set_Bit(key[1], 0);
    Leds[9] = ColorLow;
    M5.Speaker.tone(NOTE_DL55*FaktorLow, Tondauer);
  } else {
    Leds[9] = CRGB::Black;
    Clr_Bit(key[1], 0);
  }
  if ((data0 & 0x02) == 0x02) {
    Set_Bit(key[1], 1);
    Leds[10] = ColorLow;
    M5.Speaker.tone(NOTE_DL6*FaktorLow, Tondauer);
  } else {
    Leds[10] = CRGB::Black;
    Clr_Bit(key[1], 1);
  }
  if ((data0 & 0x04) == 0x04) {
    Set_Bit(key[1], 2);
    Leds[11] = ColorLow;
    M5.Speaker.tone(NOTE_DL65*FaktorLow, Tondauer);
  } else {
    Leds[11] = CRGB::Black;
    Clr_Bit(key[1], 2);
  }
  if ((data0 & 0x08) == 0x08) {
    Set_Bit(key[1], 3);
    Leds[12] = ColorLow;
    M5.Speaker.tone(NOTE_DL7*FaktorLow, Tondauer);
  } else {
    Leds[12] = CRGB::Black;
    Clr_Bit(key[1], 3);
  }
  if ((data3 & 0x10) == 0x10) {
    Set_Bit(key[1], 4);
    Leds[14] = ColorHigh;
    M5.Speaker.tone(NOTE_D1*FaktorHigh, Tondauer);
  } else {
    Leds[14] = CRGB::Black;
    Clr_Bit(key[1], 4);
  }
  if ((data3 & 0x20) == 0x20) {
    Set_Bit(key[1], 5);
    Leds[15] = ColorHigh;
    M5.Speaker.tone(NOTE_D15*FaktorHigh, Tondauer);
  } else {
    Leds[15] = CRGB::Black;
    Clr_Bit(key[1], 5);
  }
  if ((data3 & 0x40) == 0x40) {
    Set_Bit(key[1], 6);
    Leds[16] = ColorHigh;
    M5.Speaker.tone(NOTE_D2*FaktorHigh, Tondauer);
  } else {
    Leds[16] = CRGB::Black;
    Clr_Bit(key[1], 6);
  }
  if ((data4 & 0x01) == 0x01) {
    Set_Bit(key[1], 7);
    Leds[17] = ColorHigh;
    M5.Speaker.tone(NOTE_D25*FaktorHigh, Tondauer);
  } else {
    Leds[17] = CRGB::Black;
    Clr_Bit(key[1], 7);
  }

  if ((data4 & 0x02) == 0x02) {
    Set_Bit(key[2], 0);
    Leds[18] = ColorHigh;
    M5.Speaker.tone(NOTE_D3*FaktorHigh, Tondauer);
  } else {
    Leds[18] = CRGB::Black;
    Clr_Bit(key[2], 0);
  }
  if ((data5 & 0x02) == 0x02) {
    Set_Bit(key[2], 1);
    Leds[20] = ColorHigh;
    M5.Speaker.tone(NOTE_D4*FaktorHigh, Tondauer);
  } else {
    Leds[20] = CRGB::Black;
    Clr_Bit(key[2], 1);
  }
  if ((data5 & 0x04) == 0x04) {
    Set_Bit(key[2], 2);
    Leds[21] = ColorHigh;
    M5.Speaker.tone(NOTE_D45*FaktorHigh, Tondauer);
  } else {
    Leds[21] = CRGB::Black;
    Clr_Bit(key[2], 2);
  }
  if ((data5 & 0x08) == 0x08) {
    Set_Bit(key[2], 3);
    Leds[22] = ColorHigh;
    M5.Speaker.tone(NOTE_D5*FaktorHigh, Tondauer);
  } else {
    Leds[22] = CRGB::Black;
    Clr_Bit(key[2], 3);
  }
  if ((data5 & 0x10) == 0x10) {
    Set_Bit(key[2], 4);
    Leds[23] = ColorHigh;
    M5.Speaker.tone(NOTE_D55*FaktorHigh, Tondauer);
  } else {
    Leds[23] = CRGB::Black;
    Clr_Bit(key[2], 4);
  }
  if ((data3 & 0x01) == 0x01) {
    Set_Bit(key[2], 5);
    Leds[24] = ColorHigh;
    M5.Speaker.tone(NOTE_D6*FaktorHigh, Tondauer);
  } else {
    Leds[24] = CRGB::Black;
    Clr_Bit(key[2], 5);
  }
  if ((data3 & 0x02) == 0x02) {
    Set_Bit(key[2], 6);
    Leds[25] = ColorHigh;
    M5.Speaker.tone(NOTE_D65*FaktorHigh, Tondauer);
  } else {
    Leds[25] = CRGB::Black;
    Clr_Bit(key[2], 6);
  }
  if ((data3 & 0x04) == 0x04) {
    Set_Bit(key[2], 7);
    Leds[26] = ColorHigh;
    M5.Speaker.tone(NOTE_D7*FaktorHigh, Tondauer);
  } else {
    Leds[26] = CRGB::Black;
    Clr_Bit(key[2], 7);
  }

  if ((data3 & 0x08) == 0x08) {
    Set_Bit(key[3], 0);
    Leds[27] = ColorHigh;
    M5.Speaker.tone(NOTE_DH1*FaktorHigh, Tondauer);
  } else {
    Leds[27] = CRGB::Black;
    Clr_Bit(key[3], 0);
  }

  delay(20);
  FastLED.show();
  M5.update();
  //Registerwahl  
  if (M5.BtnA.pressedFor(10, 200)) {
    FaktorLow = 0.5;
    ColorLow = (CRGB::Green);
  } else if (M5.BtnB.pressedFor(10, 200)) {
    FaktorLow = 1;
    FaktorHigh = 1;
    ColorLow = (CRGB::Red);
    ColorHigh = (CRGB::Red);    
  } else if (M5.BtnC.pressedFor(10, 200)) {
    FaktorHigh = 2;
    ColorHigh = (CRGB::Blue);    
  } else if (M5.BtnB.wasReleasefor(700)) {
    M5.Lcd.setCursor(0, 0);
  }
  if (FaktorLow == 1) {
    M5.Lcd.setCursor(10, 128);
    M5.Lcd.setTextColor(TFT_RED, TFT_BLACK);
    M5.Lcd.printf("c1 - h1");
  }
  if (FaktorLow == 0.5) {
    M5.Lcd.setCursor(10, 128);
    M5.Lcd.setTextColor(GREEN, TFT_BLACK);
    M5.Lcd.printf("c0 - h0");
  }   
  if (FaktorHigh == 1) {
    M5.Lcd.setCursor(180, 128);
    M5.Lcd.setTextColor(TFT_RED, TFT_BLACK);
    M5.Lcd.printf("c2 - c3");
  }
   if (FaktorHigh == 2) {
    M5.Lcd.setCursor(180, 128);
    M5.Lcd.setTextColor(BLUE, TFT_BLACK);
    M5.Lcd.printf("c3 - c4");
  }
}

void  Init_TS20(void)
{
  //------------------ Software Reset Enable (Set)----------------------
  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(GTRL2)); // sends register address
  Wire.write(byte(0x1A)); // sends register data
  Wire.endTransmission(); // stop transmitting

  //----------------- Port Control ------------------------------------
  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Port_CTRL1)); // sends register address
  Wire.write(byte(0x00)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Port_CTRL2)); // sends register address
  Wire.write(byte(0x00)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Port_CTRL3)); // sends register address
  Wire.write(byte(0x00)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Port_CTRL4)); // sends register address
  Wire.write(byte(0x00)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Port_CTRL5)); // sends register address
  Wire.write(byte(0x00)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Port_CTRL6)); // sends register address
  Wire.write(byte(0x00)); // sends register data
  Wire.endTransmission(); // stop transmitting

  //--------------- Sensitivty control,(threshold level)-----------
  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Sensitivity_PWM1)); // sends register address
  Wire.write(byte(0x55)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Sensitivity_PWM2)); // sends register address
  Wire.write(byte(0x55)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Sensitivity_PWM3)); // sends register address
  Wire.write(byte(0x55)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Sensitivity_PWM4)); // sends register address
  Wire.write(byte(0xF5)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Sensitivity_PWM5)); // sends register address
  Wire.write(byte(0x55)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Sensitivity_PWM6)); // sends register address
  Wire.write(byte(0x55)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Sensitivity_PWM7)); // sends register address
  Wire.write(byte(0x55)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Sensitivity_PWM8)); // sends register address
  Wire.write(byte(0x55)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); /// send ic slave address
  Wire.write(byte(Sensitivity_PWM9)); // sends register address
  Wire.write(byte(0x55)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Sensitivity_PWM10)); // sends register address
  Wire.write(byte(0x55)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // sned ic slave address
  Wire.write(byte(Sensitivity_PWM11)); // sends register address
  Wire.write(byte(0x55)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(GTRL1)); // sends register address
  Wire.write(byte(0x4B)); // sends register data
  Wire.endTransmission(); // stop transmitting
  //------------------ Calibration On ---------------------------
  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Cal_HOLD1)); // sends register address
  Wire.write(byte(0x00)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Cal_HOLD2)); // sends register address
  Wire.write(byte(0x00)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Cal_HOLD3)); // sends register address
  Wire.write(byte(0x00)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Err_CTRL)); // sends register address
  Wire.write(byte(0x0D)); // sends register data
  Wire.endTransmission(); // stop transmitting

  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(Cal_CTRL)); // sends register address
  Wire.write(byte(0xFA)); // sends register data
  Wire.endTransmission(); // stop transmitting

  //------------------ Software Reset Disable (Clear) -----------------------
  Wire.beginTransmission(TS20_SLAVE_GND); // send ic slave address
  Wire.write(byte(GTRL2)); // sends register address
  Wire.write(byte(0x12)); // sends register data
  Wire.endTransmission(); // stop transmitting
}
