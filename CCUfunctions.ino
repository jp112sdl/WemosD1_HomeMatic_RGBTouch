void setStateCCU(String id, String type, String value) {
  if (WiFi.status() == WL_CONNECTED && String(HomeMaticConfig.ccuIP) != "" && String(HomeMaticConfig.ccuIP) != "0.0.0.0") {
    myGLCD.setColor(0, 0, 255);
    myGLCD.drawLine(319, 0, 319, 240);
    http.setTimeout(HTTPTimeOut);
    String url = "http://" + String(HomeMaticConfig.ccuIP) + ":8181/wemos.exe?ret=dom.GetObject(%22BidCos-RF." + id + ":" + type + "%22).State(" + value + ")";
    Serial.println("setStateCCU URL: " + url);
    http.begin(url);
    int httpCode = http.GET();
    Serial.println("setStateCCU with id: " + id + ", Type = " + type + " und Wert " + value + " ergab: " + String(httpCode));

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.println("setStateCCU Payload = " + payload);
    }
    if (httpCode != 200) {
      myGLCD.setColor(255, 0, 0);
      myGLCD.drawLine(319, 0, 319, 240);
    }
    else {
      myGLCD.setColor(0, 255, 0);
      myGLCD.drawLine(319, 0, 319, 240);
      lastSyncMillis = millis();
    }
    http.end();
  }
}

String getStateFromCCU(String id, String type) {
  if (WiFi.status() == WL_CONNECTED && String(HomeMaticConfig.ccuIP) != "" && String(HomeMaticConfig.ccuIP) != "0.0.0.0") {
    myGLCD.setColor(0, 0, 255);
    myGLCD.drawLine(319, 0, 319, 240);
    http.setTimeout(HTTPTimeOut);

    http.begin("http://" + String(HomeMaticConfig.ccuIP) + ":8181/wemos.exe?ret=dom.GetObject(%22BidCos-RF." + id + ":" + type + "%22).State()");
    int httpCode = http.GET();
    String payload = "error";
    if (httpCode > 0) {
      payload = http.getString();
    }
    if (httpCode != 200) {
      myGLCD.setColor(255, 0, 0);
      myGLCD.drawLine(319, 0, 319, 240);
    }
    else {
      myGLCD.setColor(0, 255, 0);
      myGLCD.drawLine(319, 0, 319, 240);
    }
    http.end();

    payload = payload.substring(payload.indexOf("<ret>"));
    payload = payload.substring(5, payload.indexOf("</ret>"));
    Serial.println("getStateFromCCU Payload: " + payload);
    return payload;
  } else {
    return "error";
  }
}

