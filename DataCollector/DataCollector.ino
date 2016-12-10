#include "DHT.h"
#include <SoftwareSerial.h>
#include "RunningAverage.h"

#define UNSIGNED_LONG_MAX 4294967295UL

char DataString[90];

#pragma region Settings
struct SettingsStruct {
  unsigned long MicroclimatProgrev; //Время на прогрев датчика
  unsigned long MicroclimatInterval; //Интервал опроса датчика
  int MicroclimatSMAPoints; //Колличиство отсчётов в фильтре НЧ

  unsigned long CO2Progrev;
  unsigned long CO2Interval;
  int CO2SMAPoints;

  unsigned long COProgrev;
  int COSMAPoints;

  unsigned long DustProgrev;
  unsigned long DustInterval;
  int DustSMAPoints;

  unsigned long LPGProgrev;
  unsigned long LPGInterval;
  int LPGSMAPoints;

  unsigned int SendDataInterval;
} settings = {
  2000,
  2000,
  10,

  2000,
  2000,
  10,

  5000,
  10,

  1000,
  500,
  15,

  300000UL, // 5 минут
  200,
  10,

  1000
};

#pragma endregion

#pragma region Globals
#define DHTPIN 12
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);

SoftwareSerial SoftSerialCO2(10, 11); // RX, TX
SoftwareSerial SoftSerialCO(8, 9); // RX, TX
#pragma endregion


#pragma region Microclimat
unsigned long prev_micro = 0;

RunningAverage temperatureSMA(settings.MicroclimatSMAPoints);
RunningAverage humiditySMA(settings.MicroclimatSMAPoints);
void ProcessMicroclimat()
{
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (!isnan(t)) temperatureSMA.addValue(t);
  if (!isnan(h)) humiditySMA.addValue(h);
}
#pragma endregion 
#pragma region CO2
unsigned long prev_co2 = 0;

RunningAverage CO2SMA(settings.CO2SMAPoints);
void ProcessCO2()
{
  SoftSerialCO2.listen();
  while (SoftSerialCO2.available()) SoftSerialCO2.read();
  const byte request[] = { 0xff, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79 };
  byte result[9];
  byte checksum = 0;
  SoftSerialCO2.write(request,sizeof(request));
  while (!SoftSerialCO2.available()) {} 
  for (byte i = 0; i < 9; i++)
  {
    delay(5);
    result[i] = SoftSerialCO2.read();
    if ((i>0) && (i<8)) checksum += result[i];
  }
  checksum = (~checksum) + 1;
  
  if (checksum == result[8])
  {
    int temp = result[2] * 256 + result[3];
    CO2SMA.addValue(temp);
  }
  SoftSerialCO.listen();
}
#pragma endregion
#pragma region CO
volatile unsigned long COReciveTime = 0;
RunningAverage COSMA(settings.COSMAPoints);
bool COReciveFlag = false;
void COCallback()
{
	COReciveFlag = true;
	COReciveTime = millis();
}
void ProcessCO()
{
	byte result[9];
	while (!SoftSerialCO.available()) {}
	byte checksum = 0;
	for (byte i = 0; i < 9; i++)
	{
		delay(5);
		result[i] = SoftSerialCO.read();
		if ((i>0) && (i<8)) checksum += result[i];
	}
	checksum = (~checksum) + 1;

	if (checksum == result[8])
	{
		float temp = (result[4] * 256 + result[5])*0.1;
		COSMA.addValue(temp);
	}
}
#pragma endregion
#pragma region Dust
#define samplingTime 280
#define deltaTime 40

unsigned long prev_dust = 0;

RunningAverage DustSMA(settings.DustSMAPoints);

void ProcessDust()
{
  digitalWrite(2, LOW); // Подаём питание на LED
  delayMicroseconds(samplingTime);

  float voMeasured = analogRead(A1);

  delayMicroseconds(deltaTime);
  digitalWrite(2, HIGH); // Выключаем

  float calcVoltage = voMeasured * (5.0 / 1023.0);
  float dustDensity = 0.167255 * calcVoltage - 0.0921552;
  if(dustDensity>0) DustSMA.addValue(dustDensity);
  else DustSMA.addValue(0);
}
#pragma endregion
#pragma region LPG
unsigned long prev_lpg = 0;

