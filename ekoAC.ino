
#include<SPI.h>
#include<Ethernet.h>
#include <MFRC522.h>

#define RST_PIN         8         // Configurable, see typical pin layout
#define SS_PIN          9         // Configurable, see typical pin layout
#define RELAY_PIN       7
#define LED_PIN         6
int flag = 0;

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0x00, 0x90, 0xF5, 0xC4, 0x76, 0x01 };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "192.168.11.1";    // name address for Google (using DNS)

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 11, 178);

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

void setup() {
  pinMode(RELAY_PIN, OUTPUT); 
  pinMode(LED_PIN, OUTPUT); 
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  SPI.begin();                        // Init SPI bus
  mfrc522.PCD_Init();                 // Init MFRC522
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  Ethernet.begin(mac, ip);
   // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("Apoya la tarjeta papi:");

}

void loop() {

  // Revisamos si hay nuevas tarjetas  presentes
  if ( mfrc522.PICC_IsNewCardPresent()) 
      {  
      //Seleccionamos una tarjeta
            if ( mfrc522.PICC_ReadCardSerial()) 
            {
                  // Enviamos serialemente su UID
                  String UIDtarjeta = "";
                  Serial.print("Card UID:");
                  for (byte i = 0; i < mfrc522.uid.size; i++) {
                                  
                          Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
                          UIDtarjeta.concat(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
                          Serial.print(mfrc522.uid.uidByte[i], HEX);   
                          UIDtarjeta.concat(String(mfrc522.uid.uidByte[i], HEX));
                  } 
                  //String carduid = String(mfrc522.uid.uidByte[0],HEX)+String(mfrc522.uid.uidByte[1],HEX)+String(mfrc522.uid.uidByte[2],HEX)+String(mfrc522.uid.uidByte[3],HEX);
                  Serial.println();
                  // Terminamos la lectura de la tarjeta  actual
                  mfrc522.PICC_HaltA();
                  if (client.connect(server, 80)) {
                  Serial.println("connected");
                  flag=1;
                   // Make a HTTP request:
                  client.print("GET /prueba.php?uid=");
                  client.print(UIDtarjeta);
                  client.println();
                  client.println("Host: www.mipc.com.ar");
                  client.println("Connection: close");
                  client.println();
                  } else {
                 // if you didn't get a connection to the server:
                  Serial.println("connection failed");
                  }
                           
            }      
  } 
  
  // if there are incoming bytes available
  // from the server, read them and print them:
  char inData[20]; // string maximo que lee porque son palabras cortitas que le devuelvo
  char inChar; //aca guardo el umo caracter leido
  byte index=0; //indice
  
  if (client.available() && flag) {
    while(client.available() > 0)
    {
      if ( index < 19 ) {
        inChar = client.read();
        inData[index]=inChar;
        index++;
        inData[index]='\0';
      }
    }
    //salio del while aca esta la cadena entera
    Serial.println(inData);
    if(strcmp(inData,"authorized")==0){
      Serial.println("Acciono apertura por rele de cerradura");
      digitalWrite(RELAY_PIN, HIGH); 
      digitalWrite(LED_PIN, HIGH); 
      delay(3000);
      Serial.println("Quito accion del rele para que la puerte quede cerrada");
      digitalWrite(RELAY_PIN, LOW);
      digitalWrite(LED_PIN, LOW); 
    }
  }

  // if the server's disconnected, stop the client:
  if (!client.connected() && flag) {
    flag=0;
    Serial.println();
    Serial.println("disconnecting.");
    Serial.println("Apoya la tarjeta papi:");
    client.stop();
    
  }
}

