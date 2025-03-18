/* File: BTSerial_HC05.ino

BlueTooth - HC05 @ Arduino Uno R3

 - 2025/03/19 
*/

// Reference :
//// https://mytectutor.com/hc-05-bluetooth-module-with-arduino/
//// https://mischianti.org/arduino-remote-wireless-programming/
// ? TX + resistor !?

// AT cmd of HC-05
//// https://alselectro.wordpress.com/2014/10/21/bluetooth-hc05-how-to-pair-two-modules/
//// https://curtocircuito.com.br/blog/Categoria%20Arduino/arduino-e-hc-05:-configuracao-master-slave
//// https://www.taiwaniot.com.tw/wp-content/uploads/2016/01/ADIO-HC-05-1.pdf

/////////////////////////////////////////////////////
/*
HW Pin Connect (Uno R3):

Pin #5: I: SoftSerial RX <----> HC5#TX
Pin #6: O: SoftSerial TX <--(電阻分壓降壓)--> HC5#RX
Pin #7: I: <----> HC5#Status <--(並聯)--> LED+電阻
Pin #8: O: <----> HC5#EN
Pin #4: O: <----> Relay <----> HC#PWR(5V)   --x 直接推,每1.4s 會掉一次電壓(2.5v) <-- 不斷Reset 
#GND...

*/


#include <Arduino.h>  // HC-05實測 on Arduino UNO R4 WiFi

// #define ENPIN        // For config HC-05 ONLY!   For normal communicate,  *REMARK THIS* !!
#define SOFT_SERIAL  // For Uno R3, no other choice...
#define UNO 3        // I have no UNO 4  :<

//// -- CONST of PINs define
#if (3 != UNO)
  const byte RX_PIN = 5;
  const byte TX_PIN = 6;
#else   // 3==UNO
  const byte RX_PIN = 5;
  const byte TX_PIN = 6;
#endif  // if UNO

const byte STATE_PIN = 7;  // STATE=HIGH if BT connected

// no matter what, pull it high/down
//#ifdef ENPIN
const byte EN_PIN = 8;  // KEY/EN pin = HIGH when HC-05 boot up, switch module to AT mode, baud=38400
// EN=LOW when HC-05 boot up, enter BT Serial TXT mode, baud=9600 or setting by AT-command
//#endif  // ENPIN

const byte PWR_PIN = 4;  // Power! for reset! ;  #9 FAIL! (PWM?) VolDown/1.4s

#ifdef SOFT_SERIAL
#include <SoftwareSerial.h>

SoftwareSerial BTSerial(RX_PIN, TX_PIN);
#else
#define BTSerial Serial1  // only R"4" or Mega,Leonado..etc., NOT R"3"
//// `Serial1`  *NOT* available in Arduino Uno R"3".
#endif                    // SOFT_SERIAL


void setup() {
  ////YG: ++
  digitalWrite(PWR_PIN, LOW);  // Power Off - SHUTDOWN
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, LOW);  // Power Off - SHUTDOWN

  pinMode(EN_PIN, OUTPUT);
#ifdef ENPIN
  digitalWrite(EN_PIN, HIGH);  // AT-COMMAND mode, take effect only at boot up
#else                          // ENPIN
  digitalWrite(EN_PIN, LOW);  // AT-COMMAND mode, take effect only at boot up
#endif                         // ENPIN

#ifdef SOFT_SERIAL
  pinMode(RX_PIN, INPUT);
  pinMode(TX_PIN, OUTPUT);
#endif
  pinMode(STATE_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(38400);  //xx 115200&74880 - data lost(AT+UART? -> AT+UAR?) // 57600 fail after 140 byte. // 38400 fail after 576 byte.
  delay(500);
  digitalWrite(PWR_PIN, HIGH);  // Power On
  delay(180);
  //// Open BT Serial Port
#ifdef ENPIN
    BTSerial.begin(38400);
#else  // ENPIN
  #if 1
    BTSerial.begin(38400);
  #else   // 1
    //// Factory Default Speed
    BTSerial.begin(9600);
  #endif  // 1
#endif  //ENPIN
  //BTSerial.begin(115200);

  //Serial.begin(9600);   // 115200 // 38400
  while (!Serial)
    ;

#ifdef ENPIN
  while (!BTSerial)
    ;
  #if 1
    //BTSerial.begin(38400);
    delay(2500);
    //// Config  BT UART Speed !!
    BTSerial.print("AT+UART=38400,0,0\r\n");
    Serial.println("\n EN1,BT_UART=38400\r\n EN=1\r\n");
  #else   // 0
    //BTSerial.begin(9600);
    delay(2500);
    //// Factory Default: "AT+ORGL"
    BTSerial.print("AT+UART=9600,0,0\r\n");
    Serial.println("\n EN1,BT_UART=9600\r\n");
  #endif  // 0
  //BTSerial.print("AT+UART?\r\n");
  //delay(100);

#else   // ENPIN
  Serial.println("\n EN0,UART=38400 ! Not 9600 \r\n");
#endif  //ENPIN

  // while(!BTSerial)  ;
  // delay(200);   // long delay is must
  delay(700);

  BTSerial.print("AT+UART?\r\n");
  // BTSerial.print("AT\r\n");
  delay(500);  // long delay is must
  if (BTSerial.available()) {
    Serial.println("<--");
    while (BTSerial.available()) {
      Serial.write(BTSerial.read());
    }
    Serial.println(" // -->");
  }
#ifdef ENPIN
  digitalWrite(EN_PIN, LOW);  // YG:
#endif                        // ENPIN

  Serial.println("=== ready ===");
}

void loop() {
  int tmp[2];
  tmp[1] = 0;
  // put your main code here, to run repeatedly:
  // The code below allows for commands and messages to be sent from COMPUTER (serial monitor) -> HC-05
  if (Serial) {
    if (Serial.available()) {  // Keep reading from Arduino Serial Monitor
      tmp[0] = Serial.read();
      //BTSerial.write((unsigned char)Serial.read());  // and send to HC-05
      BTSerial.write(tmp[0]);
      Serial.write(tmp[0]); // echo 
    }
  }

  // The code below allows for commands and messages to be sent from HC-05 -> COMPUTER (serial monitor)
  if (BTSerial.available())                        // Keep reading from HC-05 and send to Arduino
    if (Serial) 
      Serial.write((unsigned char)BTSerial.read());  // Serial Monitor

  // BTSerial.print("AT\r\n");
  // delay(10);
  digitalWrite(LED_BUILTIN, digitalRead(STATE_PIN));
}

