// Libraries
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>
#include <Wire.h>
**************************************************************************************

// OLED display width and height, in pixels
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1 );

//define laserhead and bluetooth pins (Rx,Tx)
SoftwareSerial mySerial(4, 3);
SoftwareSerial BTserial(10, 11);

#define key1 5                                //keyboard key-pin
#define key2 6                                //keyboard key-pin
#define key3 7                                //keyboard key-pin
#define key4 2                                //switch reference point button
#define RX 11                                 //bluetooth
#define TX 10                                 //bluetooth

//variables for measuring read from laserhead and divide
char message = 0;                             //message from laserhead
char Mereni [100];                            //divided strings 
char blt = ' ';                               //bluetooth message save
bool reference_point = 0;                     //reference point(back or front)
bool bluetooth_status = false;                //status for bluetooth
const int analogPin = A1;                     //batery indicator
float analogValue;                            //value from pin A1
double input_voltage;                         //counted voltage
**************************************************************************************

// Run once
void setup() {
  Serial.begin(38400);                        //set serial speed for serial monitor  
  mySerial.begin(19200);                      //set serial speed for laser head
  BTserial.begin(9600);                       //set serial speed for bluetooth
  delay(1000);

  // display setting
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setRotation(0);
  display.setTextWrap(false);
  display.dim(0);

  print_message("Turning on", reference_point, bluetooth_status, 0, 0, 1);
  Serial.println("Arduino is ready");
  delay(1000);
  pinMode(key1, INPUT_PULLUP);
  pinMode(key2, INPUT_PULLUP);
  pinMode(key3, INPUT_PULLUP);

  //Read and count voltage to baterry % -> not working
  analogValue = analogRead (A1);              //Conversion formula for voltage
  Serial.println(analogValue);
  delay (1000); 
  input_voltage = (analogValue * 5.0) / 1024.0;
  Serial.println("vstupni napeti:   ");
  Serial.println(input_voltage);
  //input_voltage = (100/5)*input_voltage;
  String indikation = String(input_voltage,1);
  String indikation2 = String(analogValue,1);
  Serial.println("indikejsn:    ");
  Serial.println(indikation);
  Serial.println("baterka");
// print_message_and_stay(indikation, reference_point, bluetooth_status, 80, 0, 1, 0);
  print_message_and_stay(indikation2, reference_point, bluetooth_status, 80, 0, 1, 0);
  print_message_and_stay("%", reference_point, bluetooth_status, 100, 0, 1, 0);
  delay (1000);   
}
**************************************************************************************

// Repetition
void loop() {

  int  key = 0;
  while (bluetooth_status == false) {
  //  battery(reference_point, bluetooth_status);
    int key =  wait_for_key(bluetooth_status);
    switch (key) {
      case 1:
        open_head(reference_point, bluetooth_status);
        
        key  = wait_for_key(bluetooth_status);
        switch (key) {
            case 1:
              meassure(reference_point, bluetooth_status);
              break;
            case 2:
              close_app(reference_point, bluetooth_status);
              break;
            case 3:
              multiply(reference_point,bluetooth_status) ;
              break;
            case 4:
              tracking(reference_point, bluetooth_status);
              break;         
          }
        break;
      case 2:
        reference_point = refence_point(reference_point, bluetooth_status);
        break;
      case 3:
        bluetooth_status = bluetoothh(bluetooth_status);
        break; 
    }
  }

  
  while (bluetooth_status == true) {
   
    if (!digitalRead(key3)) {
      bluetooth_status = bluetoothh(bluetooth_status);
     }
    if (BTserial.available()) {
      blt = BTserial.read();
      Serial.write(blt);
      switch (blt) {
           
         case 'O':
         open_head(reference_point,bluetooth_status);
         Serial.println("open");
         //BTserial.println("open");
         break;
         
         case 'C':
         close_head();
         Serial.println("close");
         //BTserial.println("close");
         break;
         
         case 'M':
         Serial.println("mes");
         //BTserial.println("mes");
         double delka = meassure(reference_point,bluetooth_status);
         
         break;
      }
    }
  }
}
**************************************************************************************
//// FUNCTIONS ///
**************************************************************************************

