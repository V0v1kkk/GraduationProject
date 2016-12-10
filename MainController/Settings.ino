struct Setting {
	String SSIDCfg; ////Имя wi-fi сети. Должно быть прочитана из конфига
	String WifiPass; //Пароль от wi-fi сети. Должен быть прочитан из конфига
	bool DisableMQTT;
	String MQTTServer;
	String MQTTPort;
	String MQTTLogin;
	String MQTTPassword;

	float TemperatureGreenZoneUP;
	float TemperatureGreenZoneDown;
	float TemperatureYellowZoneUP;
	float TemperatureYellowZoneDown;
	float TemperatureRedZoneUP;
	float TemperatureRedZoneDown;

	float HumidityGreenZoneUP;
	float HumidityGreenZoneDown;
	float HumidityYellowZoneUP;
	float HumidityYellowZoneDown;
	float HumidityRedZoneUP;
	float HumidityRedZoneDown;

	float CO2GreenZoneUP;
	float CO2GreenZoneDown;
	float CO2YellowZoneUP;
	float CO2YellowZoneDown;
	float CO2RedZoneUP;
	float CO2RedZoneDown;

	float COGreenZoneUP;
	float COGreenZoneDown;
	float COYellowZoneUP;
	float COYellowZoneDown;
	float CORedZoneUP;
	float CORedZoneDown;

	float DustGreenZoneUP;
	float DustGreenZoneDown;
	float DustYellowZoneUP;
	float DustYellowZoneDown;
	float DustRedZoneUP;
	float DustRedZoneDown;

	float LPGGreenZoneUP;
	float LPGGreenZoneDown;
	float LPGYellowZoneUP;
	float LPGYellowZoneDown;
	float LPGRedZoneUP;
	float LPGRedZoneDown;

	bool AlarmOn;
	bool checkError;
} Settings = 
{
	"vovikkk",
	"Aion1aion",
	false,
	"vovikkk.ru",
	"1883",
	"AirMonitor",
	"diplom",

	25.0,
	22.0,
	28.0,
	20.0,
	100.0,
	-50.0,

	60.0,
	30.0,
	65.0,
	20.0,
	100,
	0,

	1000,
	0,
	1400,
	1000,
	10000,
	1400,

	2.58,
	0,
	4.3,
	0,
	10000,
	0,

	0.15,
	0,
	0.5,
	0,
	999,
	0,

	5,
	0,
	10,
	5,
	1024,
	10,

	true,
	true
};

bool SaveSettings()
{
	Serial1.println("Save settings"); //debug

	StaticJsonBuffer<1000> jsonBuffer;
	JsonObject& json = jsonBuffer.createObject();
	json["SSIDCfg"] = Settings.SSIDCfg;
	json["WifiPass"] = Settings.WifiPass;

	json["DisableMQTT"] = Settings.DisableMQTT;
	json["MQTTServer"] = Settings.MQTTServer;
	json["MQTTPort"] = Settings.MQTTPort;
	json["MQTTLogin"] = Settings.MQTTLogin;
	json["MQTTPassword"] = Settings.MQTTPassword;

	json["AlarmOn"] = Settings.AlarmOn;
	json["checkError"] = Settings.checkError;

	json["TGU"] = Settings.TemperatureGreenZoneUP;
	json["TGD"] = Settings.TemperatureGreenZoneDown;
	json["TYU"] = Settings.TemperatureYellowZoneUP;
	json["TYD"] = Settings.TemperatureYellowZoneDown;
	json["TRU"] = Settings.TemperatureRedZoneUP;
	json["TRD"] = Settings.TemperatureRedZoneDown;

	json["HGU"] = Settings.HumidityGreenZoneUP;
	json["HGD"] = Settings.HumidityGreenZoneDown;
	json["HYU"] = Settings.HumidityYellowZoneUP;
	json["HYD"] = Settings.HumidityYellowZoneDown;
	json["HRU"] = Settings.HumidityRedZoneUP;
	json["HRD"] = Settings.HumidityRedZoneDown;

	json["CO2GU"] = Settings.CO2GreenZoneUP;
	json["CO2GD"] = Settings.CO2GreenZoneDown;
	json["CO2YU"] = Settings.CO2YellowZoneUP;
	json["CO2YD"] = Settings.CO2YellowZoneDown;
	json["CO2RU"] = Settings.CO2RedZoneUP;
	json["CO2RD"] = Settings.CO2RedZoneDown;

	json["COGU"] = Settings.COGreenZoneUP;
	json["COGD"] = Settings.COGreenZoneDown;
	json["COYU"] = Settings.COYellowZoneUP;
	json["COYD"] = Settings.COYellowZoneDown;
	json["CORU"] = Settings.CORedZoneUP;
	json["CORD"] = Settings.CORedZoneDown;

	json["DGU"] = Settings.DustGreenZoneUP;
	json["DGD"] = Settings.DustGreenZoneDown;
	json["DYU"] = Settings.DustYellowZoneUP;
	json["DYD"] = Settings.DustYellowZoneDown;
	json["DRU"] = Settings.DustRedZoneUP;
	json["DRD"] = Settings.DustRedZoneDown;

	SD.remove("config.jsn"); //debug
	File configFile = SD.open("config.jsn", FILE_WRITE);
	if (!configFile)
	{
		Serial1.println("Failed to open config file for writing"); //debug
		return false;
	}

	json.printTo(configFile);
	configFile.close();

	return true;
}

