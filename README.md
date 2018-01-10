# WemosD1_HomeMatic_RGBTouch
TFT/Touchscreen-Fernbedienung für HomeMatic RGB Controller und/oder UP-Dimmer<br>
Die Steuerung erfolgt direkt über die Seriennummer des HomeMatic-Geräts.<br>
Es ist **keine** Zusatzsoftware auf der CCU2 notwendig.

<img src="https://github.com/jp112sdl/WemosD1_HomeMatic_RGBTouch/blob/master/Images/sample2.jpg" width=200><img src="https://github.com/jp112sdl/WemosD1_HomeMatic_RGBTouch/blob/master/Images/sample1.jpg" width=200><img src="https://github.com/jp112sdl/WemosD1_HomeMatic_RGBTouch/blob/master/Images/sample3.jpg" width=200>

### Bauteile:
- 1x Wemos D1 mini
- 1x TFT Touchscreen, ILI9341 kompatibel und mit SPI Anschluss (z.B. [hier](https://www.ebay.de/itm/172726994916))
- _bei Akkubetrieb empfiehlt sich zusätzlich noch ein Wemos BatteryShield und ein 3.7V LiPo Akku_
  - _auf die Stromversorgung wird hier nicht weiter eingegangen_
  
###  


Wiring: [Link](http://usemodj.com/2016/03/21/esp8266-display-2-8-inch-touch-lcd-with-wemos-d1-mini-board/)<br>
Config (Start device keeping touchscreen touched):
![wificonfig](Images/wifi_config.png)
