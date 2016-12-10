#pragma region DataWork

void ParseData(String rawdata)
{
	
	bool newdata = false; //Флаг поступления новых данных
	//////////////////////////////////////
	String data = getStringPartByNr(rawdata, '*', 0);
	String CRC_S = getStringPartByNr(rawdata, '*', 1);

	if (CRC_S == "") return; //Нет CRC - данные не корректны
	byte CRC = 0;
	for (size_t i = 0; i < data.length(); i++)
	{
		CRC = CRC + data.charAt(i);
	}
	if (CRC_S.toInt() != CRC) return;  //Если не совпала контрольная сумма

	CRC_S.~String();
	//////////////////////////////////////

	String microclimat = getStringPartByNr(data, '|', 0);
	String CO2 = getStringPartByNr(data, '|', 1);
	String CO = getStringPartByNr(data, '|', 2);
	String dust = getStringPartByNr(data, '|', 3);
	String LPG = getStringPartByNr(data, '|', 4);
	data.~String();

	String temp = "";
	//Микроклимат
	temp = getStringPartByNr(microclimat, ';', 0);
	if (temp == "-1") 
	{ 
		Data.temperature_S = "-";
		Data.temperature = -999.0; 
	}
	else 
	{ 
		Data.temperature_S = temp;
		Data.temperature = Data.temperature_S.toFloat(); 
		newdata = true;
	}

	temp = getStringPartByNr(microclimat, ';', 1);
	if (temp == "-1")
	{ 
		Data.temperature_Error_S = "-";
		Data.temperature_Error = -999.0; 
	}
	else 
	{ 
		Data.temperature_Error_S = temp;
		Data.temperature_Error = Data.temperature_Error_S.toFloat(); 
		newdata = true;
	}

	temp = getStringPartByNr(microclimat, ';', 2);
	if (temp == "-1")
	{ 
		Data.humidity_S = "-";
		Data.humidity = -999.0; 
	}
	else 
	{ 
		Data.humidity_S = temp;
		Data.humidity = Data.humidity_S.toFloat(); 
		newdata = true;
	}

	temp = getStringPartByNr(microclimat, ';', 3);
	if (temp == "-1")
	{ 
		Data.humidity_Error_S = "-";
		Data.humidity_Error = -999.0; 
	}
	else 
	{ 
		Data.humidity_Error_S = temp;
		Data.humidity_Error = Data.humidity_Error_S.toFloat(); 
		newdata = true;
	}
	microclimat.~String();

	//СО2
	temp = getStringPartByNr(CO2, ';', 0);
	if (temp == "-1")
	{ 
		Data.CO2_S = "-";
		Data.CO2 = -999.0; 
	}
	else 
	{ 
		Data.CO2_S = temp;
		Data.CO2 = Data.CO2_S.toFloat(); 
		newdata = true;
	}

	temp = getStringPartByNr(CO2, ';', 1);
	if (temp == "-1")
	{ 
		Data.CO2_Error_S = "-";
		Data.CO2_Error = -999.0; 
	}
	else 
	{ 
		Data.CO2_Error_S = temp;
		Data.CO2_Error = Data.CO2_Error_S.toFloat(); 
		newdata = true;
	}
	CO2.~String();

	///СО
	temp = getStringPartByNr(CO, ';', 0);
	if (temp == "-1")
	{ 
		Data.CO_S = "-";
		Data.CO = -999.0; 
	}
	else 
	{ 
		Data.CO_S = temp;
		Data.CO = Data.CO_S.toFloat();
		newdata = true;
	}

	temp = getStringPartByNr(CO, ';', 1);
	if (temp == "-1")
	{ 
		Data.CO_Error_S = "-";
		Data.CO_Error = -999.0; 
	}
	else 
	{ 
		Data.CO_Error_S = temp;
		Data.CO_Error = Data.CO_Error_S.toFloat(); 
		newdata = true;
	}
	CO.~String();

	//Взвешенные частицы
	temp = getStringPartByNr(dust, ';', 0);
	if (temp == "-1")
	{ 
		Data.dust_S = "-";
		Data.dust = -999.0; 
	}
	else 
	{ 
		Data.dust_S = temp;
		Data.dust = Data.dust_S.toFloat();
		newdata = true;
	}

	temp = getStringPartByNr(dust, ';', 1);
	if (temp == "-1")
	{ 
		Data.dust_Error_S = "-";
		Data.dust_Error = -999.0; 
	}
	else 
	{ 
		Data.dust_Error_S = temp;
		Data.dust_Error = Data.dust_Error_S.toFloat(); 
		newdata = true;
	}
	dust.~String();

	//Газ
	temp = getStringPartByNr(LPG, ';', 0);
	if (temp == "-1")
	{ 
		Data.LPG_S = "-";
		Data.LPG = -999.0; 
	}
	else 
	{ 
		Data.LPG_S = temp;
		Data.LPG = Data.LPG_S.toFloat();
		newdata = true;
	}

	temp = getStringPartByNr(LPG, ';', 1);
	if (temp == "-1")
	{ 
		Data.LPG_Error_S = "-";
		Data.LPG_Error = -999.0; 
	}
	else 
	{
		Data.LPG_Error_S = temp;
		Data.LPG_Error = Data.LPG_Error_S.toFloat(); 
		newdata = true;
	}
	LPG.~String();

	
	if (newdata) //Если мы получили какие-то данные, то анализируем их
	{
		AnalizeAll();
		SendToMQTT();
		SendToDisplay();
	}
}

