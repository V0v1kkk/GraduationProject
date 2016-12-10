#pragma region Display
#pragma region GlobalDisplay
int numberoffield = 999; //При редактировании поля хранит его идентификатор
#pragma endregion

#pragma region Declaration
Nextion nex(Serial);

NextionText nexStatusText(nex, 0, 17, "status");
NextionText nexTempText(nex, 0, 2, "temp");
NextionText nexHumText(nex, 0, 4, "hum");
NextionText nexCoText(nex, 0, 6, "co");
NextionText nexCo2Text(nex, 0, 8, "co2");
NextionText nexDustText(nex, 0, 10, "dust");
NextionText nexGasText(nex, 0, 12, "gas");

NextionButton nexWifisettingsButton(nex, 0, 13, "wifisettings");
NextionText nexSsidText(nex, 1, 2, "ssid");
NextionText nexWifipassText(nex, 1, 4, "wifipassword");

NextionButton nexMqttsettingsButton(nex, 0, 14, "mqttsettings");
NextionDualStateButton nexMqttOff(nex, 2, 1, "mqttoff");
NextionText nexMqttServerText(nex, 2, 8, "mqttserver");
NextionText nexMqttPortText(nex, 2, 9, "mqttport");
NextionText nexMqttLoginText(nex, 2, 10, "mqttlogin");
NextionText nexMqttPassText(nex, 2, 11, "mqttpassword");

NextionButton nexAlarmsettingsButton1(nex, 0, 15, "alarmsettings");
NextionButton nexAlarmsettingsButton2(nex, 10, 2, "b1");
NextionButton nexAlarmsettingsButton3(nex, 3, 3, "b1");
NextionButton nexAlarmsettingsButton4(nex, 4, 2, "b1");
NextionButton nexAlarmsettingsButton5(nex, 5, 2, "b1");
NextionButton nexAlarmsettingsButton6(nex, 6, 2, "b1");

NextionDualStateButton nexAlarmOn(nex, 10, 4, "alarmonbutton");
NextionDualStateButton nexCheckErrorOn(nex, 10, 6, "intcheckon");
NextionText nexTempGUText(nex, 3, 6, "tempgu");
NextionText nexTempGDText(nex, 3, 9, "tempgd");
NextionText nexTempYUText(nex, 3, 7, "tempyu");
NextionText nexTempYDText(nex, 3, 10, "tempyd");
NextionText nexTempRUText(nex, 3, 8, "tempru");
NextionText nexTempRDText(nex, 3, 11, "temprd");

NextionText nexHumGUText(nex, 4, 6, "humgu");
NextionText nexHumGDText(nex, 4, 9, "humgd");
NextionText nexHumYUText(nex, 4, 7, "humyu");
NextionText nexHumYDText(nex, 4, 10, "humyd");
NextionText nexHumRUText(nex, 4, 8, "humru");
NextionText nexHumRDText(nex, 4, 11, "humrd");

NextionText nexCO2GUText(nex, 5, 6, "co2gu");
NextionText nexCO2GDText(nex, 5, 9, "co2gd");
NextionText nexCO2YUText(nex, 5, 7, "co2yu");
NextionText nexCO2YDText(nex, 5, 10, "co2yd");
NextionText nexCO2RUText(nex, 5, 8, "co2ru");
NextionText nexCO2RDText(nex, 5, 11, "co2rd");

NextionText nexCOGUText(nex, 6, 6, "cogu");
NextionText nexCOGDText(nex, 6, 9, "cogd");
NextionText nexCOYUText(nex, 6, 7, "coyu");
NextionText nexCOYDText(nex, 6, 10, "coyd");
NextionText nexCORUText(nex, 6, 8, "coru");
NextionText nexCORDText(nex, 6, 11, "cord");

NextionText nexDustGUText(nex, 7, 6, "dustgu");
NextionText nexDustGDText(nex, 7, 9, "dustgd");
NextionText nexDustYUText(nex, 7, 7, "dustyu");
NextionText nexDustYDText(nex, 7, 10, "dustyd");
NextionText nexDustRUText(nex, 7, 8, "dustru");
NextionText nexDustRDText(nex, 7, 11, "dustrd");

