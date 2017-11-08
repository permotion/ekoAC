/*
 * MFRC522 PINOUT
 *              SDA:  PIN 9
 *              SCK:  PIN 13
 *              MOSI: PIN 11 
 *              MISO: PIN 12
 *              IRQ:  NOT CONNECTED
 *              GND:  ARDUINO GROUND
 *              RST:  PIN 8
 *              3.3V: ARDUINO 3.3V
 * 
 */

#include<SPI.h>
#include<Ethernet.h>
#include <MFRC522.h>
#include <SD.h>

#define RST_PIN         6         // Configurable, see typical pin layout
#define SS_PIN          9         // Configurable, see typical pin layout

#define SS_SD           4
#define SS_ETH          10
#define DOOR_PIN        7

File myFile;

boolean isCardPresent;
String carduid;
String current_line;



MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance


void openDoor() {
  // this will open the door!
  Serial.println("\nThis function will open the door!");
  digitalWrite(DOOR_PIN, LOW); 
  delay(5000);
  digitalWrite(DOOR_PIN, HIGH); 
}


void setup() {
  Serial.begin(9600);
  pinMode(DOOR_PIN, OUTPUT); 
  digitalWrite(DOOR_PIN, HIGH); 
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  if (!SD.begin(SS_SD)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  
  SPI.begin();                        // Init SPI bus
  mfrc522.PCD_Init();                 // Init MFRC522
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details

  pinMode(SS_SD, OUTPUT);      
  pinMode(SS_ETH, OUTPUT);      


  delay(1000);
  Serial.println("Apoya la tarjeta papi:");
}

void loop() {
  digitalWrite(SS_SD, HIGH); 
  digitalWrite(SS_ETH, HIGH); 

  // Revisamos si hay nuevas tarjetas  presentes
  if ( mfrc522.PICC_IsNewCardPresent()) 
      {  
      //Seleccionamos una tarjeta
            if ( mfrc522.PICC_ReadCardSerial()) 
            {
                  // Enviamos serialemente su UID
                  String UIDtarjeta = "";
                  Serial.print("\nCard UID:");
                  for (byte i = 0; i < mfrc522.uid.size; i++) {
                                  
                          Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
                          UIDtarjeta.concat(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
                          Serial.print(mfrc522.uid.uidByte[i], HEX);   
                          UIDtarjeta.concat(String(mfrc522.uid.uidByte[i], HEX));
                  } 
                  carduid = String(mfrc522.uid.uidByte[0],HEX)+String(mfrc522.uid.uidByte[1],HEX)+String(mfrc522.uid.uidByte[2],HEX)+String(mfrc522.uid.uidByte[3],HEX);
                  Serial.println();
                  // Terminamos la lectura de la tarjeta  actual
                  mfrc522.PICC_HaltA();
                    
                  isCardPresent = true;  
            }      

            if (isCardPresent){
              digitalWrite(SS_PIN, HIGH);
              digitalWrite(SS_SD, LOW);

              Serial.print(carduid);
              myFile = SD.open("eko.txt");
              if (myFile) {              
                // read from the file until there's nothing else in it:
                while (myFile.available()) {
                  current_line = myFile.readStringUntil('\n');
                  //Serial.print(current_line);
                  bool match = true;
                  for (int i = 0; i < 8; i++) {

                    if (current_line[i] != carduid[i])
                      match = false;
                  }
                  if (match) {
                    Serial.write("\nArde Papi!! Te felicito!");
                    Serial.println();
                    Serial.print(current_line);
                    openDoor();
                  }
                }
                // close the file:
                myFile.close();
              } else {
                // if the file didn't open, print an error:
                Serial.println("error opening test.txt");
              }
            }
  } 

}