void AnalizeAll() //Анализирует полученные значения и выставляет уровни тревоги
{
	int temp = 0;
	//Температура
	if (Data.temperature == -999)
	{
		Data.temperature_state = 0;
	}
	else
	{
		if (Settings.checkError && (abs(Data.temperature) < Data.temperature_Error*1.96))
		{

		}
		else
		{
			temp = Analize(Data.temperature, Settings.TemperatureGreenZoneUP, Settings.TemperatureGreenZoneDown, Settings.TemperatureYellowZoneUP, Settings.TemperatureYellowZoneDown, Settings.TemperatureRedZoneUP, Settings.TemperatureRedZoneDown);
			if ((Settings.AlarmOn) && (temp == 3) && (Data.temperature_state != 3))
			{
				SendForPlaying(1);
			}
		}
		Data.temperature_state = temp;
	}

	//Влажность
	if (Data.humidity == -999)
	{
		Data.humidity_state = 0;
	}
	else
	{
		if (Settings.checkError && (Data.humidity < Data.humidity_Error*1.96))
		{

		}
		else
		{
			temp = Analize(Data.humidity, Settings.HumidityGreenZoneUP, Settings.HumidityGreenZoneDown, Settings.HumidityYellowZoneUP, Settings.HumidityYellowZoneDown, Settings.HumidityRedZoneUP, Settings.HumidityRedZoneDown);
			if ((Settings.AlarmOn) && (temp == 3) && (Data.humidity_state != 3))
			{
				SendForPlaying(2);
			}
		}
		Data.humidity_state = temp;
	}

	//CO2
	if (Data.CO2 == -999)
	{
		Data.CO2_state = 0;
	}
	else
	{
		if (Settings.checkError && (Data.CO2 < Data.CO2_Error*1.96))
		{

		}
		else
		{
			temp = Analize(Data.CO2, Settings.CO2GreenZoneUP, Settings.CO2GreenZoneDown, Settings.CO2YellowZoneUP, Settings.CO2YellowZoneDown, Settings.CO2RedZoneUP, Settings.CO2RedZoneDown);
			if ((Settings.AlarmOn) && (temp == 3) && (Data.CO2_state != 3))
			{
				SendForPlaying(3);
			}
		}
		Data.CO2_state = temp;
	}

	//CO
	if (Data.CO == -999)
	{
		Data.CO_state = 0;
	}
	else
	{
		if (Settings.checkError && (Data.CO < Data.CO_Error*1.96))
		{

		}
		else
		{
			temp = Analize(Data.CO, Settings.COGreenZoneUP, Settings.COGreenZoneDown, Settings.COYellowZoneUP, Settings.COYellowZoneDown, Settings.CORedZoneUP, Settings.CORedZoneDown);
			if ((Settings.AlarmOn) && (temp == 3) && (Data.CO_state != 3))
			{
				SendForPlaying(4);
			}
		}
		Data.CO_state = temp;
	}

	//Пыль
	if (Data.dust == -999)
	{
		Data.dust_state = 0;
	}
	else
	{
		if (Settings.checkError && (Data.dust < Data.dust_Error*1.96))
		{

		}
		else
		{
			temp = Analize(Data.dust, Settings.DustGreenZoneUP, Settings.DustGreenZoneDown, Settings.DustYellowZoneUP, Settings.DustYellowZoneDown, Settings.DustRedZoneUP, Settings.DustRedZoneDown);
			if ((Settings.AlarmOn) && (temp == 3) && (Data.dust_state != 3))
			{
				SendForPlaying(5);
			}
		}
		Data.dust_state = temp;
	}

	//LPG
	if (Data.LPG == -999)
	{
		Data.LPG_state = 0;
	}
	else
	{
		if (Settings.checkError && (Data.LPG < Data.LPG_Error*1.96))
		{

		}
		else
		{
			temp = Analize(Data.LPG, Settings.LPGGreenZoneUP, Settings.LPGGreenZoneDown, Settings.LPGYellowZoneUP, Settings.LPGYellowZoneDown, Settings.LPGRedZoneUP, Settings.LPGRedZoneDown);
			if ((Settings.AlarmOn) && (temp == 3) && (Data.LPG_state != 3))
			{
				SendForPlaying(6);
			}
		}
		Data.LPG_state = temp;
	}
}

int Analize(float value, float GreenZoneUP, float GreenZoneDown, float YellowZoneUP, float YellowZoneDown, float RedZoneUP, float RedZoneDown)
{
	int temp=-1;
	if ((value >= RedZoneDown) && (value <= RedZoneUP))
	{
		temp = 3;
	}
	if ((value >= YellowZoneDown) && (value <= YellowZoneUP))
	{
		temp = 2;
	}
	if ((value >= GreenZoneDown) && (value <= GreenZoneUP))
	{
		temp = 1;
	}

	if (temp == -1) temp = 0;
	return temp;
}

String getStringPartByNr(String data, char separator, int index)
{
	if (data == "") return "";
	int stringData = 0;
	String dataPart = "";

	for (int i = 0; i<data.length() - 1; i++)
	{
		if (data[i] == separator)
		{
			stringData++;
		}
		else if (stringData == index)
		{
			dataPart.concat(data[i]);
		}
		else if (stringData>index)
		{
			return dataPart;
			break;
		}
	}
	return dataPart;
}

#pragma endregion