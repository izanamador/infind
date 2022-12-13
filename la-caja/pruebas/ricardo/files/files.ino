#include <LittleFS.h>
//http://www.stensat.org/docs/sys395/04_filesystem.pdf
void setup()
{
 Serial.begin(115200);
 LittleFS.begin();
 //File f = LittleFS.open("/file.txt","w");
 //f.println("This is the first line");
 //f.println("This is the second line");
 //f.close();
 delay(1000);
 //f = LittleFS.open("/file.txt","r");// ”,”r”);
 //while(f.available()) {
 //String line = f.readStringUntil('\n');
//Serial.println(line);
 //}
 //f.close();
}
void loop()
{
  delay(1000);
  
  File f = LittleFS.open("/file.txt","r");// ”,”r”);
 while(f.available()) {
 String line = f.readStringUntil('\n');
Serial.println(line);
 }
 f.close();
 delay(5000);
}