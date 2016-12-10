#pragma region Wi-fi
bool findWiFiAndConnect()
{
	bool flag = false;
	String networkname;
	int numberofnetworks = WiFi.scanNetworks();
	Serial1.println("Search networks...");//debug
	if (numberofnetworks == 0)
	{
		return false;
	}
	else
	{
		if (Settings.SSIDCfg != "")
		{
			for (int i = 0; i < numberofnetworks; ++i)
			{
				if (WiFi.SSID(i) == Settings.SSIDCfg)
				{
					networkname = Settings.SSIDCfg;
					flag = true;
					WiFi.begin(Settings.SSIDCfg.c_str(), Settings.WifiPass.c_str());
				}
			}
		}
		if(flag)
		{
			Serial1.print("Connecting to " + networkname); //debug
			unsigned long temp = millis();
			while (WiFi.status() != WL_CONNECTED)
			{
				delay(500);
				if (millis() - temp > 10000UL)
				{
					WiFi.disconnect();
					return false;
				}
			}
			return true;
		}
		else
		{
			return false;
		}
	}
}
#pragma endregion