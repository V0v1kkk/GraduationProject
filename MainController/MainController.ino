#include <NextionWaveform.h>
#include <NextionVariableString.h>
#include <NextionVariableNumeric.h>
#include <NextionTypes.h>
#include <NextionTimer.h>
#include <NextionText.h>
#include <NextionSlider.h>
#include <NextionProgressBar.h>
#include <NextionPicture.h>
#include <NextionPage.h>
#include <NextionNumber.h>
#include <NextionHotspot.h>
#include <NextionGauge.h>
#include <NextionDualStateButton.h>
#include <NextionCrop.h>
#include <NextionCallbackFunctionHandler.h>
#include <NextionButton.h>
#include <Nextion.h>
#include <INextionWidget.h>
#include <INextionTouchable.h>
#include <INextionStringValued.h>
#include <INextionNumericalValued.h>
#include <INextionFontStyleable.h>
#include <INextionColourable.h>
#include <INextionCallback.h>
#include <ArduinoJson.h>
#include <DFPlayer_Mini_Mp3.h>
#include <LinkedList.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <SD.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SPI.h>
#include "WiFiUdp.h"
#include <PubSubClient.h>
#include "Diplom.h"

#define UNSIGNED_LONG_MAX 4294967295UL

#pragma region Global
bool hasSD = false;

const int sclPin = D3; //Пины I2C
const int sdaPin = D4;

#define PERIOD_ZAPROSA_DANNYH 1000UL;
unsigned long prev_zapros = 0;

ESP8266WebServer HTTP(80);
File fsUploadFile;

WiFiClient espClient;
PubSubClient MQTTClient(espClient);

struct Data {
	float temperature;
	float temperature_Error;
	float humidity;
	float humidity_Error;
	float CO2;
	float CO2_Error;
	float CO;
	float CO_Error;
	float dust;
	float dust_Error;
	float LPG;
	float LPG_Error;

	String temperature_S;
	String temperature_Error_S;
	String humidity_S;
	String humidity_Error_S;
	String CO2_S;
	String CO2_Error_S;
	String CO_S;
	String CO_Error_S;
	String dust_S;
	String dust_Error_S;
	String LPG_S;
	String LPG_Error_S;

	int temperature_state;
	int humidity_state;
	int CO2_state;
	int CO_state;
	int dust_state;
	int LPG_state;
} Data = {
	-999.0,
	-999.0,
	-999.0,
	-999.0,
	-999.0,
	-999.0,
	-999.0,
	-999.0,
	-999.0,
	-999.0,
	-999.0,
	-999.0,

	"-",
	"-",
	"-",
	"-",
	"-",
	"-",
	"-",
	"-",
	"-",
	"-",
	"-",
	"-",

	0,
	0,
	0,
	0,
	0,
	0
};


#pragma endregion

SoftwareSerial SoftTransmitter(sclPin, sdaPin, false, 90); // RX, TX
void setup() 
{
	Serial.begin(9600); 
	Serial1.begin(9600); //Инициализируем serial соединение для отладки
	SoftTransmitter.begin(9600);
	delay(10);

	if (SD.begin(SS))
	{
		Serial1.println("SD Card initialized."); //debug
		hasSD = true;
	}
	LoadSettings();

	pinMode(D2, INPUT); //Чием статус воспроизведения
	mp3_set_serial(SoftTransmitter);
	mp3_set_volume(22);

	DisplaySetup();


#pragma region ConnectToWifi
	WiFi.mode(WIFI_STA);
	delay(50);
	if (findWiFiAndConnect())
	{
		Serial1.println(""); //debug
		Serial1.println("WiFi connected"); //debug
		Serial1.print("IP address: "); //debug
		Serial1.println(WiFi.localIP()); //debug

		NetworkStatusDislpaySet(true);
	}
	else
	{
		NetworkStatusDislpaySet(false);
		WiFi.mode(WIFI_AP);
		WiFi.softAP("AirQuality");
	}
#pragma endregion
#pragma region ConnectToMQTT
	if ((!Settings.DisableMQTT) && (WiFi.mode(WIFI_STA)) && (WiFi.status() == WL_CONNECTED))
	{
		if (ConnectToMQTT())
		{
			MQTTStatusDislpaySet(true);
		}
		else
		{
			MQTTStatusDislpaySet(false);
		}
	}
	else
	{
		MQTTStatusDislpaySet(false);
	}
#pragma endregion

	HTTP_init();
}


 #pragma region MainLOOP
