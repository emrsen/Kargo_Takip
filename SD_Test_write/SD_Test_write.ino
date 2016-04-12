#include <SPI.h>                  // Serial Peripheral Interface 
#include <SD.h>                   // SD Card management

boolean SD_Ekle(String dosya, String yazilacak){
  File myFile;
  dosya = dosya + ".txt";
  Serial.println(dosya);
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

void setup() {
  Serial.begin(115200);
  
  Serial.print("Initializing SD card...");

  if (!SD.begin(7)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  String Tarih = "20160327";
  String icerik = "Ali Baba";
  SD_Ekle(Tarih,icerik);

}


void loop() {
  // put your main code here, to run repeatedly:

}
