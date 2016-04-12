
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>
#include <dht11.h> 

#undef int
#undef abs
#undef double
#undef float
#undef round
dht11 DHT11;
#define DHT11PIN 3


///////////////////////////////
///      EDIT THIS STUFF     //
///////////////////////////////

byte mac[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };  //Replace with your Ethernet shield MAC
byte ip[] = {123,456,7,890};     // Your Arduino device IP address

char devid = v42FE15BC09B20df  // THIS IS THE DEVICE ID FROM PUSHINGBOX

int del=300;  // Amount of seconds delay between posting to google docs.

///////////////////////////////
//       DONE EDITING        //
///////////////////////////////


 

char postmsg[100];
int k=0;
int temp_av = 0;
char server[] = "api.pushingbox.com";
EthernetClient client;
 
 
void setup()
{
  Serial.begin(9600);
  Ethernet.begin(mac, ip);
  delay(1000);
  Serial.println("connecting...");
}
 
void loop(){
  
  
  
  
  // average temp reading for 'del' time.........................................
 
  for(int j=0; j<del;j++)
  {
    // Read local temp........................................
    int chk = DHT11.read(DHT11PIN);
    int temp = Fahrenheit(DHT11.temperature);
    temp_av=temp_av+temp;
    delay(1000);
  }
  
  int avtemp=temp_av/(del);
  temp_av=0;
    
    
    
  
 // Post to Google Form.............................................
  if (client.connect(server, 80)) 
  {
    k=0;
    Serial.println("connected");
    sprintf(postmsg,"GET /pushingbox?devid=%c&status=%d HTTP/1.1",devid,avtemp);  // NOTE** In this line of code you can see where the temperature value is inserted into the wed address. It follows 'status=' Change that value to whatever you want to post.
    client.println(postmsg);
    client.println("Host: api.pushingbox.com");
    client.println("Connection: close");
    client.println();

    Serial.println(postmsg);
    Serial.println("Host: api.pushingbox.com");
    Serial.println("Connection: close");
    Serial.println();
 
     delay(1000);
     client.stop();
  }
  delay(1000);
  
  if (!client.connected()) 
  {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    k==1;
    return;
  }
  
 
}


double Fahrenheit(double celsius) // Function to convert to Fahrenheit
{
	return 1.8 * celsius + 32;
}

