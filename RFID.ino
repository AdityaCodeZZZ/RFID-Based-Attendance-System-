#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);                                      // Create MFRC522 instance

// Valid UIDs of students
byte validUIDs[][7] = {
  {0x13, 0xB5, 0xAB, 0xF7},  // UID of student 1
  {0x33, 0x7E, 0x57, 0xF7},  // UID of student 2
  {0x03, 0xE0, 0x32, 0x11},  // UID of student 3
  {0xA3, 0xF7, 0x17, 0x2F},  // UID of student 4
  {0x04, 0x53, 0x7C, 0x1A, 0x2F, 0x75, 0x80},  // UID of student 4
};

// Student names
const char* studentNames[] = {
  "SRI KRISHNA",  // Name of student 1
  "ADITYA KUMAR",    // Name of student 2
  "VANSHAJ",       // Name of student 3
  "VINAY SAUNSHI ",     // Name of student 4
  "Yash GAUTAM ",     // Name of student 4
};


// LCD I2C display
LiquidCrystal_I2C lcd(0x27, 20, 4);  // Adjust the address and size according to your display

void setup() {
  Serial.begin(9600);   // Initialize serial communication
  SPI.begin();          // Initiate SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  lcd.begin(16, 2);     // Initialize the LCD display
  lcd.init();
  lcd.backlight();      // Turn on the backlight
  lcd.clear();
  lcd.setCursor(5, 1);
  lcd.print("Welcome!!");
  lcd.setCursor(4, 2);
  lcd.print("Scan ID card");
  Serial.println("Ready to read RFID cards");
  pinMode(8, OUTPUT); // Configure pin 8 as output for the buzzer
  pinMode(7, OUTPUT); // Configure pin 7 as output for the LED
}

void loop() {
  // Look for new cards
  if (mfrc522.PICC_IsNewCardPresent()) {
    if (mfrc522.PICC_ReadCardSerial()) {
      // Show UID on serial monitor
      Serial.print("UID tag: ");
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print("0x");
        if (mfrc522.uid.uidByte[i] < 0x10) Serial.print("0");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
        if (i < mfrc522.uid.size - 1) Serial.print(", ");
      }
      Serial.println();
      Serial.print("UID Number: ");
      String content = "";
      byte letter;
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
        content.concat(String(mfrc522.uid.uidByte[i], HEX));
      }
      content.toUpperCase();
      Serial.println(content);

      // Check if the UID matches any of the valid UIDs
      bool uidMatched = false;
      int studentIndex = -1;
      for (int i = 0; i < sizeof(validUIDs) / sizeof(validUIDs[0]); i++) {
        if (memcmp(mfrc522.uid.uidByte, validUIDs[i], mfrc522.uid.size) == 0) {
          uidMatched = true;
          studentIndex = i;
          break;
        }
      }

      // Perform actions based on UID match
      if (uidMatched) {
        // UID matches, perform attendance action for the corresponding student

        // Update the attendance status in the database
        // Example: Update the attendance status for student studentIndex+1 to "Present"
        updateAttendance(studentIndex + 1, "Present");

        delay(3000);  // Display attendance status for 3 seconds
      } else {
        // UID doesn't match, perform other actions (e.g., display error message)

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("ID doesn't match");
        delay(2000);
      }

      // Clear the LCD display and reset for the next scan
      lcd.clear();
      lcd.setCursor(5, 1);
      lcd.print("Welcome!!");
      lcd.setCursor(4, 2);
      lcd.print("Scan ID card");
      delay(1000);   // Delay to avoid reading the card multiple times in a short period
    }
    mfrc522.PICC_HaltA();   // Stop reading
    mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
  }
}

// Function to update attendance in the database
void updateAttendance(int studentID, const char* status) {
  // Implement your code to update the attendance status in the database
  // based on the studentID and status parameters

  // Display the name of the student on the LCD
  lcd.clear();
  lcd.setCursor(5, 1);
  lcd.print(studentNames[studentID - 1]);  // -1 to adjust for array indexing
  lcd.setCursor(4, 2);  // Set cursor to the second line
  lcd.print("~");
  lcd.setCursor(5, 2);  // Set cursor position after 4 pixels
  lcd.print(status);

  // Control the buzzer and LED based on attendance status
  if (strcmp(status, "Present") == 0) {
    digitalWrite(8, HIGH); // Turn on the buzzer
    digitalWrite(7, HIGH); // Turn on the LED
    delay(1000); // Delay to keep buzzer and LED on for 1 second
    digitalWrite(8, LOW); // Turn off the buzzer
    digitalWrite(7, LOW); // Turn off the LED
  } else {
    digitalWrite(7, HIGH); // Turn on the LED
    delay(1000); // Delay to keep LED on for 1 second
    digitalWrite(7, LOW); // Turn off the LED
  }
}
