/*  GPS data extract to monitor screen - FONA808 Shield  
 *  Author : Emre Sendil
 *  Referance Adafruit, Track Your Treats, Tony DiCola
  * SD card attached to SPI bus as follows:
 ** MOSI - pin 11 on Arduino Uno/Duemilanove/Diecimila ****** MEGA 51
 ** MISO - pin 12 on Arduino Uno/Duemilanove/Diecimila ****** MEGA 50
 ** CLK - pin 13 on Arduino Uno/Duemilanove/Diecimila  ****** MEGA 52
 ** CS - depends on your SD card shield or module.     ****** MEGA 53
     Pin 7 used here **** It was D4for consistency with other Arduino examples
 git test*/


#include <SoftwareSerial.h>       // FONA serial communication.
#include "Adafruit_SleepyDog.h"   // Reset if something screw-up.
#include "Adafruit_FONA.h"        // Manage FONA shield.
#include <SPI.h>                  // Serial Peripheral Interface 
#include <SD.h>                   // SD Card management

// Ayarlar buradaki değerlerin düzenlenmesi gerekecek.

#define LED_PIN              6   // LED connection to monitor progress. 
                                 // Blink once each sec : waiting for GPS 3D fix
                                 // Constantly blink : GPS fixed ready to process
                                 // Blink rapidly 100 msec Something went wrong shall restart project

#define LOGGING_PERIOD_SEC   15  // GPS location collection frequency.

#define FONA_RX              2   // FONA serial RX pin (pin 2 for shield).

#define FONA_TX              10   // FONA serial TX pin (pin 3 for shield)(Pin 10 for MEGA).

#define FONA_RST             4   // FONA reset pin (pin 4 for shield).

#define FONA_APN             "internet"  // GSM internet connection APN parameter, "internet" required for Turkcell can be blank. Check with your operator

#define FONA_USERNAME        ""  // Username for GSM internet connection can be blank. Check with your operator

#define FONA_PASSWORD        ""  // Password for GSM internet connection can be blank. Check with your operator

#define SD_CS_Pin            53  // SD Card CS Pin FOR MEGA

// Global state (you don't need to change this):
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);     // FONA software serial connection.
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);                 // FONA library connection.

String Eski_DK = "DK";
int sayac = 0;
String imeistr = " ";

// Halt function called when an error occurs.  Will print an error and stop execution while
// doing a fast blink of the LED.  If the watchdog is enabled it will reset after 8 seconds.

void IMEIOku() {// Print module IMEI number.
  char imei[15] = {0}; // MUST use a 16 character buffer for IMEI!
  uint8_t imeiLen = fona.getIMEI(imei);
  if (imeiLen > 0) {
    Serial.print("Module IMEI "); Serial.println(imei);
    imeistr = String(imei);
  }
}

void halt(const __FlashStringHelper *error) {
  Serial.println(error);
  while (1) {
    digitalWrite(LED_PIN, LOW);
    delay(100);
    digitalWrite(LED_PIN, HIGH);
    delay(100);
  }
}

void setup() {
  // Initialize serial output.
  Serial.begin(115200);
  // Serial.println(F("GPS data display on serial monitor in desired interval - FONA808 Shield"));

  // Initialize LED and button.
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Initialize the FONA module
  // Serial.println(F("Initializing FONA....(may take 10 seconds)"));
  fonaSS.begin(4800);
  if (!fona.begin(fonaSS)) {
    halt(F("Couldn't find FONA"));
  }
  fonaSS.println("AT+CMEE=2");
  // Serial.println(F("FONA is OK"));

  // Wait for FONA to connect to cell network (up to 8 seconds, then watchdog reset).
  // Serial.println(F("GSM connection starting"));
  while (fona.getNetworkStatus() != 1) {
   delay(500);
  }
  
  // Enable GPS.
  fona.enableGPS(true);
  // Serial.println(F("GPS enabled"));  
  // Use the watchdog to simplify retry logic and make things more robust.
  // Enable this after FONA is intialized because FONA init takes about 8-9 seconds.
  Watchdog.enable(8000);
  Watchdog.reset();
  
  // Start the GPRS data connection.
  fona.setGPRSNetworkSettings(F(FONA_APN), F(FONA_USERNAME), F(FONA_PASSWORD));
  delay(2000);
  Watchdog.reset();
  // Serial.println(F("Disabling GPRS"));
  fona.enableGPRS(false);
  delay(2000);
  Watchdog.reset();
  // Serial.println(F("Enabling GPRS"));
  if (!fona.enableGPRS(true)) {
    halt(F("Failed to turn GPRS on, resetting..."));
  }
  Serial.println(F("Connected to Cellular!"));

  // Wait a little bit to stabilize the connection.
  Watchdog.reset();
  delay(3000);
  
  
  // Serial.print("Initializing SD card...");

  if (!SD.begin(SD_CS_Pin)) {
    Serial.println("SD initialization failed!");
    return;
  }
  Serial.println("SD initialization done.");

  //Get IMEI number 
  
  IMEIOku();
}

