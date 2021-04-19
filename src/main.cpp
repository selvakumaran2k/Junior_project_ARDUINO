#include <Arduino.h>
//3 parts for this project
//1 sensor
//2 esp8266
//3 web think speak
//#1 variables
int v1 = A0;
int v2 = A1;
int v3 = A2;
int button_status = 8;//this pin for button 
//ac voltage sensor variables
double sensorValue1 = 0;
double sensorValue2 = 0;
int crosscount = 0;
int climb_flag = 0;
int val[100];
int max_v = 0;
double VmaxD = 0;
double VeffD = 0;
double Veff = 0;
//initialising function
float read(int pin);
//variables for esp8266
#include <SoftwareSerial.h>       //Software Serial library
SoftwareSerial espSerial(2, 3);   //2 --rx 3 --tx of ardduino
#define DEBUG true
String mySSID = "Wifi name for network";       // WiFi SSID
String myPWD = "PASSWORD"; // WiFi Password
String myAPI = "E9L01H1IHE64T1OX";   // API Key
String myHOST = "api.thingspeak.com";
String myPORT = "80";
int sendVal;
//
String espData(String command, const int timeout, boolean debug);
void upload(String myFIELD,float sendVal);
/////
void setup(){
  Serial.begin(115200);
  pinMode(v1, INPUT);
    pinMode(v2, INPUT);
  pinMode(v3, INPUT);
  pinMode(button_status, INPUT_PULLUP);
  delay(100);//
  //wifi setup
  Serial.begin(9600);
  espSerial.begin(115200);
  
  espData("AT+RST", 1000, DEBUG);                      
  espData("AT+CWMODE=1", 1000, DEBUG);           
  espData("AT+CWJAP=\""+ mySSID +"\",\""+ myPWD +"\"", 1000, DEBUG);   
 
  delay(1000);
}
void loop()
{
  float Voltage1=read(v1);//voltage 1
  float Voltage2=read(v2);//voltage 2
  float Voltage3=analogRead(v3);//voltage 3
  bool Button=digitalRead(button_status); //button status
  Serial.println("***************************");
    delay(1500);
    upload("Vol1", Voltage1);
    upload("Vol2", Voltage2);
    upload("Vol3", Voltage3);
    upload("But1", Button==false?1.0:0);
}

float read(int pin) {

  for ( int i = 0; i < 100; i++ ) {
    sensorValue1 = analogRead(pin);
    if (analogRead(A0) > 511) {
      val[i] = sensorValue1;
    }
    else {
      val[i] = 0;
    }
    delay(1);
  }

  max_v = 0;

  for ( int i = 0; i < 100; i++ )
  {
    if ( val[i] > max_v )
    {
      max_v = val[i];
    }
    val[i] = 0;
  }
  if (max_v != 0) {


    VmaxD = max_v;
    VeffD = VmaxD / sqrt(2);
    Veff = (((VeffD - 420.76) / -90.24) * -210.2) + 210.2;
  }
  else {
    Veff = 0;
  }  
  VmaxD = 0;

  delay(100);
  return Veff;
}
void upload(String myFIELD,float sendVal)
  {
    String sendData = "GET /update?api_key="+ myAPI +"&"+ myFIELD +"="+String(sendVal);
    espData("AT+CIPMUX=1", 1000, DEBUG);   
    espData("AT+CIPSTART=0,\"TCP\",\""+ myHOST +"\","+ myPORT, 1000, DEBUG);
    espData("AT+CIPSEND=0," +String(sendData.length()+4),1000,DEBUG);  
    espSerial.find(">"); 
    espSerial.println(sendData);
    Serial.print("Value to be sent: ");
    Serial.println(sendVal);
    espData("AT+CIPCLOSE=0",1000,DEBUG);
    delay(10000);
  }
 String espData(String command, const int timeout, boolean debug)
{
  Serial.print("AT Command ==> ");
  Serial.print(command);
  Serial.println("     ");
  String response = "";
  espSerial.println(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (espSerial.available())
    {
      char c = espSerial.read();
      response += c;
    }
  }
  if (debug)
  {
  }
  return response;
}
  