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
  Serial.println("Calibration Params: "+String(vi1)+","+String(vj1)+","+String(vi2)+","+String(vj2));
}


