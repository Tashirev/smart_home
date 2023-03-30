#include <LiquidCrystal_I2C.h> // библиотека дисплея
#include <Adafruit_BMP280.h> // библиотека датчика давления
#define PIN_RELAY 5 // Определяем пин, используемый для подключения реле

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

Adafruit_BMP280 bmp280; // Создает объект для датчика давления

String from_serial = "No respons";
int boiler_status;

// структура данных для контроллера
typedef struct
{ 
  float t_home;
  float h_home;
  float t_outdoor;
  float h_outdoor;
  float t_cellar;
  float h_cellar;
  float pressure;
  float t_on;
  float t_off;
}  Data;
Data data;

Data get_data(Data data)
{ 
  int i,j=0; // i - для обрезки строк, место пробела, j - для определения продолжительности сбора данных

  data.t_home = -99;
  data.h_home = -99;
  data.t_outdoor = -99;
  data.h_outdoor = -99;
  data.t_cellar = -99;
  data.h_cellar = -99;

  Serial.println("AT+CIPSTART=0,\"UDP\",\"0.0.0.0\",8080,8080,0"); // Create a UDP connection id. 0
  while(j<400)  // поставить условие времени 10 секунд
  {  
    if (Serial.available() > 0) //Если пришли данные с ESP
    { 
      from_serial = Serial.readString();
      from_serial.trim();
      from_serial = from_serial.substring(10);
    }
    String string_name = from_serial;
    string_name.remove(4);  

    if(string_name == "home")
    {
      from_serial = from_serial.substring(5);
      i = from_serial.indexOf(' '); // место положения следующего пробела
      data.t_home = from_serial.substring(0,i).toFloat();
      data.h_home = from_serial.substring(i).toFloat();
      
      //display_serial("home word_"+String(data.t_home)+"_"+String(data.h_home));
    }
  
    if(string_name == "outd")
    {
      from_serial = from_serial.substring(8);
      i = from_serial.indexOf(' '); // место положения следующего пробела
      data.t_outdoor = from_serial.substring(0,i).toFloat();
      from_serial = from_serial.substring(i+1);
      i = from_serial.indexOf(' '); // место положения следующего пробела
      data.h_outdoor = from_serial.substring(0,i).toFloat();
      from_serial = from_serial.substring(i+1);
      i = from_serial.indexOf(' '); // место положения следующего пробела
      data.t_cellar = from_serial.substring(0,i).toFloat();
      data.h_cellar = from_serial.substring(i).toFloat();
      //display_serial("outd word_"+String(data.t_outdoor)+"_"+String(data.h_outdoor)+"_"+String(data.t_cellar)+"_"+String(data.h_cellar));
    }

    if(string_name == "boil")
    {
      from_serial = from_serial.substring(7);
      i = from_serial.indexOf(' '); // место положения следующего пробела
      data.t_on = from_serial.substring(0,i).toFloat();
      data.t_off = from_serial.substring(i).toFloat();
      //display_serial("boil word_"+String(data.t_on)+"_"+String(data.t_off));
    }
    j +=1;
    delay(50);
  }

  Serial.println("AT+CIPCLOSE=0"); // Close connection id. 0


  // получение атмосферного давления, для перевода в мм рт. ст. делится на 133.322
  data.pressure = bmp280.readPressure()/(133.322);
  if(isnan(data.pressure))
  {
    data.pressure = -88;
  }

  return data;
}

void display_value(float value)
{
  if(value != -99 && value != -88)
  {
    lcd.print(value);  
  }
  else
  {
    if(value == -99)
    {
      lcd.print("NoCon");  
    }
    if(value == -88)
    {
      lcd.print("NoSen");  
    }
  }
}

// image display
void display(Data data, int status)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Home");
  lcd.setCursor(5,0);
  lcd.print("T:");
  lcd.setCursor(7,0);
  display_value(data.t_home);
  lcd.setCursor(13,0);
  lcd.print("H:");
  lcd.setCursor(15,0);
  display_value(data.h_home);

  lcd.setCursor(0,1);
  lcd.print("Out");
  lcd.setCursor(5,1);
  lcd.print("T:");
  lcd.setCursor(7,1);
  display_value(data.t_outdoor);
  lcd.setCursor(13,1);
  lcd.print("H:");
  lcd.setCursor(15,1);
  display_value(data.h_outdoor);
  
  lcd.setCursor(0,2);
  lcd.print("Undr");
  lcd.setCursor(5,2);
  lcd.print("T:");
  lcd.setCursor(7,2);
  display_value(data.t_cellar);
  lcd.setCursor(13,2);
  lcd.print("H:");
  lcd.setCursor(15,2);
  display_value(data.h_cellar);

  lcd.setCursor(0,3);
  lcd.print("Boil:");
  lcd.setCursor(5,3);
  lcd.print(status);
  lcd.setCursor(7,3);
  lcd.print(data.t_on);  
  lcd.setCursor(12,3);
  lcd.print("<T<");
  lcd.setCursor(15,3);
  lcd.print(data.t_off);
}

void display_serial(String from_serial)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(from_serial);
}

void setup()
{
  delay(5000);
  Serial.begin(9600); //Скорость обмена данными ESP <-> arduino
  Serial.println("AT+CIPMUX=1"); // Enable multiple connections
  delay(500);
  if (Serial.available() > 0) { //Если пришли данные с ESP
    from_serial = Serial.readString();
    from_serial.trim();   
  }
  display_serial(from_serial);
  Serial.println("AT+CIPSTART=4,\"UDP\",\"192.168.0.175\",8080"); // Create a UDP transmission
  delay(500);
  if (Serial.available() > 0) { //Если пришли данные с ESP
    from_serial = Serial.readString();
    from_serial.trim();   
  }
  display_serial(from_serial);
  delay(3000);


  pinMode(PIN_RELAY, OUTPUT);   // initialize pin relay
  digitalWrite(PIN_RELAY, LOW);    // reley OFF

  boiler_status = 0;
  data.t_on = 25;
  data.t_off = 26;

  bmp280.begin();

  lcd.init();   // initialize the lcd 
  // Print a message to the LCD.
  lcd.noBacklight();
  //display(data, boiler_status);
}


void loop()
{
  
  // get data
  data = get_data(data);

  // work boiler
  if (data.t_home < data.t_on && boiler_status == 0) {
    boiler_status = 1;
    digitalWrite(PIN_RELAY, HIGH);
  }
  if (data.t_home > data.t_off && boiler_status == 1) {
    boiler_status = 0;
    digitalWrite(PIN_RELAY, LOW);
  }

  // image display
  display(data, boiler_status);
  String message = "boiler_status "+String(data.t_home)+" "+String(data.h_home)+" "+String(data.t_outdoor)+" "+String(data.h_outdoor)+" "+String(data.t_cellar)+" "+String(data.h_cellar)+" "+String(data.pressure)+" "+String(data.t_on)+" "+String(data.t_off)+" "+String(boiler_status);
  int message_len = message.length();  

  Serial.println("AT+CIPSEND=4,"+String(message_len)); // указание размера посылки
  delay(500);
//  if (Serial.available() > 0) { //Если пришли данные с ESP
//    from_serial = Serial.readString();
//    from_serial.trim();   
//  }
  //display_serial(from_serial);

  Serial.print(message); // здесь написать ввод данных
  delay(500);
//  if (Serial.available() > 0) { //Если пришли данные с ESP
//    from_serial = Serial.readString();
//    from_serial.trim();   
//  }
  //display_serial(from_serial);
  
}