unsigned long check_perepol = 0;
bool perepol = false;
unsigned long cycles_counter = 0;
unsigned long mqtt_prev_zapros = 0;
void loop()
{
#pragma region EVERY LOOP START
	
	if ((WiFi.getMode() == 3) || ((WiFi.getMode() != 3) && (WiFi.status() != WL_CONNECTED)))
	{
		NetworkStatusDislpaySet(false);
	}
	else
	{
		NetworkStatusDislpaySet(true);
	}
	

	if ((!Settings.DisableMQTT) && (!MQTTClient.connected()) && (WiFi.status() == WL_CONNECTED))
	{
		if (ConnectToMQTT())
		{
			MQTTStatusDislpaySet(true);
		}
		else
		{
			MQTTStatusDislpaySet(false);
		}
	}
	else if(Settings.DisableMQTT)
	{
		MQTTStatusDislpaySet(false);
	}
	else if (MQTTClient.connected())
	{
		MQTTStatusDislpaySet(true);
	}
	else if (!MQTTClient.connected())
	{
		MQTTStatusDislpaySet(false);
	}

	if (check_perepol > millis()) //Проверка на переполнение счётчика милисекунд со старта контроллера
	{
		perepol = true;
		cycles_counter++;
	}

	if (Settings.AlarmOn && (queue.size() > 0) && (digitalRead(D2) == HIGH))
	{
		mp3_play(queue.shift());
	}

	nex.poll(); //Обрабатываем сообщения от LCD-дисплея
#pragma endregion

#pragma region LOOP PERIOD
	if (((!perepol) && (millis() - prev_zapros >= 500UL))
		|| ((perepol) && (UNSIGNED_LONG_MAX - check_perepol + millis() >= 500UL))) //Запрос данных раз в секунду
	{
		String temp = "";
		char ch;

		if (SoftTransmitter.available())
		{
			//unsigned long startrecive = millis();//debug

			while (SoftTransmitter.available())
			{
				ch = SoftTransmitter.read();
				temp += ch;
				if (ch == '/n')
				{
					while (SoftTransmitter.available())
					{
						SoftTransmitter.read();
					}
					break;
				}
				if (!(SoftTransmitter.available()) && (ch != '/n'))
				{
					delay(10);
				}
			}

			//Serial1.print(temp);//debug

			//Serial1.println("Start parce data");//debug
			//startrecive = millis();//debug

			ParseData(temp);

			//Serial1.print("End parce data: ");//debug
			//Serial1.println(millis() - startrecive);//debug
		}

		//Serial1.print("Free heap: ");//debug
		//Serial1.println(ESP.getFreeHeap());//debug

		//Serial1.print("Connected: ");//debug
		//Serial1.println(WiFi.isConnected());//debug

		//Serial1.print("Status: ");//debug
		//Serial1.println(WiFi.status());//debug

		prev_zapros = millis();
	}


	if (((!perepol) && (millis() - mqtt_prev_zapros >= 5000UL)) 
		|| ((perepol) && (UNSIGNED_LONG_MAX - check_perepol + millis() >= 5000UL))) //Debug
	{
		if (MQTTClient.connected()) SendToMQTT();


		if ((WiFi.getMode() == 1) && (WiFi.status() != WL_CONNECTED))
		{
			Serial1.println("Connection lost! Reconnect..."); //debug
			if (!findWiFiAndConnect())
			{
				NetworkStatusDislpaySet(false);
				WiFi.mode(WIFI_AP);
				WiFi.softAP("AirQuality");
			}
			else
			{
				NetworkStatusDislpaySet(true);
			}
		}

		mqtt_prev_zapros = millis();
	}
#pragma endregion

#pragma region EVERY LOOP END
	HTTP.handleClient();
	check_perepol = millis();
	perepol = false;
	MQTTClient.loop();
#pragma endregion
}

#pragma endregion

