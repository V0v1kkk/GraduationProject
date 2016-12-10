#pragma region WebServer
File uploadFile;

#pragma region Standart
void FSSD_init(void)
{
	HTTP.on("/list", HTTP_GET, printDirectory);
	HTTP.on("/edit", HTTP_DELETE, handleDelete);
	HTTP.on("/edit", HTTP_PUT, handleCreate);
	HTTP.on("/edit", HTTP_POST, []() { returnOK(); }, handleFileUpload);
	HTTP.onNotFound(handleNotFound);
}

void returnOK()
{
	HTTP.send(200, "text/plain", "");
}

void returnFail(String msg)
{
	HTTP.send(500, "text/plain", msg + "\r\n");
}

bool loadFromSdCard(String path)
{
	String dataType = "text/plain";
	if (path.endsWith("/")) path += "index.htm";

	if (path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
	else if (path.endsWith(".htm")) dataType = "text/html";
	else if (path.endsWith(".css")) dataType = "text/css";
	else if (path.endsWith(".js")) dataType = "application/javascript";
	else if (path.endsWith(".png")) dataType = "image/png";
	else if (path.endsWith(".gif")) dataType = "image/gif";
	else if (path.endsWith(".jpg")) dataType = "image/jpeg";
	else if (path.endsWith(".ico")) dataType = "image/x-icon";
	else if (path.endsWith(".xml")) dataType = "text/xml";
	else if (path.endsWith(".pdf")) dataType = "application/pdf";
	else if (path.endsWith(".zip")) dataType = "application/zip";

	File dataFile = SD.open(path.c_str());
	if (dataFile.isDirectory())
	{
		path += "/index.htm";
		dataType = "text/html";
		dataFile = SD.open(path.c_str());
	}

	if (!dataFile) return false;

	if (HTTP.hasArg("download")) dataType = "application/octet-stream";

	if (HTTP.streamFile(dataFile, dataType) != dataFile.size())
	{

	}

	dataFile.close();
	return true;
}

void handleFileUpload()
{
	if (HTTP.uri() != "/edit") return;
	HTTPUpload& upload = HTTP.upload();
	if (upload.status == UPLOAD_FILE_START)
	{
		if (SD.exists((char *)upload.filename.c_str())) SD.remove((char *)upload.filename.c_str());
		uploadFile = SD.open(upload.filename.c_str(), FILE_WRITE);

	}
	else if (upload.status == UPLOAD_FILE_END)
	{
		if (uploadFile) uploadFile.close();

	}
}

void deleteRecursive(String path)
{
	File file = SD.open((char *)path.c_str());
	if (!file.isDirectory())
	{
		file.close();
		SD.remove((char *)path.c_str());
		return;
	}

	file.rewindDirectory();
	while (true)
	{
		File entry = file.openNextFile();
		if (!entry) break;
		String entryPath = path + "/" + entry.name();
		if (entry.isDirectory())
		{
			entry.close();
			deleteRecursive(entryPath);
		}
		else
		{
			entry.close();
			SD.remove((char *)entryPath.c_str());
		}
		yield();
	}

	SD.rmdir((char *)path.c_str());
	file.close();
}

void handleDelete()
{
	if (HTTP.args() == 0) return returnFail("BAD ARGS");
	String path = HTTP.arg(0);
	if (path == "/" || !SD.exists((char *)path.c_str()))
	{
		returnFail("BAD PATH");
		return;
	}
	deleteRecursive(path);
	returnOK();
}

void handleCreate()
{
	if (HTTP.args() == 0) return returnFail("BAD ARGS");
	String path = HTTP.arg(0);
	if (path == "/" || SD.exists((char *)path.c_str()))
	{
		returnFail("BAD PATH");
		return;
	}

	if (path.indexOf('.') > 0)
	{
		File file = SD.open((char *)path.c_str(), FILE_WRITE);
		if (file) {
			file.write((const char *)0);
			file.close();
		}
	}
	else
	{
		SD.mkdir((char *)path.c_str());
	}
	returnOK();
}

void printDirectory()
{
	if (!HTTP.hasArg("dir")) return returnFail("BAD ARGS");
	String path = HTTP.arg("dir");
	if (path != "/" && !SD.exists((char *)path.c_str())) return returnFail("BAD PATH");
	File dir = SD.open((char *)path.c_str());
	path = String();
	if (!dir.isDirectory())
	{
		dir.close();
		return returnFail("NOT DIR");
	}
	dir.rewindDirectory();
	HTTP.setContentLength(CONTENT_LENGTH_UNKNOWN);
	HTTP.send(200, "text/json", "");
	WiFiClient client = HTTP.client();

	HTTP.sendContent("[");
	for (int cnt = 0; true; ++cnt)
	{
		File entry = dir.openNextFile();
		if (!entry)
			break;

		String output;
		if (cnt > 0)
			output = ",";

		output += "{\"type\":\"";
		output += (entry.isDirectory()) ? "dir" : "file";
		output += "\",\"name\":\"";
		output += entry.name();
		output += "\"";
		output += "}";
		HTTP.sendContent(output);
		entry.close();
	}
	HTTP.sendContent("]");
	dir.close();
}

void handleNotFound()
{
	if (hasSD && loadFromSdCard(HTTP.uri())) return;
	String message = "SDCARD Not Detected\n\n";
	message += "URI: ";
	message += HTTP.uri();
	message += "\nMethod: ";
	message += (HTTP.method() == HTTP_GET) ? "GET" : "POST";
	message += "\nArguments: ";
	message += HTTP.args();
	message += "\n";
	for (uint8_t i = 0; i<HTTP.args(); i++) {
		message += " NAME:" + HTTP.argName(i) + "\n VALUE:" + HTTP.arg(i) + "\n";
	}
	HTTP.send(404, "text/plain", message);

}
#pragma endregion
#pragma region Additional

void HTTP_init(void)
{
	FSSD_init(); // Включаем работу с файловой системой

	HTTP.on("/xml", handleDataXML); // Функции для формирование xml страницы для передачи данных в web интерфейс
	HTTP.on("/alarmsettings", handleAlarmSettingsXML);
	HTTP.on("/networksettings", handleNetworkSettingsXML);
	HTTP.on("/mqttsettings", handleMQTTSettingsXML);
	HTTP.on("/stats", handleStatsXML);

	HTTP.on("/setalarmsettings", HTTP_PUT,  handleSetAlarmSettings); //Функции для приема информации с страниц
	HTTP.on("/setnetworksettings", HTTP_PUT, handleSetNetworkSettings);
	HTTP.on("/setmqttsettings", HTTP_PUT, handleSetMQTTSettings);
	
	HTTP.on("/reset", HTTP_PUT, handleReset);

	update(); //Функция Update для обновления прошивки

	HTTP.begin(); // Запускаем HTTP сервер
}

void handleReset()
{
	returnOK();	
	ESP.reset();
}

void handleSetMQTTSettings()
{
	if (HTTP.args() == 0) return; //Если параметров нет

	if (HTTP.arg("disable") != "")
	{
		if (HTTP.arg("disable") == "1")
		{
			if (!Settings.DisableMQTT)
			{
				Settings.DisableMQTT = true;
				if (MQTTClient.connected()) MQTTClient.disconnect();
			}
		}
		else
		{
			Settings.DisableMQTT = false;
			if (!MQTTClient.connected()) ConnectToMQTT();
		}
	}

	if (HTTP.arg("server") != "")
	{
		Settings.MQTTServer = HTTP.arg("server");
	}
	if (HTTP.arg("port") != "")
	{
		Settings.MQTTPort = HTTP.arg("port");
	}
	if (HTTP.arg("login") != "")
	{
		Settings.MQTTLogin = HTTP.arg("login");
	}
	if (HTTP.arg("password") != "")
	{
		Settings.MQTTPassword = HTTP.arg("password");
	}
	returnOK();
	SaveSettings();	
}
void handleSetNetworkSettings()
{
	if (HTTP.args() == 0) return; //Если параметров нет

	if (HTTP.arg("SSID") != "")
	{
		Settings.SSIDCfg = HTTP.arg("SSID");
	}
	if (HTTP.arg("WifiPass") != "")
	{
		Settings.WifiPass = HTTP.arg("WifiPass");
	}
	returnOK();
	SaveSettings();
}
void handleSetAlarmSettings()
{
	if (HTTP.args() == 0) return; //Если параметров нет

	if (HTTP.arg("AlarmState") != "")
	{
		if (HTTP.arg("AlarmState") == "1") Settings.AlarmOn = true;
		else Settings.AlarmOn = false;
	}

	if (HTTP.arg("CheckErrorState") != "")
	{
		if (HTTP.arg("CheckErrorState") == "1") Settings.checkError = true;
		else Settings.checkError = false;
	}

	if (HTTP.arg("TGZUP") != "")
	{
		Settings.TemperatureGreenZoneUP = HTTP.arg("TGZUP").toFloat();
	}
	if (HTTP.arg("TGZDown") != "")
	{
		Settings.TemperatureGreenZoneDown = HTTP.arg("TGZDown").toFloat();
	}
	if (HTTP.arg("TYZUP") != "")
	{
		Settings.TemperatureYellowZoneUP = HTTP.arg("TYZUP").toFloat();
	}
	if (HTTP.arg("TYZDown") != "")
	{
		Settings.TemperatureYellowZoneDown = HTTP.arg("TYZDown").toFloat();
	}
	if (HTTP.arg("TRZUP") != "")
	{
		Settings.TemperatureRedZoneUP = HTTP.arg("TRZUP").toFloat();
	}
	if (HTTP.arg("TRZDown") != "")
	{
		Settings.TemperatureRedZoneDown = HTTP.arg("TRZDown").toFloat();
	}

	if (HTTP.arg("HGZUP") != "")
	{
		Settings.HumidityGreenZoneUP = HTTP.arg("HGZUP").toFloat();
	}
	if (HTTP.arg("HGZDown") != "")
	{
		Settings.HumidityGreenZoneDown = HTTP.arg("HGZDown").toFloat();
	}
	if (HTTP.arg("HYZUP") != "")
	{
		Settings.HumidityYellowZoneUP = HTTP.arg("HYZUP").toFloat();
	}
	if (HTTP.arg("HYZDown") != "")
	{
		Settings.HumidityYellowZoneDown = HTTP.arg("HYZDown").toFloat();
	}
	if (HTTP.arg("HRZUP") != "")
	{
		Settings.HumidityRedZoneUP = HTTP.arg("HRZUP").toFloat();
	}
	if (HTTP.arg("HRZDown") != "")
	{
		Settings.HumidityRedZoneDown = HTTP.arg("HRZDown").toFloat();
	}

	if (HTTP.arg("CO2GZUP") != "")
	{
		Settings.CO2GreenZoneUP = HTTP.arg("CO2GZUP").toFloat();
	}
	if (HTTP.arg("CO2GZDown") != "")
	{
		Settings.CO2GreenZoneDown = HTTP.arg("CO2GZDown").toFloat();
	}
	if (HTTP.arg("CO2YZUP") != "")
	{
		Settings.CO2YellowZoneUP = HTTP.arg("CO2YZUP").toFloat();
	}
	if (HTTP.arg("CO2YZDown") != "")
	{
		Settings.CO2YellowZoneDown = HTTP.arg("CO2YZDown").toFloat();
	}
	if (HTTP.arg("CO2RZUP") != "")
	{
		Settings.CO2RedZoneUP = HTTP.arg("CO2RZUP").toFloat();
	}
	if (HTTP.arg("CO2RZDown") != "")
	{
		Settings.CO2RedZoneDown = HTTP.arg("CO2RZDown").toFloat();
	}

	if (HTTP.arg("COGZUP") != "")
	{
		Settings.COGreenZoneUP = HTTP.arg("COGZUP").toFloat();
	}
	if (HTTP.arg("COGZDown") != "")
	{
		Settings.COGreenZoneDown = HTTP.arg("COGZDown").toFloat();
	}
	if (HTTP.arg("COYZUP") != "")
	{
		Settings.COYellowZoneUP = HTTP.arg("COYZUP").toFloat();
	}
	if (HTTP.arg("COYZDown") != "")
	{
		Settings.COYellowZoneDown = HTTP.arg("COYZDown").toFloat();
	}
	if (HTTP.arg("CORZUP") != "")
	{
		Settings.CORedZoneUP = HTTP.arg("CORZUP").toFloat();
	}
	if (HTTP.arg("CORZDown") != "")
	{
		Settings.CORedZoneDown = HTTP.arg("CORZDown").toFloat();
	}

	if (HTTP.arg("DGZUP") != "")
	{
		Settings.DustGreenZoneUP = HTTP.arg("DGZUP").toFloat();
	}
	if (HTTP.arg("DGZDown") != "")
	{
		Settings.DustGreenZoneDown = HTTP.arg("DGZDown").toFloat();
	}
	if (HTTP.arg("DYZUP") != "")
	{
		Settings.DustYellowZoneUP = HTTP.arg("DYZUP").toFloat();
	}
	if (HTTP.arg("DYZDown") != "")
	{
		Settings.DustYellowZoneDown = HTTP.arg("DYZDown").toFloat();
	}
	if (HTTP.arg("DRZUP") != "")
	{
		Settings.DustRedZoneUP = HTTP.arg("DRZUP").toFloat();
	}
	if (HTTP.arg("DRZDown") != "")
	{
		Settings.DustRedZoneDown = HTTP.arg("DRZDown").toFloat();
	}
	returnOK();
	SaveSettings();
}

void handleAlarmSettingsXML()
{
	HTTP.send(200, "text/xml", buildAlarmSettingsXML());
}
void handleNetworkSettingsXML()
{
	HTTP.send(200, "text/xml", buildNetworkSettingsXML());
}
void handleMQTTSettingsXML()
{
	HTTP.send(200, "text/xml", buildMQTTSettingsXML());
}
void handleStatsXML()
{
	HTTP.send(200, "text/xml", buildStatsXML());
}
void handleDataXML()
{
	HTTP.send(200, "text/xml", buildDataXML());
}

String buildMQTTSettingsXML()
{
	String XML = F("<?xml version='1.0'?>");
	XML += F("<Result>");

	XML += F("<MQTTDisabled>");
	if(Settings.DisableMQTT) XML += "1";
	else XML += "0";
	XML += F("</MQTTDisabled>");

	XML += F("<MQTTServer>");
	XML += Settings.MQTTServer;
	XML += F("</MQTTServer>");

	XML += F("<MQTTPort>");
	XML += Settings.MQTTPort;
	XML += F("</MQTTPort>");

	XML += F("<MQTTLogin>");
	XML += Settings.MQTTLogin;
	XML += F("</MQTTLogin>");

	XML += F("<MQTTPassword>");
	XML += Settings.MQTTPassword;
	XML += F("</MQTTPassword>");

	XML += F("</Result>");

	return XML;
}
String buildStatsXML()
{
	String XML = F("<?xml version='1.0'?>");
	XML += F("<Result>");

	XML += F("<uptime>");
	XML += millis2time();
	XML += F("</uptime>");

	XML += F("<MQTTConnectionStatus>"); //Статус соединения с MQTT
	if (MQTTClient.connected()) XML += 1;
	else XML += 0;
	XML += F("</MQTTConnectionStatus>");

	XML += F("</Result>");

	return XML;
	return "";
}
String buildNetworkSettingsXML()
{
	String XML = F("<?xml version='1.0'?>");
	XML += F("<Result>");

	XML += F("<SSID>");
	XML += Settings.SSIDCfg;
	XML += F("</SSID>");

	XML += F("<WifiPass>");
	XML += Settings.WifiPass;
	XML += F("</WifiPass>");

	XML += F("</Result>");

	return XML;
}
String buildAlarmSettingsXML()
{
	String XML = F("<?xml version='1.0'?>");
	XML += F("<Result>");

	XML += F("<AlarmState>");
	XML += Settings.AlarmOn;
	XML += F("</AlarmState>");

	XML += F("<CheckErrorState>");
	XML += Settings.checkError;
	XML += F("</CheckErrorState>");

	XML += F("<TemperatureGreenZoneUP>");
	XML += Settings.TemperatureGreenZoneUP;
	XML += F("</TemperatureGreenZoneUP>");

	XML += F("<TemperatureGreenZoneDown>");
	XML += Settings.TemperatureGreenZoneDown;
	XML += F("</TemperatureGreenZoneDown>");

	XML += F("<TemperatureYellowZoneUP>");
	XML += Settings.TemperatureYellowZoneUP;
	XML += F("</TemperatureYellowZoneUP>");

	XML += F("<TemperatureYellowZoneDown>");
	XML += Settings.TemperatureYellowZoneDown;
	XML += F("</TemperatureYellowZoneDown>");

	XML += F("<TemperatureRedZoneUP>");
	XML += Settings.TemperatureRedZoneUP;
	XML += F("</TemperatureRedZoneUP>");

	XML += F("<TemperatureRedZoneDown>");
	XML += Settings.TemperatureRedZoneDown;
	XML += F("</TemperatureRedZoneDown>");

	XML += F("<HumidityGreenZoneUP>");
	XML += Settings.HumidityGreenZoneUP;
	XML += F("</HumidityGreenZoneUP>");

	XML += F("<HumidityGreenZoneDown>");
	XML += Settings.HumidityGreenZoneDown;
	XML += F("</HumidityGreenZoneDown>");

	XML += F("<HumidityYellowZoneUP>");
	XML += Settings.HumidityYellowZoneUP;
	XML += F("</HumidityYellowZoneUP>");

	XML += F("<HumidityYellowZoneDown>");
	XML += Settings.HumidityYellowZoneDown;
	XML += F("</HumidityYellowZoneDown>");

	XML += F("<HumidityRedZoneUP>");
	XML += Settings.HumidityRedZoneUP;
	XML += F("</HumidityRedZoneUP>");

	XML += F("<HumidityRedZoneDown>");
	XML += Settings.HumidityRedZoneDown;
	XML += F("</HumidityRedZoneDown>");

	XML += F("<CO2GreenZoneUP>");
	XML += Settings.CO2GreenZoneUP;
	XML += F("</CO2GreenZoneUP>");

	XML += F("<CO2GreenZoneDown>");
	XML += Settings.CO2GreenZoneDown;
	XML += F("</CO2GreenZoneDown>");

	XML += F("<CO2YellowZoneUP>");
	XML += Settings.CO2YellowZoneUP;
	XML += F("</CO2YellowZoneUP>");

	XML += F("<CO2YellowZoneDown>");
	XML += Settings.CO2YellowZoneDown;
	XML += F("</CO2YellowZoneDown>");

	XML += F("<CO2RedZoneUP>");
	XML += Settings.CO2RedZoneUP;
	XML += F("</CO2RedZoneUP>");

	XML += F("<CO2RedZoneDown>");
	XML += Settings.CO2RedZoneDown;
	XML += F("</CO2RedZoneDown>");

	XML += F("<COGreenZoneUP>");
	XML += Settings.COGreenZoneUP;
	XML += F("</COGreenZoneUP>");

	XML += F("<COGreenZoneDown>");
	XML += Settings.COGreenZoneDown;
	XML += F("</COGreenZoneDown>");

	XML += F("<COYellowZoneUP>");
	XML += Settings.COYellowZoneUP;
	XML += F("</COYellowZoneUP>");

	XML += F("<COYellowZoneDown>");
	XML += Settings.COYellowZoneDown;
	XML += F("</COYellowZoneDown>");

	XML += F("<CORedZoneUP>");
	XML += Settings.CORedZoneUP;
	XML += F("</CORedZoneUP>");

	XML += F("<CORedZoneDown>");
	XML += Settings.CORedZoneDown;
	XML += F("</CORedZoneDown>");

	XML += F("<DustGreenZoneUP>");
	XML += Settings.DustGreenZoneUP;
	XML += F("</DustGreenZoneUP>");

	XML += F("<DustGreenZoneDown>");
	XML += Settings.DustGreenZoneDown;
	XML += F("</DustGreenZoneDown>");

	XML += F("<DustYellowZoneUP>");
	XML += Settings.DustYellowZoneUP;
	XML += F("</DustYellowZoneUP>");

	XML += F("<DustYellowZoneDown>");
	XML += Settings.DustYellowZoneDown;
	XML += F("</DustYellowZoneDown>");

	XML += F("<DustRedZoneUP>");
	XML += Settings.DustRedZoneUP;
	XML += F("</DustRedZoneUP>");

	XML += F("<DustRedZoneDown>");
	XML += Settings.DustRedZoneDown;
	XML += F("</DustRedZoneDown>");

	XML += F("</Result>");

	return XML;
}
String buildDataXML()  //Сибираем XML файл
{
	Serial1.println("Start buld XML"); //debug
	unsigned long ten = millis(); //debug
	Serial1.println(ESP.getFreeHeap()); //debug
	Serial1.println(); //debug

	String XML = F("<?xml version='1.0'?>");
	XML += F("<Result>");

	XML += F("<uptime>");
	XML += millis2time();
	XML += F("</uptime>");

	XML += F("<Temperature>");
	XML += Data.temperature_S;
	XML += F("</Temperature>");

	XML += F("<TemperatureState>");
	XML += Data.temperature_state;
	XML += F("</TemperatureState>");

	XML += F("<Humidity>");
	XML += Data.humidity_S;
	XML += F("</Humidity>");

	XML += F("<HumidityState>");
	XML += Data.humidity_state;
	XML += F("</HumidityState>");

	XML += F("<CO2>");
	XML += Data.CO2_S;
	XML += F("</CO2>");

	XML += F("<CO2State>");
	XML += Data.CO2_state;
	XML += F("</CO2State>");

	XML += F("<CO>");
	XML += Data.CO_S;
	XML += F("</CO>");

	XML += F("<COState>");
	XML += Data.CO_state;
	XML += F("</COState>");

	XML += F("<Dust>");
	XML += Data.dust_S;
	XML += F("</Dust>");

	XML += F("<DustState>");
	XML += Data.dust_state;
	XML += F("</DustState>");

	XML += F("<LPGState>");
	XML += Data.LPG_state;
	XML += F("</LPGState>");

	XML += F("<MQTTConnectionStatus>"); //Статус соединения с MQTT
	if (MQTTClient.connected()) XML += 1;
	else XML += 0;
	XML += F("</MQTTConnectionStatus>");

	XML += F("</Result>");

	Serial1.println("END buld XML"); //debug
	Serial1.println(millis()-ten); //debug
	Serial1.println(ESP.getFreeHeap()); //debug

	return XML;
}
String millis2time() 
{
	String Time = "";
	unsigned long ss;
	unsigned int mm, hh;
	ss = millis() / 1000;
	hh = ss / 3600;
	mm = (ss - hh * 3600) / 60;
	ss = (ss - hh * 3600) - mm * 60;

	for (size_t i = 0; i < cycles_counter; i++) //На случай переполнения счётчика милисекунд со старта контроллера
	{
		unsigned long temp_ss = UNSIGNED_LONG_MAX / 1000;
		unsigned int temp_hh = temp_ss / 3600;
		unsigned int temp_mm = (temp_ss - temp_hh * 3600) / 60;
		temp_ss = (temp_ss - temp_hh * 3600) - temp_mm * 60;

		ss += temp_ss;
		hh += temp_hh;
		mm += temp_mm;
	}

	if (hh < 10)Time += "0";
	Time += (String)hh + ":";
	if (mm < 10)Time += "0";
	Time += (String)mm + ":";
	if (ss < 10)Time += "0";
	Time += (String)ss;
	return Time;
}

void update(void)
{
	HTTP.on("/update", HTTP_POST, []()
	{
		HTTP.sendHeader("Connection", "close");
		HTTP.sendHeader("Access-Control-Allow-Origin", "*");
		HTTP.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
		ESP.restart();
	}, []() {
		HTTPUpload& upload = HTTP.upload();
		Serial.setDebugOutput(false);
		if (upload.status == UPLOAD_FILE_START)
		{
			//Serial.setDebugOutput(false);
			WiFiUDP::stopAll();
			//Serial1.printf("Update: %s\n", upload.filename.c_str());
			uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
			if (!Update.begin(maxSketchSpace))
			{ //start with max available size
			  //Update.printError(Serial);
			}
		}
		else if (upload.status == UPLOAD_FILE_WRITE)
		{
			if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
			{
				//Update.printError(Serial);
			}
		}
		else if (upload.status == UPLOAD_FILE_END)
		{
			if (Update.end(true))
			{ //true to set the size to the current progress
			  //Serial1.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
			}
			else
			{
				//Update.printError(Serial);
			}
			//Serial.setDebugOutput(false);
		}
		yield();
	});
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
#pragma endregion