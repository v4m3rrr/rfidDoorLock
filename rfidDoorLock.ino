
#include <MFRC522.h>
#include <SPI.h>
#include <Servo.h> 

#define RST_PIN 9
#define SS_PIN 10

#define GREEN_PIN 4
#define RED_PIN 3

#define LOCK_PIN 2

#define SERVO_PIN 5

/* Create an instance of MFRC522 */
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::StatusCode status;
MFRC522::MIFARE_Key key;
int blockNum=20;

Servo servo;
#define OPEN_VAL 95
#define CLOSE_VAL 0

void setup(){
  pinMode(GREEN_PIN,OUTPUT);
  pinMode(RED_PIN,OUTPUT);

  pinMode(LOCK_PIN,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(LOCK_PIN), closeDoor, CHANGE);

  digitalWrite(RED_PIN,HIGH);
  digitalWrite(GREEN_PIN,LOW);
  
  servo.attach(SERVO_PIN);
  servo.write(CLOSE_VAL);

  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Scanning...");
}

void loop(){

  

  /* Prepare the ksy for authentication */
  /* All keys are set to FFFFFFFFFFFFh at chip delivery from the factory */
  byte keyA[]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
  for (byte i = 0; i < 6; i++)
  {
    key.keyByte[i] = keyA[i];
  }
 
  /* Reset the loop if no new card is present on RC522 Reader */
  if ( ! mfrc522.PICC_IsNewCardPresent()){
    return;
  }
    
  
  /* Select one of the cards */
  if ( ! mfrc522.PICC_ReadCardSerial()){
    Serial.println("READ CARD SERIAL");
    return;
  }
    
  Serial.print("\n");
  Serial.println("**Card Detected**");
  /* Print UID of the Card */
  Serial.print(F("Card UID:"));
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.print("\n");
  /* Print type of card (for example, MIFARE 1K) */
  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  /* Authenticating the desired data block for write access using Key A */
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print("Authentication failed for Write: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  else
  {
    Serial.println("Opening the lock");
    servo.write(OPEN_VAL);
    digitalWrite(GREEN_PIN,HIGH);
    digitalWrite(RED_PIN,LOW);
  }
  mfrc522.PICC_HaltA(); // Halt PICC
  mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
}

void closeDoor(){
  servo.write(CLOSE_VAL);
  digitalWrite(RED_PIN,HIGH);
  digitalWrite(GREEN_PIN,LOW);
}
