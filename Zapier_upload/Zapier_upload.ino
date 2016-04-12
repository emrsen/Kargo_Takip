#include <Adafruit_FONA.h>
#include <SD.h>

#define SD_CS 5

#define FONA_RST 8
#define FONA_KEY 9 

Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

// sends the requested number of bytes from the log file from sd card to service of your choice
boolean sendLog(long requested)
{
  // first turn Fona on
  long targettime = millis() + 2500;
  digitalWrite(FONA_KEY, LOW);
  while(millis() < targettime)  ; // wait for Fona to have started up
  digitalWrite(FONA_KEY, HIGH);
  
  // using Hardware Serial2 on Mega - change his for other Arduinos
  Serial2.begin(115200);
  if(! fona.begin(Serial2)) return false;

  // unlock sim - change this to your SIM pin
  if(! fona.unlockSIM("1234")) return false;

  // wait until we get network connection (home or roaming)
  int nstatus = fona.getNetworkStatus();
  for(int j = 0; j < 100 && ! (nstatus == 1 || nstatus == 5);j++)
  {
    delay(100);
    nstatus = fona.getNetworkStatus();
  }
  if(! (nstatus == 1 || nstatus == 5)) return false;

  // change this to your APN, user and password
  fona.setGPRSNetworkSettings(F("pinternet.interkom.de"), 0, 0);

  // give fona 5 seconds before going for GPRS
  delay(5000);  
  if(! fona.enableGPRS(true)) return false;
  
  // open log file and seek position to read from  ************************
  if(! SD.begin(SD_CS)) return false;
  dataFile = SD.open("datalog.txt");
  if(! dataFile) return false;
  unsigned long filesize=dataFile.size();
  dataFile.seek(filesize-requested);

  // Initialize and set parameters for http post  
  if (! fona.HTTP_init()) return false;
  if (! fona.HTTP_para(F("CID"), 1)) return false;
  if (! fona.HTTP_para(F("UA"), F("FONA"))) return false;

  // change URL to the one you want to use
  if (! fona.HTTP_para(F("URL"), "https://zapier.com/hooks/catch/3p7lj7/")) return false;
  if (! fona.HTTP_para(F("CONTENT"), F("application/json"))) return false;

  // HTTP POST data
  // change length and manual write characters to what you need
  long postdatalen = requested + 8 + 2;
  if (! fona.HTTP_data((uint16_t) postdatalen, postdatalen)) return false;
  fona.write((uint8_t)'{'); // 8 characters
  fona.write((uint8_t)'\"');
  fona.write((uint8_t)'l');
  fona.write((uint8_t)'o');
  fona.write((uint8_t)'g');
  fona.write((uint8_t)'\"');
  fona.write((uint8_t)':');
  fona.write((uint8_t)'\"');
  while(dataFile.available())
  {
    uint8_t next = (uint8_t) dataFile.read();
    fona.write(next);
  }
  fona.write((uint8_t) '\"'); // 2 characters
  fona.write((uint8_t) '}');
  if (! fona.expectReply(F("OK")))
    return false;

  // HTTP POST
  uint16_t statuscode;
  uint16_t datalength;
  if (! fona.HTTP_action(FONA_HTTP_POST, &statuscode, &datalength)) return false;

  // HTTP response data
  if (! fona.HTTP_readall(&datalength)) return false;

  // Close HTTP session
  fona.HTTP_POST_end();
  
  // Turn GPRS off again (neeeded to enable SMS)
  if(! fona.enableGPRS(false)) return false;

  // turn Fona off again
  long targettime = millis() + 2500;
  digitalWrite(FONA_KEY, LOW);
  while(millis() < targettime)  ; // wait for Fona to shut down
  digitalWrite(FONA_KEY, HIGH);
}
