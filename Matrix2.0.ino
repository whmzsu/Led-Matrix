//Matrix Programm
//By Jonathan
//Made for 16x16 Matrix WS2812B
//Minimum brightness for setBrighntess() if using clock is 31


#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <FastLED.h>

#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>

#include <LEDMatrix.h>
#include <LEDText.h>
#include <FontMatrise.h>
//#include <AceButton.h>
//using namespace ace_button;
#define MATRIX_TYPE    HORIZONTAL_ZIGZAG_MATRIX

//const char *ssid     = "Toaster";
//const char *password = "Keinedickenelche";
const char *ssid     = "FRITZ!Box 7560 VI";
const char *password = "1972197419721974";
// Button const int BUTTON_PIN = D0;
char auth[] = "lfUE6aVmgTsOnICCSN5tWfEuvXraujeb";

String ntpTimeString;
unsigned int counter=0;
unsigned int countermax = 16000;
int modus = 1;
int sync_time = 1000;
uint16_t brightness=100;
int brightness_setter = 100;
  //nur damit man die Helligkeit remote steuern kann
  uint8_t color_hour;
  uint8_t color_min;
uint8_t hue; //für animation 1
//Texteinheiten welche man in die Textobjekte schreiben kann
unsigned char TxtDemoUp[] = {"00" EFFECT_FRAME_RATE "\x02"  };
unsigned char TxtDemoDown[] = {"00" EFFECT_FRAME_RATE "\x02"  };

//animation1
uint8_t farbwechselgeschwindigkeit = 5; //bei 2 bis 4 partymodus, 16 ist normal
uint8_t geschwindigkeit = 2;
uint8_t animation_modus = 0;

WiFiUDP ntpUDP;
//AceButton button(BUTTON_PIN);
cLEDMatrix<16, 16, MATRIX_TYPE> leds;
cLEDText ScrollingMsgUp;
cLEDText ScrollingMsgDown;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);





/* Button zeug
// The event handler for the button.
void handleEvent(AceButton* , uint8_t eventType,
    uint8_t buttonState) {

  // Print out a message for all events.
  Serial.print(F("handleEvent(): eventType: "));
  Serial.print(eventType);
  Serial.print(F("; buttonState: "));
  Serial.println(buttonState);

  // Control the LED only for the Pressed and Released events.
  // Notice that if the MCU is rebooted while the button is pressed down, no
  // event is triggered and the LED remains off.
  
  switch (eventType) {
    case AceButton::kEventPressed:
      //digitalWrite(LED_PIN, LED_ON);
      break;
    case AceButton::kEventReleased:
      //digitalWrite(LED_PIN, LED_OFF);
      break;
  }
}
*/

BLYNK_WRITE(V0)
{
  modus = param.asInt(); // assigning incoming value from pin V1 to a variable
  Serial.println("modus changed");
  FastLED.clear(true);
  // process received value
}


BLYNK_WRITE(V1)
{
  farbwechselgeschwindigkeit = param.asInt(); // assigning incoming value from pin V1 to a variable
}

BLYNK_WRITE(V2)
{
  geschwindigkeit = param.asInt(); // assigning incoming value from pin V1 to a variable
}

BLYNK_WRITE(V3)
{
  animation_modus = param.asInt(); // assigning incoming value from pin V1 to a variable
}

BLYNK_WRITE(V4)
{
  brightness_setter = param.asInt(); // assigning incoming value from pin V1 to a variable
}

void animation1(){
  int16_t x, y;
  uint8_t h;
  
  FastLED.clear();
  FastLED.setBrightness(brightness_setter);
  
  h = hue;
  if (counter < countermax/2)
  {
    // ** Fill LED's with diagonal stripes
    for (x=0; x<(leds.Width()+leds.Height()); ++x)
    {
      leds.DrawLine(x - leds.Height(), leds.Height() - 1, x, 0, CHSV(h, 255, 255));
      h+=farbwechselgeschwindigkeit;//änderung der geschwindigkeit des farbwechsels
    }
  }
  else
  {
    // ** Fill LED's with horizontal stripes
    for (y=0; y<leds.Height(); ++y)
    {
      leds.DrawLine(0, y, leds.Width() - 1, y, CHSV(h, 255, 255));
      h+=farbwechselgeschwindigkeit;//änderung der geschwindigkeit des farbwechsels
    }
  }
  //änderung der anfangsfarbe
  hue+=geschwindigkeit;
  if (counter < countermax/10)
    ;
  else if (counter < countermax*2/10)
    leds.HorizontalMirror();
  else if (counter < countermax*3/10)
    leds.VerticalMirror();
  else if (counter < countermax*4/10)
    leds.QuadrantMirror();
  else if (counter < countermax*5/10)
    leds.QuadrantRotateMirror();
  else if (counter < countermax*6/10)
    ;
  else if (counter < countermax*7/10)
    leds.TriangleTopMirror();
  else if (counter < countermax*8/10)
    leds.TriangleBottomMirror();
  else if (counter < countermax*9/10)
    leds.QuadrantTopTriangleMirror();
  else if (counter < countermax)
    leds.QuadrantBottomTriangleMirror();

  FastLED.show();
}

//ändert die Farbe der Matrix für die Uhrzeit
   