float LPGCurve[2] = {13.91826722,-0.320860296};
float RL_VALUE = 1;
float Ro = 1.22;


RunningAverage LPGSMA(settings.LPGSMAPoints);

void ProcessLPG()
{
  if (!temperatureSMA.FullBuffer()) return; //Продолжаем только если у нас есть данные о микроклимате
  float voMeasured = analogRead(A0);

  float vcc = readVcc();
  float vrl = voMeasured*(vcc / 1023);
  float resistance = (vcc - vrl) / vrl*RL_VALUE;

  resistance = resistance*(1.147618596*pow(2.71828182846, ((-0.00550023923)*temperatureSMA.getFastAverage()))); //Компенсация температуры
  resistance = resistance + (33.0 - humiditySMA.getFastAverage())*0.002884615; //Компенсация влажности
  float ratio = resistance / Ro;
  
  float result = LPGCurve[0] * pow(ratio,LPGCurve[1]);

  LPGSMA.addValue(result);
}

float readVcc()  //this function actual only for ATMega328
{
  byte i;
  float result = 0.0;
  float tmp = 0.0;

  for (i = 0; i < 5; i++) 
  {
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
 
    delay(3);
    ADCSRA |= _BV(ADSC);
    while (bit_is_set(ADCSRA,ADSC));

    uint8_t low  = ADCL;
    uint8_t high = ADCH;

    tmp = (high<<8) | low;
    tmp = (1.1 * 1023.0) / tmp;
    result = result + tmp;
    delay(5);
  }

  result = result / 5;
  return result;
}


#pragma endregion
#pragma region SoftTX
SoftwareSerial SoftTransmitter(A4, A5); // RX, TX
unsigned long send_prev = 0;

void MakeDataString()
{
  DataString[0] = '\0';
  char tempNum[6];
    if (temperatureSMA.FullBuffer()) //Микроклимат
    {
      dtostrf(temperatureSMA.getAverage(), 6, 2, tempNum);
      trim(tempNum);
      strcat(DataString, tempNum);
      strcat(DataString, ";");
      dtostrf(temperatureSMA.GetStandardError(), 6, 2, tempNum);
      trim(tempNum);
      strcat(DataString, tempNum);
      strcat(DataString, ";");
      dtostrf(humiditySMA.getAverage(), 6, 2, tempNum);
      trim(tempNum);
      strcat(DataString, tempNum);
      strcat(DataString, ";");
      dtostrf(humiditySMA.GetStandardError(), 6, 2, tempNum);
      trim(tempNum);
      strcat(DataString, tempNum);
      strcat(DataString, "|");
    }
    else
    {
      strcat(DataString, "-1;-1;-1;-1");
    }

    if (CO2SMA.FullBuffer()) //CO2
    {
      dtostrf(CO2SMA.getFastAverage(), 7, 2, tempNum);
      trim(tempNum);
      strcat(DataString, tempNum);
      strcat(DataString, ";");
      dtostrf(CO2SMA.GetStandardError(), 7, 2, tempNum);
      trim(tempNum);
      strcat(DataString, tempNum);
      strcat(DataString, "|");
    }
    else
    {
      strcat(DataString, "-1;-1|");
    }

    if (COSMA.FullBuffer()) //CO
    {
      dtostrf(COSMA.getFastAverage(), 6, 2, tempNum);
      trim(tempNum);
      strcat(DataString, tempNum);
      strcat(DataString, ";");
      dtostrf(COSMA.GetStandardError(), 6, 2, tempNum);
      trim(tempNum);
      strcat(DataString, tempNum);
      strcat(DataString, "|");
    }
    else
    {
      strcat(DataString, "-1;-1|");
    }

    if (DustSMA.FullBuffer()) //Пыль
    {
      dtostrf(DustSMA.getFastAverage(), 6, 2, tempNum);
      trim(tempNum);
      strcat(DataString, tempNum);
      strcat(DataString, ";");
      dtostrf(DustSMA.GetStandardError(), 6, 2, tempNum);
      trim(tempNum);
      strcat(DataString, tempNum);
      strcat(DataString, "|");
    }
    else
    {
      strcat(DataString, "-1;-1|");
    }

    if (LPGSMA.FullBuffer()) //Газ
    {
      dtostrf(LPGSMA.getFastAverage(), 6, 2, tempNum);
      trim(tempNum);
      strcat(DataString, tempNum);
      strcat(DataString, ";");
      dtostrf(LPGSMA.GetStandardError(), 6, 2, tempNum);
      trim(tempNum);
      strcat(DataString, tempNum);
    }
    else
    {
      strcat(DataString, "-1;-1");
    }
 
  byte CRC = 0;
  for (size_t i = 0; i < strlen(DataString);i++)
  {
    CRC = CRC + (byte)(*(DataString+i));
  }

  char str[4];
  sprintf(str, "%03d", CRC);
  
  strcat(DataString, "*");
  strcat(DataString, str);
}


