#include "tReader.c"

//const char * delimiter = "\n";
//charStar gets the input from the file
char * charStar;
//String of charStar
String str;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("setup");
  pinMode(11,OUTPUT);
  digitalWrite(11, LOW);
  delay(500);
  digitalWrite(11, HIGH);
  delay(500);
  digitalWrite(11, LOW);
}
void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println("hiii");
  charStar = readFile();
  //Serial.println("hiii");
  Serial.print(charStar);
  //char c = *charStar;
  //int len = 0;
  //while(c != 3 && len < 10){
   // c = *(charStar + len + 1);
  //  len++;
  //}
  //Serial.println(len);
  str = String(charStar);
  byte bytes[str.length()];
  str.getBytes(bytes, str.length());
  
  for(int i = 0; i < str.length(); i++){
     Serial.println(bytes[i]); 
  }
  Serial.print(" ");
  digitalWrite(11, HIGH);
  Serial.println(str.length());
  
  //str.getBytes(
}
