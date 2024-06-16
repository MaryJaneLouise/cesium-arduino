#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <SoftwareSerial.h>

const int ROW_NUM = 4;
const int COLUMN_NUM = 4;

const int RELAY = 49; 
const int LED_RED = 51;
const int LED_GREEN = 53;

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};

byte pin_rows[ROW_NUM] = {11, 10, 9, 8};
byte pin_column[COLUMN_NUM] = {7, 6, 5, 4};

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

LiquidCrystal_I2C lcd(0x27, 16, 2);

String ID = "";
String OTP = "";
String correctOTP = "808080";
String masterOTP = "888888";

const int INPUT_ID = 1;
const int INPUT_OTP = 2;
const int AUTHENTICATING = 3;
int step = 1; 

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);

  pinMode(RELAY, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_RED, HIGH);
  digitalWrite(RELAY, HIGH);

  lcd.begin();
  lcd.print("Welcome to AUF!");
  delay(2000);

  lcd.clear();
  lcd.print("Enter ID:");
}

void loop() {
  char key = keypad.getKey();

  if (step == INPUT_ID) {
    if (key) {
      if (key == 'C') {
        ID = "";
        lcd.clear();
        lcd.print("Enter ID:");
      } 
      else if (key == '*') {
        step = INPUT_OTP;
        lcd.clear();
        lcd.print("Enter OTP:");
      } 
      else if (key != 'A' && key != 'B' && key != 'D' && key != '#') {
        ID += key;
        lcd.setCursor(0, 1);
        lcd.print(ID);
      }
    }
  } else if (step == INPUT_OTP) {
      if (key) {
      if (key == 'C') {
        OTP = "";
        lcd.clear();
        lcd.print("Enter OTP:");
      } 
      else if (key == '*') {
        sendOTPToESP8266();
        lcd.clear();
        lcd.print("Authenticating...");
        step = AUTHENTICATING;
      } 
      else if (key != 'A' && key != 'B' && key != 'D' && key != '#') {
        OTP += key;
        lcd.setCursor(0, 1);
        lcd.print(OTP);
      }
    }
  } else {
    checkResponse();
  }
}

void sendOTPToESP8266() {
  Serial.println("*" + ID + "/" + OTP);
}

void checkResponse() {
  if (Serial.available()) {
    String message = Serial.readString();
    if (message[0] == '|') {
      int endIndex = message.indexOf('\n');
      String response = message.substring(1, endIndex);

      Serial.println(response);
      
      // IDK how to check strings for equality
      // So let's just check the first letter :)
      if (response[0] == 'A') {
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_GREEN, HIGH);

        lcd.clear();
        lcd.print("Access granted!");
        digitalWrite(RELAY, LOW);
        delay(2000);
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(RELAY, HIGH);
      } else {
        lcd.clear();
        lcd.print("Access denied!");
        delay(2000);
      }
      lcd.clear();
      lcd.print("Enter ID:");
      ID = "";
      OTP = "";
      step = INPUT_ID;
    }
  }
}