NextionButton nexCancelButton(nex, 8, 16, "editorcancel");
NextionButton nexTCancelButton(nex, 9, 7, "teditorcancel");
NextionButton nexOKButton(nex, 8, 15, "editordone");
NextionButton nexTOKButton(nex, 9, 6, "teditordone");

NextionText nexEditValue(nex, 8, 1, "editorvalue");
NextionText nexTEditValue(nex, 9, 1, "txteditvalue");

NextionButton nexRebootButton(nex, 0, 18, "reboot");
//NextionButton nexNetworkStatusButton(nex, 0, 19, "netstatus");
//NextionButton nexMQTTStatusButton(nex, 0, 20, "mqttstatus");

NextionVariableString nexTextVariable(nex, 8, 17, "text");
#pragma endregion

void DisplaySetup()
{
	nex.init();

	nexWifisettingsButton.attachCallback(&UploadWifiSettings);
	nexMqttsettingsButton.attachCallback(&UploadMqttSettings);
	nexAlarmsettingsButton1.attachCallback(&UploadAlarmSettings1);
	nexAlarmsettingsButton2.attachCallback(&UploadAlarmSettings2);
	nexAlarmsettingsButton3.attachCallback(&UploadAlarmSettings3);
	nexAlarmsettingsButton4.attachCallback(&UploadAlarmSettings4);
	nexAlarmsettingsButton5.attachCallback(&UploadAlarmSettings5);
	nexAlarmsettingsButton6.attachCallback(&UploadAlarmSettings6);

	nexSsidText.attachCallback(&PushWifiSSID);
	nexWifipassText.attachCallback(&PushWifiPass);

	nexMqttOff.attachCallback(&PushMqttOff);
	nexMqttServerText.attachCallback(&PushMqttServer);
	nexMqttPortText.attachCallback(&PushMqttPort);
	nexMqttLoginText.attachCallback(&PushMqttLogin);
	nexMqttPassText.attachCallback(&PushMqttPass);

	nexAlarmOn.attachCallback(&PushAlarmOn);
	nexCheckErrorOn.attachCallback(&PushErrorCheckOn);

	nexTempGUText.attachCallback(&PusAlarmTempGU);
	nexTempGDText.attachCallback(&PusAlarmTempGD);
	nexTempYUText.attachCallback(&PusAlarmTempYU);
	nexTempYDText.attachCallback(&PusAlarmTempYD);
	nexTempRUText.attachCallback(&PusAlarmTempRU);
	nexTempRDText.attachCallback(&PusAlarmTempRD);

	nexHumGUText.attachCallback(&PusAlarmHumGU);
	nexHumGDText.attachCallback(&PusAlarmHumGD);
	nexHumYUText.attachCallback(&PusAlarmHumYU);
	nexHumYDText.attachCallback(&PusAlarmHumYD);
	nexHumRUText.attachCallback(&PusAlarmHumRU);
	nexHumRDText.attachCallback(&PusAlarmHumRD);

	nexCO2GUText.attachCallback(&PusAlarmCO2GU);
	nexCO2GDText.attachCallback(&PusAlarmCO2GD);
	nexCO2YUText.attachCallback(&PusAlarmCO2YU);
	nexCO2YDText.attachCallback(&PusAlarmCO2YD);
	nexCO2RUText.attachCallback(&PusAlarmCO2RU);
	nexCO2RDText.attachCallback(&PusAlarmCO2RD);

	nexCOGUText.attachCallback(&PusAlarmCOGU);
	nexCOGDText.attachCallback(&PusAlarmCOGD);
	nexCOYUText.attachCallback(&PusAlarmCOYU);
	nexCOYDText.attachCallback(&PusAlarmCOYD);
	nexCORUText.attachCallback(&PusAlarmCORU);
	nexCORDText.attachCallback(&PusAlarmCORD);

	nexDustGUText.attachCallback(&PusAlarmDustGU);
	nexDustGDText.attachCallback(&PusAlarmDustGD);
	nexDustYUText.attachCallback(&PusAlarmDustYU);
	nexDustYDText.attachCallback(&PusAlarmDustYD);
	nexDustRUText.attachCallback(&PusAlarmDustRU);
	nexDustRDText.attachCallback(&PusAlarmDustRD);

	nexCancelButton.attachCallback(&ClearMarkEvent);
	nexTCancelButton.attachCallback(&ClearMarkEvent);

	nexOKButton.attachCallback(&OKEvent);
	nexTOKButton.attachCallback(&OKEvent);

	nexRebootButton.attachCallback(&ResetButtonPush);
}