boolean SD_Ekle(String dosya, String yazilacak){
  File myFile;
  dosya = dosya + ".txt";
  Serial.print("Dosya Adi :");
  Serial.println(dosya);
  Serial.print("Yazilacak :");
  Serial.println(yazilacak);
  myFile = SD.open(dosya, FILE_WRITE);
  if (myFile) {
    Serial.print("Writing to file...");
    myFile.println(yazilacak);
    // close the file:
    myFile.close();
    Serial.println("done.");
    return true;
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening file");
    return false;
  }
}

uint16_t VoltajOku() {
        // read the battery voltage and percentage
  uint16_t vbat;
  if (! fona.getBattVoltage(&vbat)) {
    Serial.println(F("Failed to read Batt"));
    return 0;
  } else {
    Serial.print(F("VBat = ")); Serial.print(vbat); Serial.println(F(" mV"));
    return vbat;
  }
}

uint16_t PilYuzdeOku() {
        // read the battery voltage and percentage
  uint16_t vbat;
  if (! fona.getBattPercent(&vbat)) {
    Serial.println(F("Failed to read Batt"));
    return 0;
  } else {
    Serial.print(F("Pil Doluluk = %")); Serial.print(vbat);
    return vbat;
  }
}

void GrapGPS() {
  // Grab a GPS reading.
  float latitude, longitude, speed_kph, heading, altitude;
  bool gpsFix = fona.getGPS(&latitude, &longitude, &speed_kph, &heading, &altitude);
  
  // Light the LED solid if there's a GPS fix, otherwise flash it on and off once a second.
  if (gpsFix) {
    digitalWrite(LED_PIN, HIGH);
    Serial.print("GPS fixed ");
    //Capture whole sentence
    char gpsbuffer[120];
    uint8_t res_len = fona.getGPS(32, gpsbuffer, 120);
    String gpsbuffer1 = String(gpsbuffer);
    String Saat = gpsbuffer1.substring(12,16);
    String DK = gpsbuffer1.substring(14,16);
    String Tarih = gpsbuffer1.substring(4,12);
    Serial.print("DK ");
    Serial.println(DK);
    Serial.print("Tarih ");
    Serial.println(Tarih);
    if (DK == Eski_DK){  // Dakika degisince kayıt ediyoruz
        Serial.println("Ayni dk icindeyiz");
      } 
    else {                                      // DK veya Gun degerlerinden birisi ya da ikisi de degistiyse işlem yapılması lazım
      Serial.println("DAKIKA DEGISTI");
          String yazilacak = /*imeistr + "," + */Saat + "," + gpsbuffer1.substring(23,50) + "," + VoltajOku() + ",%" + PilYuzdeOku() + ",";
          String dosya_adi = Tarih;
          if (SD_Ekle(dosya_adi, yazilacak)) {              // işlem başarı ise Eski_DK, DK olarak değiştirilir
          Eski_DK = DK;
          Serial.println("karta yazdik dk update ettik");
        }
        else {
          Serial.println("SIKINTI Karta yazamadik");
        }
      }
    Serial.println(sayac);
    sayac = sayac +1;
  }
  else {
    // No fix, blink the LED once a second and stop further processing.
    digitalWrite(LED_PIN, (millis()/1000) % 2);
    //sayac = 0;
    return;
  }
}

void loop() {
  // Use the watchdog to simplify retry logic and make things more robust.
  // Enable this after FONA is intialized because FONA init takes about 8-9 seconds.
  Watchdog.reset();
  GrapGPS();
}
