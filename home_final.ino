#include <ESP8266WiFi.h> // библиотека управления wi-fi и ip
#include <WiFiUdp.h> // библиотека управления udp
#include <DHT.h> // библиотека датчика температуры и влажности

char wifi_ssid[]="Selenium";
char wifi_pass[]="A123456++aBeeline";

IPAddress local_IP(192,168,0,202);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);

char remote_IP[]="192.168.0.200";
int remote_Port = 8080;
int local_Port = 8080;

WiFiUDP wifi_udp;

#define DHTPIN 2     // Номер пина, который подключен к DHT22
#define DHTTYPE DHT22   // Указываем, какой тип датчика мы используем
DHT dht_home(DHTPIN, DHTTYPE); // Создает объект DHT для дома

void setup() {
  dht_home.begin();
  WiFi.begin(wifi_ssid, wifi_pass);
  WiFi.config(local_IP, gateway, subnet);
}

void loop() {
  float t_home = dht_home.readTemperature(); // Температура
  float h_home = dht_home.readHumidity(); // Влажность
  if (isnan(t_home))
  {
    t_home = -88;
  }
  if (isnan(h_home))
  {
    h_home = -88;
  }

  String message_str = "home "+String(t_home)+" "+String(h_home);
  int message_len = message_str.length()+1; 
  char message_char[message_len];
  message_str.toCharArray(message_char, message_len);
  
  wifi_udp.beginPacket(remote_IP,remote_Port);
  wifi_udp.write(message_char);
  wifi_udp.endPacket();

  delay(8500);
}
