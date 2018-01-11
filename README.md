# WemosD1 HomeMatic RGBTouch Controller
TFT/Touchscreen-Fernbedienung für HomeMatic RGB Controller und/oder UP-Dimmer<br>
Die Steuerung erfolgt direkt über die Seriennummer des HomeMatic-Geräts.<br>
Es ist **keine** Zusatzsoftware auf der CCU2 notwendig.

<img src="https://github.com/jp112sdl/WemosD1_HomeMatic_RGBTouch/blob/master/Images/sample2.jpg" width=200><img src="https://github.com/jp112sdl/WemosD1_HomeMatic_RGBTouch/blob/master/Images/sample1.jpg" width=200><img src="https://github.com/jp112sdl/WemosD1_HomeMatic_RGBTouch/blob/master/Images/sample3.jpg" width=200>

### Bauteile:
- 1x Wemos D1 mini
- 1x TFT Touchscreen, ILI9341 kompatibel und mit SPI Anschluss (z.B. [hier](https://www.ebay.de/itm/172726994916))
- 1x beliebiger Taster
- 1x Widerstand, irgendwas zwischen 1k...10k Ohm (dient nur als Pullup)
- _bei Akkubetrieb empfiehlt sich zusätzlich noch ein Wemos BatteryShield und ein 3.7V LiPo Akku_
  - _auf die Stromversorgung wird hier nicht weiter eingegangen_
  
### Verdrahtung:
<img src="https://github.com/jp112sdl/WemosD1_HomeMatic_RGBTouch/blob/master/Images/wiring.png" width=400><br>
Die Fritzing-Datei kann auch [heruntergeladen](https://github.com/jp112sdl/WemosD1_HomeMatic_RGBTouch/blob/master/Images/Schaltplan.fzz) werden.

### Inbetriebnahme:
1. Touchscreen berühren und dabei das Gerät einschalten
2. AP-Modus wird gestartet<br>
<img src="https://github.com/jp112sdl/WemosD1_HomeMatic_RGBTouch/blob/master/Images/ap_modus.jpg" width=200><br>
3. Nach neuen WLANs am Notebook suchen und mit dem WLAN WemosD1-xx:xx:xx:xx:xx:xx verbinden
4. Konfigurationsseite öffnen: http://192.168.4.1 und dort die Daten entsprechend eingeben, anschließend speichern.

<img src="https://github.com/jp112sdl/WemosD1_HomeMatic_RGBTouch/blob/master/Images/wifi_config.png" width=400><br>

**Erklärung der Felder:**
- SSID / WLAN-Key, IP der CCU2, RGB Dimmer Seriennummer und UP Dimmer Seriennummer sind wohl selbsterklärend
- **es ist auch möglich nur EINEN von beiden Dimmern zu benutzen! Es ist dann das Feld (Seriennummer) des ungenutzten Dimmertyps leer zu lassen.**
- ON_LEVEL: Die Buttons AN und AUS am rechten Bildschirmrand dienen dem schnellen Ein- und Ausschalten des UP-Dimmers. Bei "AUS" wird 0% angefahren, bei "EIN" der im ON_LEVEL definierte Wert.
- HM Sync: wenn aktiviert, wird alle 30 Sekunden die CCU2 abgefragt, ob die angezeigten Werte noch dem IST-Zustand entsprechen. Wenn nicht (weil bspw. jemand per Schalter den Dimmer betätigt hat) wird der neue/aktuelle Wert am TFT angezeigt.



### Gehäuse:
Mein verwendetes Gehäuse gibt es als [.stl](https://github.com/jp112sdl/WemosD1_HomeMatic_RGBTouch/blob/master/Images/HomeMatic%20RGB%20Dimmer.stl) und als [.obj](https://github.com/jp112sdl/WemosD1_HomeMatic_RGBTouch/blob/master/Images/HomeMaticRGBDimmerOBJ.zip) im komprimierten zip-File.

### Kalibrierung des Touchscreens:
<img src="https://github.com/jp112sdl/WemosD1_HomeMatic_RGBTouch/blob/master/Images/calib1.jpg" width=200><br>
- Bei gedrücktem Taster den Wemos einschalten.<br>
- Es erscheint ein Fadenkreuz in der Ecke oben links
- Das Fadenkreuz (am besten mit einem Stift) antippen
- Es erscheint ein Fadenkreuz in der Ecke unten rechts
- Das Fadenkreuz (am besten mit einem Stift) antippen

Die Kalibrierung ist nun beendet. Die Kalibrierungsdaten werden gespeichert und sind dauerhaft gespeichert.
Der Vorgang kann jederzeit wiederholt werden


Wer "Danke" sagen möchte (in Form einer kleinen Spende), kann gern hier klicken: [![paypal](https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=UBX8NFNYVWW8N)