// turning bluetooth on/off
int bluetoothh (bool bluetooth_status) {
  if (bluetooth_status == false) {
    print_message_and_stay("Bluetooth ON", reference_point, bluetooth_status, 0, 0, 1,1);
    delay(500);
    bluetooth_status = true; //TURNon
  } else {
    bluetooth_status = false; //TURNoff
    print_message("Bluetooth OFF", reference_point, bluetooth_status, 0, 0, 1);
     delay(500);
  }
  return (bluetooth_status);
}
**************************************************************************************

// ordinary message print
void print_message_and_stay(String message, int reference_point, bool bluetooth_status, int X, int Y, int s, bool cl) {
  if (bluetooth_status == false) {
    if (cl == 1) {
      display.clearDisplay();
    }
    display.setTextSize(s);
    display.setCursor(X, Y);
    display.println(message);
    display.display();
  }
  ref_p(reference_point, bluetooth_status);

}
**************************************************************************************

// short message print
void print_message(String message, bool reference_point, bool bluetooth_status, int X, int Y, int s) {
  if (bluetooth_status == false) {
    print_message_and_stay(message, reference_point, bluetooth_status, X, Y, s, 1);
    delay(1000);
    print_message_and_stay("Ready", reference_point, bluetooth_status, 0, 0, 1, 1);
  }
}
**************************************************************************************

// show used reference point
void ref_p(bool reference_point, bool bluetooth_status) {
  int keyswitch  = digitalRead(key4);
  if (reference_point == 0| keyswitch == true) {
    display.setCursor(120, 0);
    display.print("B");
    display.display();
  } else {
    display.setCursor(120, 0);
    display.print("F");
    display.display();
  }
}
**************************************************************************************

// change reference point
int refence_point(bool reference_point, bool bluetooth_status) {
  if (reference_point == 1) {
    reference_point = 0;
    print_message("Set point: Back", reference_point, bluetooth_status, 0, 0, 1);
  } else {
    reference_point = 1;
    print_message("Set point: Front", reference_point, bluetooth_status, 0, 0, 1);
  }
  return (reference_point);
}
**************************************************************************************

// Function for keyboard
int wait_for_key(bool bluetooth_status) {
  int key = 0;
  if (bluetooth_status == false) {
    while (key == 0) {
      int keyup  = digitalRead(key1);
      int keydown  = digitalRead(key2);
      int keyc  = digitalRead(key3);
      delay(100);
      if (keyup == false && keydown == true) {
        key = 1;
      } else if (keydown == false && keyup == true) {
        key = 2;
      } else if (!keyc) {
        key = 3;
      } else if (keyup == false && keydown == false) {
        key = 4;
      }
    }
  }
  return (key);
}
**************************************************************************************

// opening laserhead
void open_head(int reference_point, bool bluetooth_status) {
  mySerial.listen(); //listen laserhead imstead of bluetooth
  mySerial.println("O");
  if (bluetooth_status == false) {
    print_message_and_stay("Open", reference_point, bluetooth_status, 0, 0, 1, 1);
  }
  delay(500);
  while (mySerial.available() > 0) {
    message = mySerial.read();
  }
  BTserial.listen();
}
// closing laserhead
void close_app(int reference_point, bool bluetooth_status) {
  close_head();
  print_message("Closed", reference_point, bluetooth_status, 0, 0, 1);
}

void close_head() {
  mySerial.listen();
  mySerial.println("C");
  while (mySerial.available() > 0) {
    message = mySerial.read();
  }
  BTserial.listen();
}
**************************************************************************************

