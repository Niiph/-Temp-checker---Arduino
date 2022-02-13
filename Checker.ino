#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <string.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define MAX_STRING_LEN 60
String sensorrequest = "http://domain.com/sensorlist.php?r=sensor&s=";
String senddata = "http://domain.com/sensorlist.php?r=data&s=";

const char *ssid = "";                 // SSID
const char *password = ""; // password
const char *record;
char *ignore = "/ ,";
char *p, *i;
int address[8] = {};
char *add0;
int addr;
String req;
int pin, senno;
int pin1 = 5;
int pin2 = 11;

int przerwa = 1800000; // time between readings

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  // Serial.begin(9600);
  // sensors.begin();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    // Serial.println("Connecting..");
  }
  digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
  int nct = numbercheck();
  // Serial.print("Ilość czujników: ");
  // Serial.println(nct);
  // int ct = 2;
  for (int ct = 1; ct <= nct; ct++)
  {
    req = sensorrequest + ct;
    // Serial.println(req);
    // Serial.print("Odczyt numer: ");
    // Serial.println(ct);
    connection(req);
    delay(500);
  }
  // Serial.print("Przerwa w działaniu");
  delay(przerwa); // Send a request every 30 seconds
}

int numbercheck()
{
  if (WiFi.status() == WL_CONNECTED)
  { // Check WiFi connection status

    HTTPClient http; // Declare an object of class HTTPClient

    http.begin("http://temp.ilovegeckos.com/sensorlist.php?r=number"); // Specify request destination
    int httpCode = http.GET();                                         // Send the request

    if (httpCode > 0)
    {                                    // Check the returning code
      String payload = http.getString(); // Get the request response payload
      // Serial.println(payload);                     //Print the response payload
      return payload.toInt();
    }
    http.end(); // Close connection
  }
}

void connection(String reqaddress)
{
  if (WiFi.status() == WL_CONNECTED)
  { // Check WiFi connection status

    HTTPClient http; // Declare an object of class HTTPClient

    http.begin(reqaddress);    // Specify request destination
    int httpCode = http.GET(); // Send the request

    if (httpCode > 0)
    { // Check the returning code

      String payload = http.getString(); // Get the request response payload
      // Serial.println(payload);                     //Print the response payload
      // record = payload;
      record = const_cast<char *>(payload.c_str());
    }

    http.end(); // Close connection
  }

  add0 = subStr(record, ignore, 1);
  addr = strtol(add0, NULL, 10);
  senno = addr;
  // Serial.print("Numer czujnika: ");
  // Serial.print(senno);
  add0 = subStr(record, ignore, 2);
  addr = strtol(add0, NULL, 10);
  pin = addr;
  // Serial.print(", numer pinu: ");
  // Serial.print(pin);

  for (int x = 3; x <= 10; x++)
  {
    add0 = subStr(record, ignore, x);
    addr = strtol(add0, NULL, 16);
    address[x - 2] = addr;
  }
  // Serial.print(", adres czujnika: {");
  // for(int y = 1; y <= 8; y++ ) {
  // Serial.print(address[y], HEX);
  // Serial.print(", ");
  // }
  // Serial.println(" }");
  OneWire ds(pin);
  // ds.reset();
  DallasTemperature sensors(&ds);
  sensors.begin();
  // sensors.requestTemperatures();

  uint8_t hexaddress[8] = {address[1], address[2], address[3], address[4], address[5], address[6], address[7], address[8]};

  sensors.setResolution(11);
  sensors.requestTemperaturesByAddress(hexaddress);
  float tempread = sensors.getTempC(hexaddress);
  // Serial.println(tempread);

  String sendddress = senddata + senno + "&v=" + tempread;
  HTTPClient http;
  http.begin(sendddress); // Specify request destination
  int httpCode = http.GET();
}

// Function to return a substring defined by a delimiter at an index
char *subStr(const char *str, char *delim, int index)
{
  char *act, *sub, *ptr;
  static char copy[MAX_STRING_LEN];
  int i;

  // Since strtok consumes the first arg, make a copy
  strcpy(copy, str);

  for (i = 1, act = copy; i <= index; i++, act = NULL)
  {
    // Serial.print(".");
    sub = strtok_r(act, delim, &ptr);
    if (sub == NULL)
      break;
  }
  return sub;
}