void trim(char *str)
{
    int i;
    int begin = 0;
    int end = strlen(str) - 1;

    while (isspace(str[begin]))
    {
      begin++;
    }
    while ((end >= begin) && isspace(str[end]))
    {
      end--;
    }
    for (i = begin; i <= end; i++)
    {
      str[i - begin] = str[i];
    }
    str[i - begin] = '\0';
}

#pragma endregion

void setup() 
{
  Serial.begin(9600);

  SoftSerialCO2.begin(9600);
  delay(10);
  SoftSerialCO.begin(9600);
  delay(10);
  SoftTransmitter.begin(9600);

  SoftSerialCO.onReceiveAttach(COCallback);

  pinMode(2, OUTPUT);
}


unsigned long check_overflow=0;
bool overflow = false;
void loop() 
{
  if (check_overflow > millis()) overflow = true;
  #pragma region LOOP
  if (((!overflow) && (millis() > settings.MicroclimatProgrev) && (millis() - prev_micro >= settings.MicroclimatInterval))
    || ((overflow) && (UNSIGNED_LONG_MAX - check_overflow + millis() >= settings.MicroclimatInterval)))
  {
    ProcessMicroclimat();
    prev_micro = millis();
  }
  if (((!overflow) && (millis() > settings.CO2Progrev) && (millis() - prev_co2 >= settings.CO2Interval)) 
    || ((overflow) && (UNSIGNED_LONG_MAX - check_overflow + millis() >= settings.CO2Interval)))
  {
    ProcessCO2();
    prev_co2 = millis();
  }
  if ((millis() > settings.COProgrev) && COReciveFlag && ((millis() - COReciveTime) >= 20))
  {
    ProcessCO();
  }
  if (((!overflow) && (millis() > settings.DustProgrev) && (millis() - prev_dust >= settings.DustInterval))
    || ((overflow) && (UNSIGNED_LONG_MAX - check_overflow + millis() >= settings.DustInterval)))
  {
    ProcessDust();
    prev_dust = millis();
  }
  if (((!overflow) && (millis() > settings.LPGProgrev) && (millis() - prev_lpg >= settings.LPGInterval))
    || ((overflow) && (UNSIGNED_LONG_MAX - check_overflow + millis() >= settings.LPGInterval)))
  {
    ProcessLPG();
    prev_lpg = millis();
  }


  if (((!overflow) && (millis() - send_prev >= settings.SendDataInterval))
    || ((overflow) && (UNSIGNED_LONG_MAX - check_overflow + millis() >= settings.SendDataInterval)))
  {
    MakeDataString();
    SoftTransmitter.println(DataString);
    
    send_prev = millis();
  }
  #pragma endregion
  check_overflow = millis();
  overflow = false;
}