#pragma region Callbacks
#pragma region UploadSettings
void UploadWifiSettings(NextionEventType type, INextionTouchable *widget)
{
	UploadWifiSettings1();
}
void UploadWifiSettings1()
{
	char temp[20];
	strcpy(temp, Settings.SSIDCfg.c_str());
	nexSsidText.setText(temp);
	strcpy(temp, Settings.WifiPass.c_str());
	nexWifipassText.setText(temp);
}
void UploadMqttSettings(NextionEventType type, INextionTouchable *widget)
{
	UploadMqttSettings1();
}
void UploadMqttSettings1()
{
	if (Settings.DisableMQTT) nexMqttOff.setActive(true);
	else nexMqttOff.setActive(false);

	char temp[25];
	strcpy(temp, Settings.MQTTServer.c_str());
	nexMqttServerText.setText(temp);
	strcpy(temp, Settings.MQTTPort.c_str());
	nexMqttPortText.setText(temp);
	strcpy(temp, Settings.MQTTLogin.c_str());
	nexMqttLoginText.setText(temp);
	strcpy(temp, Settings.MQTTPassword.c_str());
	nexMqttPassText.setText(temp);
}
void UploadAlarmSettings1(NextionEventType type, INextionTouchable *widget)
{
	UploadAlarm1();
}
void UploadAlarmSettings2(NextionEventType type, INextionTouchable *widget)
{
	UploadAlarm2();
}
void UploadAlarmSettings3(NextionEventType type, INextionTouchable *widget)
{
	UploadAlarm3();
}
void UploadAlarmSettings4(NextionEventType type, INextionTouchable *widget)
{
	UploadAlarm4();
}
void UploadAlarmSettings5(NextionEventType type, INextionTouchable *widget)
{
	UploadAlarm5();
}
void UploadAlarmSettings6(NextionEventType type, INextionTouchable *widget)
{
	UploadAlarm6();
}
void UploadAlarm1()
{
	if (Settings.AlarmOn) nexAlarmOn.setActive(true);
	else nexAlarmOn.setActive(false);

	if (Settings.checkError) nexCheckErrorOn.setActive(true);
	else nexCheckErrorOn.setActive(false);
}
void UploadAlarm2()
{
	char temp[10];

	dtostrf(Settings.TemperatureGreenZoneUP, 7, 2, temp);
	Serial1.println(temp); //debug
	trim(temp);
	Serial1.println(temp); //debug
	nexTempGUText.setText(temp);

	dtostrf(Settings.TemperatureGreenZoneDown, 7, 2, temp);
	trim(temp);
	nexTempGDText.setText(temp);
	dtostrf(Settings.TemperatureYellowZoneUP, 7, 2, temp);
	trim(temp);
	nexTempYUText.setText(temp);

	dtostrf(Settings.TemperatureYellowZoneDown, 7, 2, temp);
	trim(temp);
	nexTempYDText.setText(temp);

	dtostrf(Settings.TemperatureRedZoneUP, 7, 2, temp);
	trim(temp);
	nexTempRUText.setText(temp);

	dtostrf(Settings.TemperatureRedZoneDown, 7, 2, temp);
	trim(temp);
	nexTempRDText.setText(temp);
}
void UploadAlarm3()
{
	char temp[10];

	dtostrf(Settings.HumidityGreenZoneUP, 7, 2, temp);
	trim(temp);
	nexHumGUText.setText(temp);

	dtostrf(Settings.HumidityGreenZoneDown, 7, 2, temp);
	trim(temp);
	nexHumGDText.setText(temp);

	dtostrf(Settings.HumidityYellowZoneUP, 7, 2, temp);
	trim(temp);
	nexHumYUText.setText(temp);

	dtostrf(Settings.HumidityYellowZoneDown, 7, 2, temp);
	trim(temp);
	nexHumYDText.setText(temp);

	dtostrf(Settings.HumidityRedZoneUP, 7, 2, temp);
	trim(temp);
	nexHumRUText.setText(temp);

	dtostrf(Settings.HumidityRedZoneDown, 7, 2, temp);
	trim(temp);
	nexHumRDText.setText(temp);
}
void UploadAlarm4()
{
	char temp[10];

	dtostrf(Settings.CO2GreenZoneUP, 7, 2, temp);
	trim(temp);
	nexCO2GUText.setText(temp);

	dtostrf(Settings.CO2GreenZoneDown, 7, 2, temp);
	trim(temp);
	nexCO2GDText.setText(temp);

	dtostrf(Settings.CO2YellowZoneUP, 7, 2, temp);
	trim(temp);
	nexCO2YUText.setText(temp);

	dtostrf(Settings.CO2YellowZoneDown, 7, 2, temp);
	trim(temp);
	nexCO2YDText.setText(temp);

	dtostrf(Settings.CO2RedZoneUP, 7, 2, temp);
	trim(temp);
	nexCO2RUText.setText(temp);

	dtostrf(Settings.CO2RedZoneDown, 7, 2, temp);
	trim(temp);
	nexCO2RDText.setText(temp);
}
void UploadAlarm5()
{
	char temp[10];

	dtostrf(Settings.COGreenZoneUP, 7, 2, temp);
	trim(temp);
	nexCOGUText.setText(temp);

	dtostrf(Settings.COGreenZoneDown, 7, 2, temp);
	trim(temp);
	nexCOGDText.setText(temp);

	dtostrf(Settings.COYellowZoneUP, 7, 2, temp);
	trim(temp);
	nexCOYUText.setText(temp);

	dtostrf(Settings.COYellowZoneDown, 7, 2, temp);
	trim(temp);
	nexCOYDText.setText(temp);

	dtostrf(Settings.CORedZoneUP, 7, 2, temp);
	trim(temp);
	nexCORUText.setText(temp);

	dtostrf(Settings.CORedZoneDown, 7, 2, temp);
	trim(temp);
	nexCORDText.setText(temp);
}
void UploadAlarm6()
{
	char temp[10];

	dtostrf(Settings.DustGreenZoneUP, 7, 2, temp);
	trim(temp);
	nexDustGUText.setText(temp);

	dtostrf(Settings.DustGreenZoneDown, 7, 2, temp);
	trim(temp);
	nexDustGDText.setText(temp);

	dtostrf(Settings.DustYellowZoneUP, 7, 2, temp);
	trim(temp);
	nexDustYUText.setText(temp);

	dtostrf(Settings.DustYellowZoneDown, 7, 2, temp);
	trim(temp);
	nexDustYDText.setText(temp);

	dtostrf(Settings.DustRedZoneUP, 7, 2, temp);
	trim(temp);
	nexDustRUText.setText(temp);

	dtostrf(Settings.DustRedZoneDown, 7, 2, temp);
	trim(temp);
	nexDustRDText.setText(temp);
}
#pragma endregion

