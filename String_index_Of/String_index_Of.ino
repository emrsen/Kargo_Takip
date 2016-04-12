


void setup() {
  Serial.begin(115200);
}

void loop() {
  String gpsbuffer = "1,1,20160326152152.000,40.976523,29.055358,688.400,0.81,77.8,1,,1.0,1.3,0.8,,11,6,,,18,,";
  int birinci, ikinci, ucuncu, dorduncu, besinci;
  Serial.println(gpsbuffer);
  birinci = gpsbuffer.indexOf(',');
  Serial.print("Birinci virgul yeri ");
  Serial.println(birinci);
  ikinci = gpsbuffer.indexOf(',',birinci+1);
  Serial.print("Ikinci virgul yeri ");
  Serial.println(ikinci);
  ucuncu = gpsbuffer.indexOf(',',ikinci+1);
  Serial.print("Ucuncu virgul yeri ");
  Serial.println(ucuncu);
  dorduncu = gpsbuffer.indexOf(',',ucuncu+1);
  Serial.print("Dorduncu virgul yeri ");
  Serial.println(dorduncu);
  besinci = gpsbuffer.indexOf(',',dorduncu+1);
  Serial.print("Besinci virgul yeri ");
  Serial.println(besinci);
  Serial.print("Zaman :");
  String zaman = gpsbuffer.substring(ikinci+1,ucuncu);
  Serial.println(zaman);
  String tarih = zaman.substring(0,8);
  Serial.println(tarih);
  String SAAT = zaman.substring(8,14);
  Serial.println(SAAT);
  String saat = SAAT.substring(0,2);
  Serial.println(saat);
  String dk = SAAT.substring(2,4);
  Serial.println(dk);
  while(true);
}
