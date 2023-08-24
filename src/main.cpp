#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_LPS2X.h>
#include <Adafruit_Sensor.h>  // Install library
#include <Notecard.h>

Adafruit_LPS22 g_lps22hb;
Notecard notecard;

#define PRODUCT_UID ""

float read_data();
void sending(float data);

//Simple function to configure barometer
void setup(void){

  Serial.println("Adafruit LPS22 test!");

  // Try to initialize!
  if (!g_lps22hb.begin_I2C(0x5c)) 
  {
    Serial.println("Failed to find LPS22 chip");
    while (1) 
    { 
      delay(10); 
    }
  }

  Serial.println("LPS22 Found!");

  g_lps22hb.setDataRate(LPS22_RATE_10_HZ);
  Serial.print("Data rate set to: ");

  switch (g_lps22hb.getDataRate()) 
  {
    case LPS22_RATE_ONE_SHOT: Serial.println("One Shot / Power Down"); 
      break;
    case LPS22_RATE_1_HZ: Serial.println("1 Hz"); 
      break;
    case LPS22_RATE_10_HZ: Serial.println("10 Hz"); 
      break;
    case LPS22_RATE_25_HZ: Serial.println("25 Hz"); 
      break;
    case LPS22_RATE_50_HZ: Serial.println("50 Hz"); 
      break;

  }
  Wire.begin();
  notecard.begin();
  
  J *req = notecard.newRequest("hub.set");
  if (PRODUCT_UID[0]) {
    JAddStringToObject(req, "product", PRODUCT_UID);
  }

  JAddStringToObject(req, "mode", "continuous");

  notecard.sendRequest(req);
  Serial.println("Notecard configured!");
}

void loop(){
  float press;
  press = read_data();
  Serial.println(press);
  sending(press);
  Serial.println("It worked, data sent!");
  delay(3*60*1000);
}


float read_data() {
  sensors_event_t pressure;
  sensors_event_t temp;
  g_lps22hb.getEvent(&pressure, &temp);
  return(pressure.pressure);
}

void sending(float data) {
  float dataa = data;

  J *req = notecard.newRequest("note.add");
  if (req != NULL) {
    JAddStringToObject(req, "file", "data.qo");
    Serial.println("Communication enabled");
    JAddBoolToObject(req, "sync", true);
    Serial.println("Sync Ready!");

    J *body = JCreateObject();
    if (body != NULL) {
      JAddNumberToObject(body, "temp", dataa);
      JAddItemToObject(req, "body", body);
    }
    notecard.sendRequest(req);
  }
}
