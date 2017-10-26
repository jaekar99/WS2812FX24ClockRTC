#include <WS2812FX.h>
#include <Wire.h>
#define DS3231_I2C_ADDRESS 0x68
#define PIN      6
#define N_LEDS 24
int pins;
int oldpins = 8888;
int oldpinm = 8888;
int oldminute = 8888;
int br = 30;
int t;
int pinm;
long pinkleur;
int r;
long data[24];
int aa, bb, cc;
int pen;
int spd = 22;

int m; //alleen voor random test
int h; //alleen voor random test
long getal; //alleen voor random test

byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;

WS2812FX strip = WS2812FX(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);

byte bcdToDec(byte val)
{
  return ( (val / 16 * 10) + (val % 16) );
}

byte decToBcd(byte val)
{
  return ( (val / 10 * 16) + (val % 10) );
}

void setup()
{

  getal = analogRead(0) * analogRead(1); //alleen voor random test

  randomSeed(getal); //alleen voor random test

  Wire.begin();
  strip.begin();
  Serial.begin(9600);

  whiteout();

  h = random(23); //alleen voor random test
  m = random(59); //alleen voor random test

  //setDS3231time(30,16,2,1,13,11,16); //alleen voor eenmalig instellen tijd

  displayTime();
}


void loop() {

  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);

  if ((oldminute == 14) && (minute == 15)) {
    whiteout();
    pinstore();
    perkwartier(6, spd);
    delay(4000);
    pinrestore(spd);
  }

  if ((oldminute == 29) && (minute == 30)) {
    whiteout();
    pinstore();
    perkwartier(12, spd);
    delay(4000);
    pinrestore(spd);
  }

  if ((oldminute == 44) && (minute == 45)) {
    whiteout();
    pinstore();
    perkwartier(18, spd);
    delay(4000);
    pinrestore(spd);
  }

  if ((oldminute == 59) && (minute == 0)) {
    whiteout();
    pinstore();
    perkwartier(23, spd);
    delay(4000);
    pinrestore(spd);
  }

  if (hour > 11) {
    hour = hour - 12;
  }

  pins = map(second, 0, 57, 0, 23);
  pinm = map(minute, 0, 57, 0, 23);

  if (pins == oldpins) {
    br = br - 12;
  }
  else {
    br = 30;
  }

  for (t = 0; t <= 23; t++) {
    strip.setPixelColor(t, 0x050505);
  }

  oldpins = pins;

  strip.setPixelColor((hour * 2), 3, 0, 0);

  if (pins == (hour * 2)) {
    strip.setPixelColor(pins, 3, br, 0);
  }
  else {
    strip.setPixelColor(pins, 0, br, 0);
  }

  if (pinm == pins) {
    strip.setPixelColor(pinm, 30, br, 0);
  }
  else {
    strip.setPixelColor(pinm, 30, 0, 0);
  }

  if ((minute != oldminute) && (pinm == oldpinm)) {
    pinflash(pinm);
  }

  strip.show();

  oldpinm = pinm;

  oldminute = minute;

  //displayTime();

  delay (1000);
}


void perkwartier(int kw, int slower) {

  aa = 0;
  bb = 35;
  cc = 0;

  for (pen = 0; pen <= kw; pen++) {
    strip.setPixelColor(pen, aa, bb, cc);
    strip.show();

    if (pen < 7) {
      aa = aa + 5;
      bb = bb - 5;
      cc = 0;
    }

    if ((pen > 6) && (pen < 13)) {
      aa = aa - 5;
      cc = cc + 5;
    }

    if ((pen > 12) && (pen < 18)) {
      bb = bb + 5;
      cc = cc - 5;
    }

    if (pen > 17) {
      aa = aa - 1;
      cc = cc - 1;
    }
    delay(slower);
  }
}

void whiteout() {
  for (t = 0; t <= 23; t++) {
    strip.setPixelColor(t, 0x050505);
  }
  strip.show();
}

void pinstore() {
  for (t = 0; t <= 23; t++) {
    data[t] = strip.getPixelColor(t);
  }
}

void pinrestore(int slowness) {
  for (t = 23; t >= 0; t--) {
    strip.setPixelColor(t, data[t]);
    strip.show();
    delay (slowness);
  }
}

void pinflash(int curpin) {
  pinkleur = strip.getPixelColor(curpin);

  for (r = 30; r >= 0; r--) {
    strip.setPixelColor(curpin, r, 0, 0);
    strip.show();
    delay (1);
  }

  for (r = 0; r <= 30; r++) {
    strip.setPixelColor(curpin, r, 0, 0);
    strip.show();
    delay (1);
  }

  strip.setPixelColor(curpin, pinkleur);
  strip.show();
}

void readDS3231time(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year) {
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}

void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte
                   dayOfMonth, byte month, byte year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0);
  Wire.write(decToBcd(second));
  Wire.write(decToBcd(minute));
  Wire.write(decToBcd(hour));
  Wire.write(decToBcd(dayOfWeek));
  Wire.write(decToBcd(dayOfMonth));
  Wire.write(decToBcd(month));
  Wire.write(decToBcd(year));
  Wire.endTransmission();
}

void displayTime()
{
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
                 &year);
  Serial.print(hour, DEC);

  Serial.print(":");
  if (minute < 10)
  {
    Serial.print("0");
  }
  Serial.print(minute, DEC);
  Serial.print(":");
  if (second < 10)
  {
    Serial.print("0");
  }
  Serial.print(second, DEC);
  Serial.print(" ");
  Serial.print(dayOfMonth, DEC);
  Serial.print("/");
  Serial.print(month, DEC);
  Serial.print("/");
  Serial.print(year, DEC);
  Serial.print(" Day of week: ");
  switch (dayOfWeek) {
    case 1:
      Serial.println("Sunday");
      break;
    case 2:
      Serial.println("Monday");
      break;
    case 3:
      Serial.println("Tuesday");
      break;
    case 4:
      Serial.println("Wednesday");
      break;
    case 5:
      Serial.println("Thursday");
      break;
    case 6:
      Serial.println("Friday");
      break;
    case 7:
      Serial.println("Saturday");
      break;
  }
}
