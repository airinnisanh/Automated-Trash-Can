#include <Servo.h> 
#include <SoftwareSerial.h>
#include <MFRC522.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

SoftwareSerial espSerial(10,11);
MFRC522 mfrc522(53,10);
LiquidCrystal_I2C lcd(0x27, 16, 2);
String rfid;

//Defining the PIN used for ultrasonic sensor
const int trigPinLogam = 22;
const int echoPinLogam = 23;
const int trigPinPemantauan = 24;
const int echoPinPemantauan = 25;
const int trigPinPlastik = 26;
const int echoPinPlastik = 27;

//Defining the PIN used for servo motor
int ServoTutupPinLogam = 5;
int ServoSekatAtasPinLogam = 3;
int ServoSekatBawahPinLogam = 4;
int ServoTutupPinPlastik = 6;
int ServoSekatAtasPinPlastik = 7;
int ServoSekatBawahPinPlastik = 8;

//Signing a variable for LDR sensor
unsigned int AnalogValue;
unsigned int AnalogBaseValue; 
unsigned int AnalogValuePlastik;

int ket = 19;
int ber = 100;

bool yes = false;
    
//Making new objects for servo motor
Servo ServoTutupLogam;
Servo ServoSekatAtasLogam;
Servo ServoSekatBawahLogam;
Servo ServoTutupPlastik;
Servo ServoSekatAtasPlastik;
Servo ServoSekatBawahPlastik;

void setup() {
    // setting servo 
    ServoTutupLogam.attach(ServoTutupPinLogam);
    ServoSekatAtasLogam.attach(ServoSekatAtasPinLogam);
    ServoSekatBawahLogam.attach(ServoSekatBawahPinLogam);
    ServoTutupPlastik.attach(ServoTutupPinPlastik);
    ServoSekatAtasPlastik.attach(ServoSekatAtasPinPlastik);
    ServoSekatBawahPlastik.attach(ServoSekatBawahPinPlastik);

    // setting ultrasonic
    pinMode(trigPinLogam, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPinLogam, INPUT); // Sets the echoPin as an Input
    pinMode(trigPinPlastik, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPinPlastik, INPUT); // Sets the echoPin as an Input
    pinMode(trigPinPemantauan, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPinPemantauan, INPUT); // Sets the echoPin as an Input

    Serial.begin(115200);
    SPI.begin();
    mfrc522.PCD_Init();
    espSerial.begin(115200);

    //Making sure the lids are all closed when the system starts
    tutupTutupLogam();
    tutupSekatAtasLogam();
    tutupSekatBawahLogam();
    tutupTutupPlastik();
    tutupSekatAtasPlastik();
    tutupSekatBawahPlastik();
    
}

void loop() {
    //the inductive sensor is starting to read the value
    AnalogBaseValue = analogRead(A1);
    
   //Reading RFID user ID card
    cek_rfid();
    if (rfid == "2ECE1951") {
      lcd.print("Welcome Airin");
      yes = true;
    }
     
    //Opens the lid when system detects there's someone approaches the trash can
    if (adaOrangLogam()) {
      bukaTutupLogam();
      delay(2000);      
      tutupTutupLogam();

      delay(3000);
      AnalogValue = analogRead(A0);

      //Condition where the inductive sensor detects a metal thing arrived, the trash will be continued to the metal trash can, the user will receive reward
      if (AnalogValue < 250) {
        bukaSekatAtasLogam();
        delay(3000);
        tutupSekatAtasLogam(); 
        ket = ket-2;
        ber = ber+10;
        if (yes) {
          lcd.clear();
          lcd.print("Poin kamu bertambah 5");
          yes = false;
        }
      } else { // Otherwise condition where a non-metal thing arrived, the trash will be pushed into the middle trash can
        tengahSekatAtasLogam();
        delay(1000);
        bukaSekatBawahLogam();
        tutupSekatAtasLogam();
        delay(4000);
        tutupSekatBawahLogam();
      }
      //Sends the data when the process already finished
      kirimData();
    }

    //Opens the lid when system detects there's someone approaches the trash can
    if (adaOrangPlastik()) {
      bukaTutupPlastik();
      delay(2000);      
      tutupTutupPlastik();

      delay(500);
      AnalogValuePlastik = analogRead(A1);
      int x = 50;

      //Condition where the inductive sensor detects a plastic thing arrived, the trash will be continued to the plastic trash can
      if ((AnalogValuePlastik >= (AnalogBaseValue-x)) && (AnalogValuePlastik <= (AnalogBaseValue+x))) {
        bukaSekatAtasPlastik();
        delay(3000);
        tutupSekatAtasPlastik(); 
      } else { // Otherwise condition where a non-metal thing arrived, the trash will be pushed into the middle trash can
        tengahSekatAtasPlastik();
        delay(1000);
        bukaSekatBawahPlastik();
        tutupSekatAtasPlastik();
        delay(4000);
        tutupSekatBawahPlastik();
      }
    }
    
    delay(1000);
    
}

