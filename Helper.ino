bool drawCurrentDimLevel(String currentLEVELValue) {
  if (currentLEVELValue != "error") {
    float flValue = currentLEVELValue.toFloat() * 100;
    int percentValue = (int)flValue;
    String displayPercent = "   " + String(percentValue);
    displayPercent = displayPercent.substring(displayPercent.length() - 3) + "%";
    myGLCD.setColor(255, 255, 255);
    myGLCD.print(displayPercent, percentX, percentY, 90);

    draw_used_unused(RGBstartX, RGBstartY);

    for (int tPosY = posY; tPosY <= posY + posEndY; tPosY++)
      myGLCD.drawLine(posX, tPosY, posX + (percentValue * posEndX / 100), tPosY);

    yield();
    return true;
  } else {
    return false;
  }
}
