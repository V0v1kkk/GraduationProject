#pragma region MQTT
void SendToMQTT()
{
	if (MQTTClient.connected())
	{
		if (Data.temperature_S != "-") MQTTClient.publish("Air/Temperature", Data.temperature_S.c_str());
		if (Data.temperature_Error_S != "-") MQTTClient.publish("Air/Temperature_Error", Data.temperature_Error_S.c_str());
		if (Data.humidity_S != "-") MQTTClient.publish("Air/Humidity", Data.humidity_S.c_str());
		if (Data.humidity_Error_S != "-") MQTTClient.publish("Air/Humidity_Error", Data.humidity_Error_S.c_str());

		if (Data.CO2_S != "-") MQTTClient.publish("Air/CO2", Data.CO2_S.c_str());
		if (Data.CO2_Error_S != "-") MQTTClient.publish("Air/CO2_Error", Data.CO2_Error_S.c_str());

		if (Data.CO_S != "-") MQTTClient.publish("Air/CO", Data.CO_S.c_str());
		if (Data.CO_Error_S != "-") MQTTClient.publish("Air/CO_Error", Data.CO_Error_S.c_str());

		if (Data.dust_S != "-") MQTTClient.publish("Air/Dust", Data.dust_S.c_str());
		if (Data.dust_Error_S != "-") MQTTClient.publish("Air/Dust_Error", Data.dust_Error_S.c_str());

		if (Data.LPG_S != "-") MQTTClient.publish("Air/LPG", String(Data.LPG_state).c_str());
	}
}
bool ConnectToMQTT()
{
	bool tempflag = false;
	Serial1.println("Start connect to MQTT"); //debug
	MQTTClient.setServer(Settings.MQTTServer.c_str(), (unsigned int)Settings.MQTTPort.toInt());
	if (MQTTClient.connect("Prototype_1", Settings.MQTTLogin.c_str(), Settings.MQTTPassword.c_str()))
	{
		MQTTClient.publish("Test", WiFi.localIP().toString().c_str()); //debug
		tempflag = true;
	}
	Serial1.println("End connect to MQTT"); //debug
	return tempflag;
}

#pragma endregion