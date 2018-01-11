static void calibratePoint(uint16_t x, uint16_t y, uint16_t &vi, uint16_t &vj) {
  Serial.println("calibratePoint(" + String(x) + "," + String(y) + "," + String(vi) + "," + String(vj) + ")");
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawLine((uint16_t)myGLCD.getDisplayXSize() - (x - 8), y, (uint16_t)myGLCD.getDisplayXSize() - (x + 8), y);
  myGLCD.drawLine((uint16_t)myGLCD.getDisplayXSize() - x, y - 8, (uint16_t)myGLCD.getDisplayXSize() - x, y + 8);
  while (!myTouch.isTouching()) {
    delay(10);
  }
  myTouch.getRaw(vi, vj);
  myGLCD.setColor(0, 0, 0);
  myGLCD.drawLine((uint16_t)myGLCD.getDisplayXSize() - (x - 8), y, (uint16_t)myGLCD.getDisplayXSize() - (x + 8), y);
  myGLCD.drawLine((uint16_t)myGLCD.getDisplayXSize() - x, y - 8, (uint16_t)myGLCD.getDisplayXSize() - x, y + 8);
}

void calibrate() {
  uint16_t x1, y1, x2, y2;
  uint16_t vi1, vj1, vi2, vj2;
  myTouch.getCalibrationPoints(x1, y1, x2, y2);
  Serial.println("myTouch.getCalibrationPoints(" + String(x1) + "," + String(y1) + "," + String(x2) + "," + String(y2) + ")");
  calibratePoint(y1, x1, vi1, vj1);
  delay(250);
  calibratePoint(y2, x2, vi2, vj2);
  myTouch.setCalibration(vi1, vj1, vi2, vj2);
  Serial.println("Calibration Params: " + String(vi1) + "," + String(vj1) + "," + String(vi2) + "," + String(vj2));
  TouchCalibration.P1 = vi1;
  TouchCalibration.P2 = vj1;
  TouchCalibration.P3 = vi2;
  TouchCalibration.P4 = vj2;
  saveCalibrationConfig();
}

bool loadCalibrationConfig() {
  Serial.println(F("loadCalibrationConfig mounting FS..."));
  if (SPIFFS.begin()) {
    Serial.println(F("loadCalibrationConfig mounted file system"));
    if (SPIFFS.exists("/" + calibJsonFile)) {
      Serial.println(F("loadCalibrationConfig reading config file"));
      File calibFile = SPIFFS.open("/" + calibJsonFile, "r");
      if (calibFile) {
        Serial.println(F("loadCalibrationConfig opened config file"));
        size_t size = calibFile.size();
        std::unique_ptr<char[]> buf(new char[size]);

        calibFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        Serial.println("Content of JSON Config-File: /" + calibJsonFile);
        json.printTo(Serial);
        Serial.println();
        if (json.success()) {
          Serial.println("\nJSON OK");
          TouchCalibration.P1 = json["p1"];
          TouchCalibration.P2 = json["p2"];
          TouchCalibration.P3 = json["p3"];
          TouchCalibration.P4 = json["p4"];
        } else {
          Serial.println(F("\nloadCalibrationConfig ERROR loading config"));
        }
      }
      return true;
    } else {
      Serial.println("/" + calibJsonFile + " not found.");
      return false;
    }
    SPIFFS.end();
  } else {
    Serial.println(F("loadCalibrationConfig failed to mount FS"));
    return false;
  }
}

bool saveCalibrationConfig() {
  SPIFFS.begin();
  Serial.println(F("saving calibration config"));
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["p1"] = TouchCalibration.P1;
  json["p2"] = TouchCalibration.P2;
  json["p3"] = TouchCalibration.P3;
  json["p4"] = TouchCalibration.P4;

  SPIFFS.remove("/" + calibJsonFile);
  File calibFile = SPIFFS.open("/" + calibJsonFile, "w");
  if (!calibFile) {
    Serial.println(F("failed to open calibration config file for writing"));
    return false;
  }
  json.printTo(Serial);
  Serial.println();
  json.printTo(calibFile);
  calibFile.close();
  SPIFFS.end();
  return true;
}