bool LoadSettings()
{
	File configFile = SD.open("config.jsn", FILE_READ);
	if (!configFile) 
	{
		Serial1.println("Failed to open config file"); //debug
		return false;
	}

	size_t size = configFile.size();
	char *jsonstring = new char[size];
	configFile.readBytes(jsonstring, size);


	StaticJsonBuffer<1000> jsonBuffer;
	JsonObject& json = jsonBuffer.parseObject(jsonstring);

	if (!json.success())
	{
		Serial1.println("Failed to parse config file"); //debug
		delete[]  jsonstring;
		return false;
	}

	const char* SSIDCfg = json["SSIDCfg"];
	Settings.SSIDCfg = SSIDCfg;
	const char* WifiPass = json["WifiPass"];
	Settings.WifiPass = WifiPass;


	Settings.DisableMQTT = json["DisableMQTT"].as<bool>();
	const char* MQTTServer = json["MQTTServer"];
	Settings.MQTTServer = MQTTServer;
	const char* MQTTPort = json["MQTTPort"];
	Settings.MQTTPort = MQTTPort;
	const char* MQTTLogin = json["MQTTLogin"];
	Settings.MQTTLogin = MQTTLogin;
	const char* MQTTPassword = json["MQTTPassword"];
	Settings.MQTTPassword = MQTTPassword;

	Settings.AlarmOn = json["AlarmOn"].as<bool>();
	Settings.checkError = json["checkError"].as<bool>();

	Settings.TemperatureGreenZoneUP = json["TGU"].as<float>();
	Settings.TemperatureGreenZoneDown = json["TGD"].as<float>();
	Settings.TemperatureYellowZoneUP = json["TYU"].as<float>();
	Settings.TemperatureYellowZoneDown = json["TYD"].as<float>();
	Settings.TemperatureRedZoneUP = json["TRU"].as<float>();
	Settings.TemperatureRedZoneDown = json["TRD"].as<float>();

	Settings.HumidityGreenZoneUP = json["HGU"].as<float>();
	Settings.HumidityGreenZoneDown = json["HGD"].as<float>();
	Settings.HumidityYellowZoneUP = json["HYU"].as<float>();
	Settings.HumidityYellowZoneDown = json["HYD"].as<float>();
	Settings.HumidityRedZoneUP = json["HRU"].as<float>();
	Settings.HumidityRedZoneDown = json["HRD"].as<float>();

	Settings.CO2GreenZoneUP = json["CO2GU"].as<float>();
	Settings.CO2GreenZoneDown = json["CO2GD"].as<float>();
	Settings.CO2YellowZoneUP = json["CO2YU"].as<float>();
	Settings.CO2YellowZoneDown = json["CO2YD"].as<float>();
	Settings.CO2RedZoneUP = json["CO2RU"].as<float>();
	Settings.CO2RedZoneDown = json["CO2RD"].as<float>();

	Settings.COGreenZoneUP = json["COGU"].as<float>();
	Settings.COGreenZoneDown = json["COGD"].as<float>();
	Settings.COYellowZoneUP = json["COYU"].as<float>();
	Settings.COYellowZoneDown = json["COYD"].as<float>();
	Settings.CORedZoneUP = json["CORU"].as<float>();
	Settings.CORedZoneDown = json["CORD"].as<float>();

	Settings.DustGreenZoneUP = json["DGU"].as<float>();
	Settings.DustGreenZoneDown = json["DGD"].as<float>();
	Settings.DustYellowZoneUP = json["DYU"].as<float>();
	Settings.DustYellowZoneDown = json["DYD"].as<float>();
	Settings.DustRedZoneUP = json["DRU"].as<float>();
	Settings.DustRedZoneDown = json["DRD"].as<float>();
	

	delete[]  jsonstring;
	return true;
}