#pragma region Wifi
void PushWifiSSID(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 1;
}
void PushWifiPass(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 2;
}
#pragma endregion
#pragma region MQTT
void PushMqttOff(NextionEventType type, INextionTouchable *widget)
{
	if (nexMqttOff.isActive())
	{
		Settings.DisableMQTT = true;
		if(MQTTClient.connected()) MQTTClient.disconnect();
	}
	else
	{
		Settings.DisableMQTT = false;
		if (!MQTTClient.connected()) ConnectToMQTT();
	}
	SaveSettings();
}
void PushMqttServer(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 10;
}
void PushMqttPort(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 11;
}
void PushMqttLogin(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 12;
}
void PushMqttPass(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 13;
}
#pragma endregion
#pragma region Alarm
void PushAlarmOn(NextionEventType type, INextionTouchable *widget)
{
	if (nexAlarmOn.isActive()) Settings.AlarmOn = true;
	else Settings.AlarmOn = false;
	SaveSettings();
}
void PushErrorCheckOn(NextionEventType type, INextionTouchable *widget)
{
	if (nexCheckErrorOn.isActive()) Settings.checkError = true;
	else Settings.checkError = false;
	SaveSettings();
}

void PusAlarmTempGU(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 20;
}
void PusAlarmTempGD(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 21;
}
void PusAlarmTempYU(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 22;
}
void PusAlarmTempYD(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 23;
}
void PusAlarmTempRU(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 24;
}
void PusAlarmTempRD(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 25;
}

