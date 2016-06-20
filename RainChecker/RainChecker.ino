#include <ArduinoJson.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#include "app_setting.h"

#include "LedControl.h"
LedControl lc = LedControl(15,12,13,1);


#define Serial Serial

ESP8266WiFiMulti WiFiMulti;

unsigned char err_icon[8]{
B10000001,
B01000010,
B00100100,
B00011000,
B00011000,
B00100100,
B01000010,
B10000001
};

int parse_weather_data(String payload);

void setup() {
  Serial.println("Set to STA mode");
  WiFi.mode(WIFI_STA);

  lc.shutdown(0,false);
  lc.setIntensity(0,0);
  lc.clearDisplay(0);
  
  Serial.begin(115200);
  
  Serial.println();
  Serial.println();
  Serial.println();
  
  WiFiMulti.addAP(ssid, password);

  int busy_count = 0;
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    busy_count++;
    lc.setRow(0,0,busy_count);
  }
}

void loop() {
    int rainfall_count = 0;
    // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;

        Serial.print("[HTTP] begin...\n");
        // configure traged server and url

        http.begin("http://weather.olp.yahooapis.jp/v1/place?coordinates=" + pos + "&output=json&appid=" + appid);        

        Serial.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);

            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();

                lc.shutdown(0,false);
                lc.clearDisplay(0);
                rainfall_count = parse_weather_data(payload);

                Serial.println(payload);
            }
        } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());

            for(int col=0;col<8;col++) {
              lc.setColumn(0,col,err_icon[col]);
            }
        }

        http.end();
    }

    //ウェイト処理
    if(rainfall_count > 0){
      int delay_time = 5 * 60 * 1000;// 5 min
      int delay_interval = 8;//LEDの数で8分割
      for(int i=0;i<delay_interval;i++){
        lc.setRow(0,7,B10000000 >> i);
        delay(delay_time / delay_interval);
      }
      //以下のforループは元の位置に戻るアニメーション用
      for(int i=0;i< (delay_interval-1) ;i++){
        lc.setRow(0,7,B1 << i);
        delay(50); 
      }
    }else{
/* 
 * Set the brightness of the display.
 * Params:
 * int addr      the address of the display to control
 * int intensity the brightness of the display. 
 *               Only values between 0(darkest) and 15(brightest) are valid.
*/
      lc.setIntensity(0,0);
      lc.shutdown(0,true);//一定時間の降水予測が0の場合はLEDモジュールを停止
      delay(30 * 60 * 1000);
    }
}

int parse_weather_data(String payload){
  const char *json = payload.c_str();
  
  StaticJsonBuffer<2048> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  
  int count = root["ResultInfo"]["Count"];
  Serial.print("count:");
  Serial.println(count);
  
  int jstatus = root["ResultInfo"]["Status"];
  Serial.print("Status:");
  Serial.println(jstatus);
  
  int rainfall_count = 0;
  int num = sizeof(root["Feature"][0]["Property"]["WeatherList"]["Weather"]);
  Serial.print("sizeof:");
  Serial.println(num);
  if(num > 7){
    num = 7;
  }
  for(int i=0;i<num;i++){
    double rainfall  = root["Feature"][0]["Property"]["WeatherList"]["Weather"][i]["Rainfall"];
    
    Serial.print("rainfall:");
    Serial.println(rainfall);
    if(rainfall > 0){
      rainfall_count++;
    }
    int rawdata = 0;
    if(rainfall > 0){
      if(rainfall < 1){
        rawdata = 1;
      }else{
        rawdata= (int)(log10(rainfall) * 4.0);
      }
    }
    Serial.print("rawdata:");
    Serial.println(rawdata);

    if(rawdata > 8){
      rawdata = 8;
    }
    int bitdata = 0;
    for(int j=0;j<rawdata;j++){
      bitdata += (1<<j);
    }
    lc.setRow(0,i,bitdata);
  
    
    const char* weather_type = root["Feature"][0]["Property"]["WeatherList"]["Weather"][i]["Type"];
    Serial.print("type:");
    Serial.println(weather_type);
    
    const char* weather_date = root["Feature"][0]["Property"]["WeatherList"]["Weather"][i]["Date"];
    Serial.print("Date:");
    Serial.println(weather_date);  
    Serial.println();  
  }
  
  Serial.print("rainfall_count:");
  Serial.println(rainfall_count);
  
  const char* fname = root["Feature"][0]["Name"];
  
  Serial.print("name:");
  Serial.println(fname);
  
  //.Feature[0].Property.WeatherList.Weather[i].Type
  return rainfall_count;
}

