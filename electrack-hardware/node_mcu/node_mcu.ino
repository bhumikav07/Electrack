#include <ESP8266WiFi.h>
#include <WebSocketClient.h>

boolean handshakeFailed = 0;
char path[] = "/";  //identifier of this device
const char *ssid = "codeDwaraka";
const char *password = "helloworld";
char *host = "192.168.1.11";  //replace this ip address with the ip address of your Node.Js server
const int espport = 3000;

WebSocketClient webSocketClient;
unsigned long previousMillis = 0;
unsigned long currentMillis;
unsigned long interval = 300; //interval for sending data to the websocket server in ms
// Use WiFiClient class to create TCP connections
WiFiClient client;

const int Sensor_Pin = A0;
int Sensitivity = 66;
float Vpp = 0;  // peak-peak voltage 
float Vrms = 0; // rms voltage
float Irms = 0; // rms current
float Supply_Voltage = 230.0; // reading from DMM
float Vcc = 5.0;  // ADC reference voltage  // voltage at 5V pin 
float power = 0;  // power in watt              
float Wh = 0; // Energy in kWh
float Watt = 0;
unsigned long last_time = 0;
unsigned long current_time = 0;
unsigned int calibration = 100; // V2 slider calibrates this
unsigned int pF = 85; // Power Factor default 95
float bill_amount = 0;  // 30 day cost as present energy usage incl approx PF 
unsigned int energyTariff = 8.0;

void setup()
{
  Serial.begin(9600);
  pinMode(Sensor_Pin, INPUT); // Initialize the LED_BUILTIN pin as an output
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  delay(1000);

  wsconnect();

}

void loop()
{
  if (client.connected()) {
  webSocketClient.sendData(getACS712());  //send sensor data to websocket server
  }
  delay(5000);
}

float getVPP()
{
  float result;
  int readValue;
  int maxValue = 0;
  int minValue = 1024;
  uint32_t start_time = millis();
  while ((millis() - start_time) < 1000)  //read every 0.95 Sec
  {
    readValue = analogRead(Sensor_Pin);
    if (readValue > maxValue)
    {
      maxValue = readValue;
    }
    if (readValue < minValue)
    {
      minValue = readValue;
    }
  }
  result = ((maxValue - minValue) *Vcc) / 1024.0;
  return result;
}

String getACS712()
{
  // for AC
  Vpp = getVPP();
  Vrms = (Vpp / 2.0) *0.707;
  //Vrms = Vrms - (calibration / 10000.0);      // calibtrate to zero with slider
  Irms = (Vrms *1000) / Sensitivity;
  //Irms=Irms+4.6;
  if ((Irms > -0.015) && (Irms < 0.008))
  {
    // remove low end chatter
    Irms = 0.0;
  }
  //Irms=Irms+3.6;
  power = (Supply_Voltage *Irms) *(pF / 100.0);
  //power=power+881;
  last_time = current_time;
  current_time = millis();
  Wh = Wh + power *((current_time - last_time) / 3600000.0);  // calculating energy in Watt-Hour
  Watt = Wh / 1000;
  return (String(Supply_Voltage, 3) +":"+ String(Irms, 3) +":"+  String(power, 3) +":"+  String(Wh, 3));

}

void wsconnect()
{
  // Connect to the websocket server
  if (client.connect(host, espport))
  {
    Serial.println("Connected");
  }
  else
  {
    Serial.println("Connection failed.");
    delay(1000);

    if (handshakeFailed)
    {
      handshakeFailed = 0;
      ESP.restart();
    }
    handshakeFailed = 1;
  }
  // Handshake with the server
  webSocketClient.path = path;
  webSocketClient.host = host;
  if (webSocketClient.handshake(client))
  {
    Serial.println("Handshake successful");
  }
  else
  {

    Serial.println("Handshake failed.");
    delay(4000);

    if (handshakeFailed)
    {
      handshakeFailed = 0;
      ESP.restart();
    }
    handshakeFailed = 1;
  }
}
