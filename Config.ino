#define JSONCONFIG_IP                     "ip"
#define JSONCONFIG_NETMASK                "netmask"
#define JSONCONFIG_GW                     "gw"
#define JSONCONFIG_CCUIP                  "ccuip"
#define JSONCONFIG_RGBDIMMER              "rgbdimser"
#define JSONCONFIG_UPDIMMER               "updimmer"
#define JSONCONFIG_ENABLESYNC             "sync"
#define JSONCONFIG_ONLEVEL                "onlevel"

bool loadSystemConfig() {
  Serial.println(F("loadSystemConfig mounting FS..."));
  if (SPIFFS.begin()) {
    Serial.println(F("loadSystemConfig mounted file system"));
    if (SPIFFS.exists("/" + configJsonFile)) {
      Serial.println(F("loadSystemConfig reading config file"));
      File configFile = SPIFFS.open("/" + configJsonFile, "r");
      if (configFile) {
        Serial.println(F("loadSystemConfig opened config file"));
        size_t size = configFile.size();
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        Serial.println("Content of JSON Config-File: /" + configJsonFile);
        json.printTo(Serial);
        Serial.println();
        if (json.success()) {
          Serial.println("\nJSON OK");
          ((json[JSONCONFIG_IP]).as<String>()).toCharArray(WemosNetConfig.ip, IPSIZE);
          ((json[JSONCONFIG_NETMASK]).as<String>()).toCharArray(WemosNetConfig.netmask, IPSIZE);
          ((json[JSONCONFIG_GW]).as<String>()).toCharArray(WemosNetConfig.gw, IPSIZE);
          ((json[JSONCONFIG_CCUIP]).as<String>()).toCharArray(HomeMaticConfig.ccuIP, IPSIZE);
          ((json[JSONCONFIG_RGBDIMMER]).as<String>()).toCharArray(HomeMaticConfig.RGB_Dimmer_Serial, VARIABLESIZE);
          ((json[JSONCONFIG_UPDIMMER]).as<String>()).toCharArray(HomeMaticConfig.UP_Dimmer_Serial, VARIABLESIZE);
          HomeMaticConfig.enableSync = json[JSONCONFIG_ENABLESYNC];
          HomeMaticConfig.OnLevel = json[JSONCONFIG_ONLEVEL];
        } else {
          Serial.println(F("\nloadSystemConfig ERROR loading config"));
        }
      }
      return true;
    } else {
      Serial.println("/" + configJsonFile + " not found.");
      return false;
    }
    SPIFFS.end();
  } else {
    Serial.println(F("loadSystemConfig failed to mount FS"));
    return false;
  }
}

bool saveSystemConfig() {
  SPIFFS.begin();
  Serial.println(F("saving system config"));
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json[JSONCONFIG_IP] = WemosNetConfig.ip;
  json[JSONCONFIG_NETMASK] = WemosNetConfig.netmask;
  json[JSONCONFIG_GW] = WemosNetConfig.gw;
  json[JSONCONFIG_CCUIP] = HomeMaticConfig.ccuIP;
  json[JSONCONFIG_RGBDIMMER] = HomeMaticConfig.RGB_Dimmer_Serial;
  json[JSONCONFIG_UPDIMMER] = HomeMaticConfig.UP_Dimmer_Serial;
  json[JSONCONFIG_ENABLESYNC] = HomeMaticConfig.enableSync;
  json[JSONCONFIG_ONLEVEL] = HomeMaticConfig.OnLevel;

  SPIFFS.remove("/" + configJsonFile);
  File configFile = SPIFFS.open("/" + configJsonFile, "w");
  if (!configFile) {
    Serial.println(F("failed to open config file for writing"));
    return false;
  }
  json.printTo(Serial);
  Serial.println();
  json.printTo(configFile);
  configFile.close();
  SPIFFS.end();
  return true;
}



