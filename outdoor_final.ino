#include <LiquidCrystal_I2C.h>
#include <dht.h> // библиотека датчика температуры и влажности

LiquidCrystal_I2C lcd(0x27,20,4); 

dht DHT_outdoor;  // Создает объект DHT для улицы
dht DHT_cellar;  // Создает объект DHT для погреба


// структура данных для контроллера, будет передаваться в сеть
typedef struct
{
  float t_outdoor;
  float h_outdoor;
  float t_cellar;
  float h_cellar;
}  outdoor_cellar;

String from_serial = "No respons";

void display_data(outdoor_cellar data)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("t.o:");
  lcd.setCursor(4,0);
  lcd.print(data.t_outdoor);
  lcd.setCursor(10,0);
  lcd.print("h.o:");
  lcd.setCursor(14,0);
  lcd.print(data.h_outdoor);
  lcd.setCursor(0,1);
  lcd.print("t.c:");
  lcd.setCursor(4,1);
  lcd.print(data.t_cellar);
  lcd.setCursor(10,1);
  lcd.print("h.c:");
  lcd.setCursor(14,1);
  lcd.print(data.h_cellar);
}

void display_serial(String from_serial)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(from_serial);
}

// функция получения данных с датчиков, возвращает структуру outdoor_cellar_data
outdoor_cellar get_data()
{
  outdoor_cellar data;
  int read_DHT_outdoor = DHT_outdoor.read22(7);  // привязываем датчик улицы к пину D_7
  int read_DHT_cellar = DHT_cellar.read22(8);  // привязываем датчик погреба к пину D_8

  // получение температуры и влажности улицы
  if(read_DHT_outdoor == DHTLIB_OK)
  {
    data.t_outdoor = DHT_outdoor.temperature;
    data.h_outdoor = DHT_outdoor.humidity;
  }
  else
  {
    data.t_outdoor = -88;
    data.h_outdoor = -88;    
  }
  // получение температуры и влажности погреба
  if(read_DHT_cellar == DHTLIB_OK)
  {
    data.t_cellar = DHT_cellar.temperature;
    data.h_cellar = DHT_cellar.humidity;
  }
  else
  {
    data.t_cellar = -88;
    data.h_cellar = -88;    
  }
  return data;
}

void setup()
{
  lcd.init(); 
  lcd.noBacklight();
  Serial.begin(9600); //Скорость обмена данными ESP <-> arduino
  Serial.println("AT+CIPMUX=0"); // single mode connection
}

void loop()
{
  outdoor_cellar data = get_data();  
  display_data(data);
  String message = "outdoor "+String(data.t_outdoor)+" "+String(data.h_outdoor)+" "+String(data.t_cellar)+" "+String(data.h_cellar);
  int message_len = message.length();
  delay(4000); // задержка для отображения физических параметров на дисплее

  Serial.println("AT+CIPSTART=\"UDP\",\"192.168.0.200\",8080"); // Create a UDP transmission
  delay(1000);  

  if (Serial.available() > 0) { //Если пришли данные с ESP
    from_serial = Serial.readString();
    from_serial.trim();   
  }
  display_serial(from_serial);
  delay(1000);

  Serial.println("AT+CIPSEND="+String(message_len)); // указание размера посылки
  delay(1000); 
  if (Serial.available() > 0) { //Если пришли данные с ESP
    from_serial = Serial.readString();
    from_serial.trim();   
  }
  display_serial(from_serial);
  delay(1000); 

  Serial.print(message); // здесь написать ввод данных
  delay(1000);
  if (Serial.available() > 0) { //Если пришли данные с ESP
    from_serial = Serial.readString();
    from_serial.trim();   
  }
  display_serial(from_serial);
  delay(1000);  
}