// measuring
double meassure(int reference_point, bool bluetooth_status) {
  mySerial.listen(); 
  mySerial.println("F"); // send letter "F" to laser head
  print_message_and_stay("Measuring", reference_point, bluetooth_status, 0, 0, 1, 1);
  delay(2000);

  int i = 0;
  while (mySerial.available() > 0) {
    message = mySerial.read();
    Mereni[i] = message; // save message from laserhead
    i++;
    }
  BTserial.listen();
 // BTserial.println(Mereni);

  char *strings[4];
  char *ptr = NULL;
  byte index = 0;
  ptr = strtok(Mereni, ":m,"); //divide message to strings
  strings[2] = NULL;//set intenzite to null
  while (ptr != NULL) {
    strings[index] = ptr;
    index++;
    ptr = strtok(NULL, ":m, ");  // takes a list of delimiters
  //  BTserial.print(index);
   // BTserial.println(strings[index]);
  }
   
  double mes;//vysledna delka v typu 

  int keyswitch  = digitalRead(key4);
 if(bluetooth_status==false){ // pokud jde delka do aplikace...zanechame tzp string
  if (reference_point == 0 | keyswitch == true) {
    mes = atof(strings[1]) + 0.12; //nastaveni odsazeni pro back
  } else {
    mes = atof(strings[1]) + 0.0; //nastaveni odsazeni pro front
  }
 }
//strings[1] = "45.256"; //test
//strings[2] = "0076"; //test
 if(bluetooth_status==true){
  
  BTserial.print(atof(strings[1]),3);
  BTserial.print("m,");
  BTserial.println(atof(strings[2]),0);
 }
  if (atof(strings[1]) == 0.00) {
    print_message("Err-Short distance", reference_point, bluetooth_status, 0, 0, 1); //kratka vzdlenost
  } else if (atof(strings[2]) > 1000 || atof(strings[2]) == 0.00) {
    print_message("Err-Bad surface", reference_point, bluetooth_status, 0, 0, 1); //spatny povrch,uhel,daleko
  } else {
    if(bluetooth_status==false){
    Serial.print(mes);
    print_message_and_stay("Dist: ", reference_point, bluetooth_status, 0, 0, 1, 1);
    display.setTextSize(2);
    display.setCursor(30, 15);
    display.print(mes, 3); //vypis format double,pocet des mist
    display.display();
    print_message_and_stay(" m", reference_point, bluetooth_status, 100, 25, 0, 0);
    }
  }
  delay(600);
  close_head();
  return (mes);
}
**************************************************************************************

//tracking
void tracking(bool reference_point, bool bluetooth_status) {
  int key  = true;
      print_message_and_stay("Tracking", reference_point, bluetooth_status, 0, 0, 1,1);
       delay(500);
  while (key == true) {
  
    meassure(reference_point, bluetooth_status);
    delay(1800);
    if (!digitalRead(key3)) {
      key = false;
    }
  }
  print_message("END", reference_point, bluetooth_status, 0, 0, 1);
  delay(500);

}
**************************************************************************************

//multiply
void multiply(int reference_point, bool bluetooth_status) {
  print_message_and_stay("Area", reference_point, bluetooth_status, 0, 0, 1,1);
  double distance2;
  double distance1;
  int k = 0;
  int c = 0;
  print_message_and_stay("Meassure D1", reference_point, bluetooth_status, 0, 0, 1,1);
  while (k == 0) {
    if (!digitalRead(key2)) {
      close_app(reference_point, bluetooth_status);
      c = 1;
      k = 1;
    }

    if (!digitalRead(key1)) {
      distance1 =  meassure(reference_point,bluetooth_status);
      k = 1;
    }
  }

  k = 0;
  Serial.print(c);
  if (c == 0) {
    delay(500);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Meassure D2");
    display.print("D1: ");
    display.print(distance1, 3);
    display.print(" m");
    display.display();

    while (k == 0) {
      if (!digitalRead(key2)) {
       close_app(reference_point, bluetooth_status);
        k = 1;
        c = 1;
        break;
      }
      if (c == 0) {
        Serial.print(c);
        if (!digitalRead(key1)) {
               mySerial.listen();
          mySerial.println("O");
          print_message_and_stay("Open",  reference_point, bluetooth_status, 0, 0, 1,1);
          delay(500);
          mySerial.listen();
          while (mySerial.available() > 0) {
            message = mySerial.read();
            Serial.print(message);
          }
            BTserial.listen();

          int j = 0;
          while (j == 0) {

            if (!digitalRead(key2)) {
              close_app(reference_point, bluetooth_status);
              k = 1;
              c = 1;
              j = 1;
              break;
            }

            if (!digitalRead(key1)) {
              distance2 =  meassure(reference_point, bluetooth_status);
              k = 1;
              j = 1;
            }
          }


          if (c == 0) {
            delay(500);
            double vysledek = distance1 * distance2;
            display.setCursor(0, 0);
            display.clearDisplay();
            display.print("D1: ");
            display.print(distance1, 3);
            display.println(" m");
            display.print("D2: ");
            display.print(distance2, 3);
            display.println(" m");
            display.print("P: ");
            display.setTextSize(2);
            display.print(vysledek, 3);
            display.setTextSize(1);
            display.println("m2");
            display.display();
          }
        }
      }
    }
  }
}
