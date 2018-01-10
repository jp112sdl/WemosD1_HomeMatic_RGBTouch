bool doWifiConnect() {
  String _ssid = WiFi.SSID();
  String _psk = WiFi.psk();

  String _pskMask = "";
  for (int i = 0; i < _psk.length(); i++) {
    _pskMask += "*";
  }
  Serial.println("ssid = " + _ssid + ", psk = " + _pskMask);

  const char* ipStr = WemosNetConfig.ip; byte ipBytes[4]; parseBytes(ipStr, '.', ipBytes, 4, 10);
  const char* netmaskStr = WemosNetConfig.netmask; byte netmaskBytes[4]; parseBytes(netmaskStr, '.', netmaskBytes, 4, 10);
  const char* gwStr = WemosNetConfig.gw; byte gwBytes[4]; parseBytes(gwStr, '.', gwBytes, 4, 10);

  if (!startWifiManager && _ssid != "" && _psk != "" ) {
    Serial.println(F("Connecting WLAN the classic way..."));
    myGLCD.print("Verbinde WLAN", 175, 10, 90);
    myGLCD.drawBitmap (wifiIconX, wifiIconY, 32, 32, wifiicon32, 90 , 16 , 16); yield();
    WiFi.mode(WIFI_STA);
    WiFi.hostname(Hostname);
    WiFi.begin(_ssid.c_str(), _psk.c_str());
    int waitCounter = 0;
    if (String(WemosNetConfig.ip) != "0.0.0.0")
      WiFi.config(IPAddress(ipBytes[0], ipBytes[1], ipBytes[2], ipBytes[3]), IPAddress(gwBytes[0], gwBytes[1], gwBytes[2], gwBytes[3]), IPAddress(netmaskBytes[0], netmaskBytes[1], netmaskBytes[2], netmaskBytes[3]));

    while (WiFi.status() != WL_CONNECTED) {
      waitCounter++;
      Serial.print(".");
      if (waitCounter == 30) {
        return false;
      }
      delay(500);
    }
    Serial.println("Wifi Connected");
    printWifiStatus();
    return true;
  } else {
    myGLCD.print("AP-Modus", 175, 55, 90);
    myGLCD.drawBitmap (wifiIconX, wifiIconY, 32, 32, wifiicon32, 90 , 16 , 16); yield();
    WiFiManager wifiManager;
    wifiManager.setDebugOutput(true);
    digitalWrite(LED_BUILTIN, LOW);
    wifiManager.setAPCallback(configModeCallback);
    wifiManager.setSaveConfigCallback(saveConfigCallback);
    WiFiManagerParameter custom_ccuip("ccu", "IP der CCU2", HomeMaticConfig.ccuIP, IPSIZE, 0, "pattern='((^|\\.)((25[0-5])|(2[0-4]\\d)|(1\\d\\d)|([1-9]?\\d))){4}$'");
    WiFiManagerParameter custom_rgbdimmerserial("rgbdimmerserial", "RGB Dimmer Seriennummer", HomeMaticConfig.RGB_Dimmer_Serial, VARIABLESIZE);
    WiFiManagerParameter custom_dimmerserial("dimmerserial", "Dimmer Seriennummer", HomeMaticConfig.UP_Dimmer_Serial, VARIABLESIZE);
    String strOnLevel = String(HomeMaticConfig.OnLevel);
    WiFiManagerParameter custom_onlevel("onlevel", "ON_LEVEL", String(HomeMaticConfig.OnLevel).c_str(), 10, 0, "pattern='[0-9]{1,3}'");

    char*chrEnableSync = "0";
    if (HomeMaticConfig.enableSync) chrEnableSync =  "1" ;
    WiFiManagerParameter custom_enablesync("enablesync", "HM Sync", chrEnableSync, 8, 1);

    WiFiManagerParameter custom_ip("custom_ip", "IP-Adresse", (String(WemosNetConfig.ip) != "0.0.0.0") ? WemosNetConfig.ip : "", IPSIZE, 0, "pattern='((^|\\.)((25[0-5])|(2[0-4]\\d)|(1\\d\\d)|([1-9]?\\d))){4}$'");
    WiFiManagerParameter custom_netmask("custom_netmask", "Netzmaske", (String(WemosNetConfig.netmask) != "0.0.0.0") ? WemosNetConfig.netmask : "", IPSIZE, 0, "pattern='((^|\\.)((25[0-5])|(2[0-4]\\d)|(1\\d\\d)|([1-9]?\\d))){4}$'");
    WiFiManagerParameter custom_gw("custom_gw", "Gateway",  (String(WemosNetConfig.gw) != "0.0.0.0") ? WemosNetConfig.gw : "", IPSIZE, 0, "pattern='((^|\\.)((25[0-5])|(2[0-4]\\d)|(1\\d\\d)|([1-9]?\\d))){4}$'");
    WiFiManagerParameter custom_text("<br/><br><div>Statische IP (wenn leer, dann DHCP):</div>");
    wifiManager.addParameter(&custom_ccuip);
    wifiManager.addParameter(&custom_rgbdimmerserial);
    wifiManager.addParameter(&custom_dimmerserial);
    wifiManager.addParameter(&custom_onlevel);
    wifiManager.addParameter(&custom_enablesync);
    wifiManager.addParameter(&custom_text);
    wifiManager.addParameter(&custom_ip);
    wifiManager.addParameter(&custom_netmask);
    wifiManager.addParameter(&custom_gw);

    String Hostname = "WemosD1-" + WiFi.macAddress();

    wifiManager.setConfigPortalTimeout(ConfigPortalTimeout);

    if (startWifiManager == true) {
      if (_ssid == "" || _psk == "" ) {
        wifiManager.resetSettings();
      }
      else {
        if (!wifiManager.startConfigPortal(Hostname.c_str())) {
          Serial.println(F("WM: failed to connect and hit timeout"));
          delay(500);
          ESP.restart();
        }
      }
    }

    wifiManager.setSTAStaticIPConfig(IPAddress(ipBytes[0], ipBytes[1], ipBytes[2], ipBytes[3]), IPAddress(gwBytes[0], gwBytes[1], gwBytes[2], gwBytes[3]), IPAddress(netmaskBytes[0], netmaskBytes[1], netmaskBytes[2], netmaskBytes[3]));

    wifiManager.autoConnect(Hostname.c_str());

    Serial.println(F("Wifi Connected"));
    Serial.println("CUSTOM STATIC IP: " + String(WemosNetConfig.ip) + " Netmask: " + String(WemosNetConfig.netmask) + " GW: " + String(WemosNetConfig.gw));
    if (wm_shouldSaveConfig) {
      if (String(custom_ip.getValue()).length() > 5) {
        Serial.println("Custom IP Address is set!");
        strcpy(WemosNetConfig.ip, custom_ip.getValue());
        strcpy(WemosNetConfig.netmask, custom_netmask.getValue());
        strcpy(WemosNetConfig.gw, custom_gw.getValue());

      } else {
        strcpy(WemosNetConfig.ip,      "0.0.0.0");
        strcpy(WemosNetConfig.netmask, "0.0.0.0");
        strcpy(WemosNetConfig.gw,      "0.0.0.0");
      }

      strcpy(HomeMaticConfig.ccuIP, custom_ccuip.getValue());
      strcpy(HomeMaticConfig.RGB_Dimmer_Serial, custom_rgbdimmerserial.getValue());
      strcpy(HomeMaticConfig.UP_Dimmer_Serial, custom_dimmerserial.getValue());
      HomeMaticConfig.enableSync = (atoi(custom_enablesync.getValue()) == 1);
      HomeMaticConfig.OnLevel = atoi(custom_onlevel.getValue());

      saveSystemConfig();

      delay(100);
      ESP.restart();
    }
    return true;
  }
}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("AP-Modus ist aktiv!");
}

void saveConfigCallback () {
  Serial.println("Should save config");
  wm_shouldSaveConfig = true;
}

void parseBytes(const char* str, char sep, byte* bytes, int maxBytes, int base) {
  for (int i = 0; i < maxBytes; i++) {
    bytes[i] = strtoul(str, NULL, base);
    str = strchr(str, sep);
    if (str == NULL || *str == '\0') {
      break;
    }
    str++;
  }
}

void printWifiStatus() {
  Serial.println("SSID: " + WiFi.SSID());
  Serial.println("IP Address: " + IpAddress2String(WiFi.localIP()));
  Serial.println("Gateway Address: " + IpAddress2String(WiFi.gatewayIP()));
  Serial.println("signal strength (RSSI):" + String(WiFi.RSSI()) + " dBm");
}