void editTimePrint(){
  ntpTimeString = timeClient.getFormattedTime();

  String houres_string = ntpTimeString.substring(0,2);
  String minutes_string = ntpTimeString.substring(3,5);

  uint8_t houres = houres_string.toInt();
  uint8_t minutes = minutes_string.toInt();

  color_min = map(minutes,0,60,1,255);
  color_hour = map(houres,0,24,1,255);
  //color_hour = houres*10+10;
  //color_min = minutes*4+10;
  
//Helligkeit an die Uhrzeit anpassen
    uint8_t adjust = 50; 

    if(houres < 10){
      brightness = 0;
      sync_time = 60000;
    }
    else{
      sync_time = 1000;
      brightness = 200;
      if(houres >=18){
        brightness = map(houres,18,23,180,40);
      }
    }
  
  if(brightness_setter >=0){
    brightness = brightness_setter;
  }
    
   ScrollingMsgDown.SetTextColrOptions(COLR_HSV | COLR_SINGLE, color_min, 255, brightness);
   ScrollingMsgUp.SetTextColrOptions(COLR_HSV | COLR_SINGLE, color_hour, 255, brightness);
}

//schreibt die zeit auf die matrix
void printTime(){
  
  //String mit Zeit
  ntpTimeString = timeClient.getFormattedTime();

  unsigned char hours[] = {0,0};
  unsigned char minutes[] = {0,0};
  //get minutes/houres u of string
  for(int i=0;i<=1;i++){
    hours[i] = ntpTimeString[i];
  }
  for(int i=3;i<=4;i++){
    minutes[i-3]=ntpTimeString[i];
  }
  
  //ausführen des schreiben des Textes
  ScrollingMsgUp.SetText((unsigned char *)hours, sizeof(TxtDemoUp) - 1);
  ScrollingMsgUp.UpdateText();

  ScrollingMsgDown.SetText((unsigned char *)minutes, sizeof(TxtDemoDown) - 1);
  ScrollingMsgDown.UpdateText();
  FastLED.show();
  
}





void setup(){
  Serial.begin(115200);
  delay(100);

/*BUTTON SETUP
 * pinMode(D0, INPUT);
  ButtonConfig* buttonConfig = button.getButtonConfig();
  buttonConfig->setEventHandler(handleEvent);
  buttonConfig->setFeature(ButtonConfig::kFeatureClick);
  */

//WIFI SETUP   
  if(1==1){ //Wifi needed
    Blynk.begin(auth, ssid, password);
    Serial.println ( "Blynk server connected" );
    
    //WiFi.begin(ssid, password);
    while ( WiFi.status() != WL_CONNECTED ) {
      delay ( 500 );
      Serial.print ( "Toaster.." );
    }
    Serial.print ( "Connection estabilisched            ->  :" );
    Serial.println ( WiFi.status() );

    
//time client setup
    timeClient.begin();
    timeClient.update();
    ntpTimeString = timeClient.getFormattedTime();
    
    
  }
//LED SETUP
  FastLED.addLeds<WS2812B, 5, GRB>(leds[0], leds.Size());
  FastLED.setBrightness(64);
  FastLED.clear(true);
  FastLED.show();

  ScrollingMsgUp.SetFont(MatriseFontData);
  ScrollingMsgUp.Init(&leds, leds.Width(), ScrollingMsgUp.FontHeight() + 1, 4, 8);
  ScrollingMsgUp.SetText((unsigned char *)TxtDemoUp, sizeof(TxtDemoUp) - 1);
  ScrollingMsgUp.SetTextColrOptions(COLR_RGB | COLR_SINGLE, 0xff, 0x00, 0xff);
  ScrollingMsgUp.SetFrameRate(4);

  ScrollingMsgDown.SetFont(MatriseFontData);                               //Breite,Höhe
  ScrollingMsgDown.Init(&leds, leds.Width(), ScrollingMsgDown.FontHeight() + 1, 4, 0);
  ScrollingMsgDown.SetText((unsigned char *)TxtDemoDown, sizeof(TxtDemoDown) - 1);
  ScrollingMsgDown.SetTextColrOptions(COLR_RGB | COLR_SINGLE, 0xff, 0x00, 0xff);
  ScrollingMsgDown.SetFrameRate(4);
  
}

void loop() {
  //Standby
  if(modus == 3){
    FastLED.clear(true);
    FastLED.show();
  }
  //Uhrzeit modus
  if(modus == 1){
    
    EVERY_N_MILLISECONDS(sync_time) { 
    timeClient.update();
    editTimePrint();
    printTime();
    }

    //animationsmodus
  }else if(modus == 2){
    EVERY_N_MILLISECONDS(500) { 
    }
    animation1();
    
  }

//matrixexample 1
//Spritetest
//colorpalette_me
  //Button ist gedrückt auf 0
  //button.check();
  Blynk.run();

  //animation_modus legt fest, ob der Modus von ausen gesteurt werden soll
  //bei 0 läuft er alle modi automatisch durch
  if(animation_modus == 0){
    counter++;
    //counter durchläuft die 16000(countermax) in 0,1ms, also wird eine counterzahl einmal pro 0,1ms aufgerufen
    if(counter>=countermax){
      counter=0;
    }
  }else{
      counter = countermax*animation_modus/10;
  }
}