void PusAlarmHumGU(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 30;
}
void PusAlarmHumGD(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 31;
}
void PusAlarmHumYU(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 32;
}
void PusAlarmHumYD(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 33;
}
void PusAlarmHumRU(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 34;
}
void PusAlarmHumRD(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 35;
}

void PusAlarmCO2GU(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 40;
}
void PusAlarmCO2GD(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 41;
}
void PusAlarmCO2YU(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 42;
}
void PusAlarmCO2YD(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 43;
}
void PusAlarmCO2RU(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 44;
}
void PusAlarmCO2RD(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 45;
}

void PusAlarmCOGU(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 50;
}
void PusAlarmCOGD(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 51;
}
void PusAlarmCOYU(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 52;
}
void PusAlarmCOYD(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 53;
}
void PusAlarmCORU(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 54;
}
void PusAlarmCORD(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 55;
}

void PusAlarmDustGU(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 60;
}
void PusAlarmDustGD(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 61;
}
void PusAlarmDustYU(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 62;
}
void PusAlarmDustYD(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 63;
}
void PusAlarmDustRU(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 64;
}
void PusAlarmDustRD(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 65;
}
#pragma endregion
#pragma region Other
void ClearMarkEvent(NextionEventType type, INextionTouchable *widget)
{
	numberoffield = 999;
}

void OKEvent(NextionEventType type, INextionTouchable *widget)
{
	uint8_t c = 0;
	bool str_start_flag = false;
	uint8_t cnt_0xff = 0;
	String temp = String("");

	Serial.print("get editor.text.txt");
	Serial.write(0xFF);
	Serial.write(0xFF);
	Serial.write(0xFF);

	delay(160);

	while (Serial.available())
	{
		c = Serial.read();
		//Serial1.println(c); //debug
		if (str_start_flag)
		{
			if (0xFF == c)
			{
				cnt_0xff++;
				if (cnt_0xff >= 3)
				{
					break;
				}
			}
			else
			{
				temp += (char)c;
			}
		}
		else if (NEX_RET_STRING_HEAD == c)
		{
			str_start_flag = true;
		}
	}
	
	EditSettingEvent(temp);
}