//A function made to scan the trash height to decide whether the trash can is already full
String bacatinggiSampah() {
  long duration;
  int distance;
  
  digitalWrite(trigPinPemantauan, LOW);
  delayMicroseconds(2);
      
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPinPemantauan, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinPemantauan, LOW);
    
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPinPemantauan, HIGH);
    
  // Calculating the distance
  distance= duration*0.034/2;
  return String(distance);
}

//A function made to decide whether someone approaches the metal trash can
bool adaOrangLogam() {
  long duration;
  int distance;
  int batasAdaOrang = 10;
  
  digitalWrite(trigPinLogam, LOW);
  delayMicroseconds(2);
      
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPinLogam, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinLogam, LOW);
    
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPinLogam, HIGH);
    
  // Calculating the distance
  distance= duration*0.034/2;

  if (distance < batasAdaOrang) {
    return true;
  } else {
    return false;
  }
}

//A function made to decide whether someone approaches the plastic trash can
bool adaOrangPlastik() {
  long duration;
  int distance;
  int batasAdaOrang = 10;
  
  digitalWrite(trigPinPlastik, LOW);
  delayMicroseconds(2);
      
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPinPlastik, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPinPlastik, LOW);
    
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPinPlastik, HIGH);
    
  // Calculating the distance
  distance= duration*0.034/2;

  if (distance < batasAdaOrang) {
    return true;
  } else {
    return false;
  }
}

//A procedure made to close the first lid of the metal trash can
void tutupSekatAtasLogam() {
  ServoSekatAtasLogam.write(5);
}

//A procedure made to move the first lid of the metal trash can
void tengahSekatAtasLogam() {
  ServoSekatAtasLogam.write(45);
}

//A procedure made to open the first lid of the metal trash can
void bukaSekatAtasLogam() {
  ServoSekatAtasLogam.write(90);
}

//A procedure made to close the second lid of the metal trash can
void tutupSekatBawahLogam() {
  ServoSekatBawahLogam.write(180);
}

//A procedure made to open the second lid of the metal trash can
void bukaSekatBawahLogam() {
  ServoSekatBawahLogam.write(80);
}

//A procedure made to close the lid of the metal trash can
void tutupTutupLogam() {
  ServoTutupLogam.write(180);
}

//A procedure made to open the lid of the metal trash can
void bukaTutupLogam() {
  ServoTutupLogam.write(0);
}


//A procedure made to close the first lid of the plastic trash can
void tutupSekatAtasPlastik() {
  ServoSekatAtasPlastik.write(0);
}

//A procedure made to move the first lid of the plastic trash can
void tengahSekatAtasPlastik() {
  ServoSekatAtasPlastik.write(25);
}

//A procedure made to open the first lid of the plastic trash can
void bukaSekatAtasPlastik() {
  ServoSekatAtasPlastik.write(150);
}

//A procedure made to close the second lid of the plastic trash can
void tutupSekatBawahPlastik() {
  ServoSekatBawahPlastik.write(180);
}

//A procedure made to open the second lid of the plastic trash can
void bukaSekatBawahPlastik() {
  ServoSekatBawahPlastik.write(80);
}

//A procedure made to close the lid of the plastic trash can
void tutupTutupPlastik() {
  ServoTutupPlastik.write(180);
}

//A procedure made to open the lid of the plastic trash can
void bukaTutupPlastik() {
  ServoTutupPlastik.write(0);
}

//A procedure made to send the data to ESP-01
void kirimData() {
  String berat = String(ber);
  String ketinggian = bacaTinggiSampah();
  String data;
 
  data.concat(ketinggian);
  data.concat(String("."));
  data.concat(berat);
  data.concat(String(".23");
  espSerial.print(data);
}

//A procedure made to scan RFID card
void cek_rfid() {
  if (!mfrc522.PICC_IsNewCardPresent()) { 
    rfid = "";
    return;
  } 
  if (!mfrc522.PICC_ReadCardSerial()) {
    rfid = "";
    return;
  } 

  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : ""));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));  
  }
  content.toUpperCase();
  
  rfid = content;
}
