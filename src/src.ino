/*********************************************************************************
 *  MIT License
 *  
 *  Copyright (c) 2020-2023 Gregg E. Berman
 *  
 *  https://github.com/HomeSpan/HomeSpan
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *  
 ********************************************************************************/

#include "HomeSpan.h"

struct LED_Service : Service::LightBulb {

  int ledPin;
  SpanCharacteristic *power;
  SpanCharacteristic *level;
  
  LED_Service(int ledPin) : Service::LightBulb(){
    power=new Characteristic::On();
    level=new Characteristic::Brightness2(20,true);
    this->ledPin=ledPin;
    pinMode(ledPin,OUTPUT);   
    level->setVal(level->getVal()+1); 
  }

  boolean update(){            
    digitalWrite(ledPin,power->getNewVal());   
    return(true);  
  }

};
      
//////////////////////////////////////

void setup() {
  
  Serial.begin(115200);

//  homeSpan.setHostNameSuffix("");

  homeSpan.setControlPin(21);
  
  homeSpan.setLogLevel(2);
//  homeSpan.reserveSocketConnections(12);
  
//  homeSpan.setApSSID("HS_Setup");
//  homeSpan.setApPassword("");

//homeSpan.setQRID("9Sam");

  homeSpan.enableWebLog(20);

//          .setStatusPin(13);
//  homeSpan.setSerialInputDisable(true);
//  homeSpan.enableOTA();


  homeSpan.begin(Category::Lighting,"HomeSpan LED");
  
  new SpanAccessory();   
    new Service::AccessoryInformation(); 
      new Characteristic::Identify();
    new LED_Service(13);  
}

//////////////////////////////////////

void loop(){ 
  homeSpan.poll();
}

//////////////////////////////////////