void EditSettingEvent(String data)
{
	if (numberoffield == 999) return;

	char temp[25];
	strcpy(temp, data.c_str());


	switch (numberoffield) 
	{
	case 1:
		Settings.SSIDCfg = data;
		UploadWifiSettings1();
		break;
	case 2:
		Settings.WifiPass = data;
		UploadWifiSettings1();
		break;
	case 10:
		Settings.MQTTServer = data;
		UploadMqttSettings1();
		break;
	case 11:
		Settings.MQTTPort = data;
		UploadMqttSettings1();
		break;
	case 12:
		Settings.MQTTLogin = data;
		UploadMqttSettings1();
		break;
	case 13:
		Settings.MQTTPassword = data;
		UploadMqttSettings1();
		break;
	case 20:
		Settings.TemperatureGreenZoneUP = data.toFloat();
		UploadAlarm2();
		break;
	case 21:
		Settings.TemperatureGreenZoneDown = data.toFloat();
		UploadAlarm2();
		break;
	case 22:
		Settings.TemperatureYellowZoneUP = data.toFloat();
		UploadAlarm2();
		break;
	case 23:
		Settings.TemperatureYellowZoneDown = data.toFloat();
		UploadAlarm2();
		break;
	case 24:
		Settings.TemperatureRedZoneUP = data.toFloat();
		UploadAlarm2();
		break;
	case 25:
		Settings.TemperatureRedZoneDown = data.toFloat();
		UploadAlarm2();
		break;
	case 30:
		Settings.HumidityGreenZoneUP = data.toFloat();
		UploadAlarm3();
		break;
	case 31:
		Settings.HumidityGreenZoneDown = data.toFloat();
		UploadAlarm3();
		break;
	case 32:
		Settings.HumidityYellowZoneUP = data.toFloat();
		UploadAlarm3();
		break;
	case 33:
		Settings.HumidityYellowZoneDown = data.toFloat();
		UploadAlarm3();
		break;
	case 34:
		Settings.HumidityRedZoneUP = data.toFloat();
		UploadAlarm3();
		break;
	case 35:
		Settings.HumidityRedZoneDown = data.toFloat();
		UploadAlarm3();
		break;
	case 40:
		Settings.CO2GreenZoneUP = data.toFloat();
		UploadAlarm4();
		break;
	case 41:
		Settings.CO2GreenZoneDown = data.toFloat();
		UploadAlarm4();
		break;
	case 42:
		Settings.CO2YellowZoneUP = data.toFloat();
		UploadAlarm4();
		break;
	case 43:
		Settings.CO2YellowZoneDown = data.toFloat();
		UploadAlarm4();
		break;
	case 44:
		Settings.CO2RedZoneUP = data.toFloat();
		UploadAlarm4();
		break;
	case 45:
		Settings.CO2RedZoneDown = data.toFloat();
		UploadAlarm4();
		break;
	case 50:
		Settings.COGreenZoneUP = data.toFloat();
		UploadAlarm5();
		break;
	case 51:
		Settings.COGreenZoneDown = data.toFloat();
		UploadAlarm5();
		break;
	case 52:
		Settings.COYellowZoneUP = data.toFloat();
		UploadAlarm5();
		break;
	case 53:
		Settings.COYellowZoneDown = data.toFloat();
		UploadAlarm5();
		break;
	case 54:
		Settings.CORedZoneUP = data.toFloat();
		UploadAlarm5();
		break;
	case 55:
		Settings.CORedZoneDown = data.toFloat();
		UploadAlarm5();
		break;
	case 60:
		Settings.DustGreenZoneUP = data.toFloat();
		UploadAlarm6();
		break;
	case 61:
		Settings.DustGreenZoneDown = data.toFloat();
		UploadAlarm6();
		break;
	case 62:
		Settings.DustYellowZoneUP = data.toFloat();
		UploadAlarm6();
		break;
	case 63:
		Settings.DustYellowZoneDown = data.toFloat();
		UploadAlarm6();
		break;
	case 64:
		Settings.DustRedZoneUP = data.toFloat();
		UploadAlarm6();
		break;
	case 65:
		Settings.DustRedZoneDown = data.toFloat();
		UploadAlarm6();
		break;
	}

	SaveSettings();
	numberoffield = 999;
}
void SendStatusToDisplay(String text)
{
	char temp[21];
	strcpy(temp, text.c_str());
	nexStatusText.setText(temp);
}
void SendToDisplay()
{
	char temp[10];
	
	strcpy(temp, Data.temperature_S.c_str());
	nexTempText.setText(temp);
	if (Data.temperature_state == 1) nexTempText.setColour("pco", 17640, true);
	else if (Data.temperature_state == 2) nexTempText.setColour("pco", 64512, true);
	else if (Data.temperature_state == 3) nexTempText.setColour("pco", 63488, true);
	else nexTempText.setColour("pco", 0, true);

	strcpy(temp, Data.humidity_S.c_str());
	nexHumText.setText(temp);
	if (Data.humidity_state == 1) nexHumText.setColour("pco", 17640, true);
	else if (Data.humidity_state == 2) nexHumText.setColour("pco", 64512, true);
	else if (Data.humidity_state == 3) nexHumText.setColour("pco", 63488, true);
	else nexHumText.setColour("pco", 0, true);

	strcpy(temp, Data.CO2_S.c_str());
	nexCo2Text.setText(temp);
	if (Data.CO2_state == 1) nexCo2Text.setColour("pco", 17640, true);
	else if (Data.CO2_state == 2) nexCo2Text.setColour("pco", 64512, true);
	else if (Data.CO2_state == 3) nexCo2Text.setColour("pco", 63488, true);
	else nexCo2Text.setColour("pco", 0, true);

	strcpy(temp, Data.CO_S.c_str());
	nexCoText.setText(temp);
	if (Data.CO_state == 1) nexCoText.setColour("pco", 17640, true);
	else if (Data.CO_state == 2) nexCoText.setColour("pco", 64512, true);
	else if (Data.CO_state == 3) nexCoText.setColour("pco", 63488, true);
	else nexCoText.setColour("pco", 0, true);

	strcpy(temp, Data.dust_S.c_str());
	nexDustText.setText(temp);
	if (Data.dust_state == 1) nexDustText.setColour("pco", 17640, true);
	else if (Data.dust_state == 2) nexDustText.setColour("pco", 64512, true);
	else if (Data.dust_state == 3) nexDustText.setColour("pco", 63488, true);
	else nexDustText.setColour("pco", 0, true);

	
	if (Data.LPG_state == 1)
	{
		nexGasText.setText("No");
		nexGasText.setColour("pco", 17640, true);
	}
	else if (Data.LPG_state == 2)
	{
		nexGasText.setText("Maybe");
		nexGasText.setColour("pco", 64512, true);
	}
	else if (Data.LPG_state == 3)
	{
		nexGasText.setText("Yes");
		nexGasText.setColour("pco", 63488, true);
	}
	else 
	{
		nexGasText.setText("-");
		nexGasText.setColour("pco", 0, true);
	}
	

	//Выдаём собщение
	int alarmlevel = GetAlarmLevel();
	if (alarmlevel == 1)
	{
		SendStatusToDisplay("OK");
		nexStatusText.setColour("pco", 17640, true);
	}
	else if (alarmlevel == 2) 
	{ 
		SendStatusToDisplay("Warning"); 
		nexStatusText.setColour("pco", 64512, true);
	}
	else if (alarmlevel == 3)
	{
		SendStatusToDisplay("Alarm");
		nexStatusText.setColour("pco", 63488, true);
	}
}
void ResetButtonPush(NextionEventType type, INextionTouchable *widget)
{
	ESP.reset();
}
int GetAlarmLevel()
{
	int t = 1;
	if (Data.temperature_state > t) t = Data.temperature_state;
	if (Data.humidity_state > t) t = Data.humidity_state;
	if (Data.CO2_state > t) t = Data.CO2_state;
	if (Data.CO_state > t) t = Data.CO_state;
	if (Data.dust_state > t) t = Data.dust_state;
	if (Data.LPG_state > t) t = Data.LPG_state;
	return t;
}
void NetworkStatusDislpaySet(bool status)
{
	if (status)
	{
		nex.drawRect(293, 0, 27, 26, false, 17640);
	}
	else
	{
		nex.drawRect(293, 0, 27, 26, false, 63488);
	}
}
void MQTTStatusDislpaySet(bool status)
{
	if (status)
	{
		nex.drawRect(293, 26, 27, 26, false, 17640);
	}
	else
	{
		nex.drawRect(293, 26, 27, 26, false, 63488);
	}
}
#pragma endregion
#pragma endregion
#pragma endregion