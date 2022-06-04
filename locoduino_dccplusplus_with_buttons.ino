#include "DCCpp.h"
#include "Commanders.h"

#include "Loco.hpp"

#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin  
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Loco locos[4];
int locoIndex;
int locoNumber;

ButtonsCommanderEncoder EncoderRelative;

ButtonsCommanderPush Push_Encoder;
ButtonsCommanderPush Push_TrackPower;
ButtonsCommanderPush Push_PanicStop;
ButtonsCommanderPush Push_CycleLoco;
ButtonsCommanderPush Push_ToggleAuto;

void update_screen_time();

void setup()
{
  Serial.begin(115200);
 
  DCCpp::begin();
  
  DCCpp::beginMainPololu();
  DCCpp::beginProgPololu();

  // Init display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(2);                
  display.setTextColor(SSD1306_WHITE);   // Draw white text

  // Init buttons
  Commanders::begin(ReceiveEvent, LED_BUILTIN);
  EncoderRelative.begin(30, 52, 53);
  Push_Encoder.begin(51, 51);
  Push_TrackPower.begin(100, 46);
  Push_PanicStop.begin(105, 47);
  Push_CycleLoco.begin(110, 44);
  Push_ToggleAuto.begin(115, 45);

  // Loco
  locoIndex = 0;
  locoNumber = 4;
  locos[0] = Loco(47);
  locos[1] = Loco(7);
  locos[2] = Loco(56);
  locos[3] = Loco(95);
  
  DCCpp::powerOn();
  
  update_screen_time();
}

void ReceiveEvent(unsigned long inId, COMMANDERS_EVENT_TYPE inEventType, int inEventData)
{
  Serial.println("Received event: " + String(inId) + " " + String(inEventType) + " " + String(inEventData));

  // Power on/off
  if (inId == 100) {
    if (DCCpp::IsPowerOnMain || DCCpp::IsPowerOnProg) 
      DCCpp::powerOff();
    else
      DCCpp::powerOn();

    update_screen_time();
    return;
  }

  // Panic stop
  if (inId == 105) {
    for (int index = 0; index < locoNumber; index++) {
      locos[index].stop();
    }
    update_screen_time();
    return;
  }

  // Cycle loco
  if (inId == 110) {
    locoIndex +=1;
    locoIndex = locoIndex % locoNumber;
    update_screen_time();
    return;
  }
  
  // Toggle auto
  if (inId == 115) {
    return;
  }
  
  if (inId == 30) {
    // Encoder turned
    
    if (inEventData > 0) {
      // Knob turned CW
      
      if (locos[locoIndex].locoDirectionForward) {
        // Loco moving forward : accelerate
        if (locos[locoIndex].locoStepsNumber >= 100)
          locos[locoIndex].locoSpeed += 10;
        else
          locos[locoIndex].locoSpeed++;
        if (locos[locoIndex].locoSpeed > locos[locoIndex].locoStepsNumber - 1)
          locos[locoIndex].locoSpeed = locos[locoIndex].locoStepsNumber - 1;
      } else {
        // Loco moving backwards : decelerate
        if (locos[locoIndex].locoStepsNumber >= 100) {
          locos[locoIndex].locoSpeed -= 10;
        } else {
          locos[locoIndex].locoSpeed--;
        }
        // Handle change of direction
        if (locos[locoIndex].locoSpeed < 0) {
          locos[locoIndex].locoSpeed = -locos[locoIndex].locoSpeed;
          locos[locoIndex].locoDirectionForward = true;
        }
      }   
    } else if (inEventData < 0) {
      // Knob turned CCW

      if (!locos[locoIndex].locoDirectionForward) {
        // Loco moving backward : accelerate
        if (locos[locoIndex].locoStepsNumber >= 100)
          locos[locoIndex].locoSpeed += 10;
        else
          locos[locoIndex].locoSpeed++;
        if (locos[locoIndex].locoSpeed > locos[locoIndex].locoStepsNumber - 1)
          locos[locoIndex].locoSpeed = locos[locoIndex].locoStepsNumber - 1;
      } else {
        // Loco moving forward : decelerate
        if (locos[locoIndex].locoStepsNumber >= 100) {
          locos[locoIndex].locoSpeed -= 10;
        } else {
          locos[locoIndex].locoSpeed--;
        }
        // Handle change of direction
        if (locos[locoIndex].locoSpeed < 0) {
          locos[locoIndex].locoSpeed = -locos[locoIndex].locoSpeed;
          locos[locoIndex].locoDirectionForward = false;
        }
      }  
    }    
  } else if (inId == 51) {
    // Encoder button pressed
    
    // Stop the loco without changing direction
    locos[locoIndex].locoSpeed = 0;
  }
  
  // Finally send command and update
  locos[locoIndex].send();
  update_screen_time();
}

void loop()
{
  DCCpp::loop();
  Commanders::loop();
}
