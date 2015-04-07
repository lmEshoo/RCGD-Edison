#include <LiquidCrystal.h>
#include <Servo.h>

//LCD Hookups
#define RS 26
#define EN 27
#define D4 22
#define D5 23
#define D6 24
#define D7 25
#define LCD_BACKLIGHT A4

//Motors Hookups
#define BACK_MOTOR 12
#define FRONT_MOTOR 13
#define BACK_BRAKE 9
#define FRONT_BRAKE 8

LiquidCrystal lcd(EN, RS, D7, D6, D5, D4);

void setup(){
   Serial.begin(9600);
   lcdSetup();
   carSetupAndStart();  
   delay(500);
} //setup

void loop(){
  /////////////////////////////////////////////
  //                CHECK FOR INPUT          //
  ////////////////////////////////////////////
//  if(driveAlone){
//    //Serial.println("DRIVE_ALONE_ON!");
//    selfDrive();
//  }
  /////////////////////////////////////////////
  //                YOU DRIVE               //
  ////////////////////////////////////////////
  if(Udrive){
    int OUT_UD = atoi(Umsg);  //129 | MIN:0 | MAX:255
    int OUT_LR = atoi(Lmsg);  //128 | MIN:0 | MAX:255
    //Serial.println(OUT_UD);
    int LR = map(OUT_LR, 100, 355, 0, 255);
    int UD = map(OUT_UD, 360, 615, 0, 255);
    Serial.print(UD);
    Serial.println(LR);
    
    if( (UD == 129 && LR == 128) || (UD < 0 && LR < 0)
        || UD < 0){
      Serial.println("STOP1");
      digitalWrite(BACK_BRAKE, HIGH);
      digitalWrite(FRONT_BRAKE, HIGH);
    }
    else if( UD >= 129 ){
      //GO_STRAIGHT
      if( LR > 128 ){
       //GO RIGHT 
       Serial.println("GORIGHT");
       digitalWrite(FRONT_BRAKE, LOW);
       digitalWrite(FRONT_MOTOR, HIGH);
       analogWrite(11, LR);
       digitalWrite(BACK_BRAKE, LOW);
       digitalWrite(BACK_MOTOR, HIGH);
       analogWrite(3,UD);
      }else if( LR < 128 ){
       //GO LEFT 
       Serial.println("GOLEFT");
       digitalWrite(FRONT_BRAKE, LOW);
       digitalWrite(FRONT_MOTOR, LOW);
       analogWrite(11, 255-LR);
       digitalWrite(BACK_BRAKE, LOW);
       digitalWrite(BACK_MOTOR, HIGH);
       analogWrite(3,UD);
      }else{
        // GO STRAIGHT
        Serial.println("GOSTRAIGHT");
        digitalWrite(BACK_BRAKE, LOW);
        digitalWrite(FRONT_BRAKE, HIGH);
        digitalWrite(BACK_MOTOR, HIGH);
        analogWrite(3,UD);
      }
    }else if( UD < 128){
     //REVERSE 
       if( LR > 128 ){
         //reverse & TURN WHELE RIGHT
         Serial.println("GOREVERSE& RIGHT");
         digitalWrite(FRONT_BRAKE, LOW);
         digitalWrite(FRONT_MOTOR, HIGH);
         analogWrite(11, LR);
         digitalWrite(BACK_BRAKE, LOW); 
         analogWrite(3, 255-UD);
         digitalWrite(BACK_MOTOR, LOW);
       }else if( LR < 128){
         //reverse & TURN WHELE LEFT
         Serial.println("GOREVERSE&LEFT");
         digitalWrite(FRONT_BRAKE, LOW);
         digitalWrite(FRONT_MOTOR, LOW);
         analogWrite(11, 255-LR);
         digitalWrite(BACK_BRAKE, LOW); 
         analogWrite(3, 255-UD);
         digitalWrite(BACK_MOTOR, LOW);
       }else{
         //GO REVERSE
         Serial.println("GOREVERSE");
         digitalWrite(BACK_BRAKE, LOW); 
         analogWrite(3, 255-UD);
         digitalWrite(BACK_MOTOR, LOW);
       }
    }
    else{
     //STOP 
     Serial.println("STOP");
     digitalWrite(BACK_BRAKE, HIGH);
     digitalWrite(FRONT_BRAKE, HIGH);
    }
    
  }//else
  //digitalWrite(A13, LOW);
}//loop


void goBackward( int sec ){
  digitalWrite(BACK_BRAKE, LOW); 
  analogWrite(3, 200);  //activate drive motor w/ 200 speed
  digitalWrite(BACK_MOTOR, LOW);
  delay(sec); 
  //forwardAgain=false;
} //goBackward

void turnLeft(){
  digitalWrite(FRONT_BRAKE, LOW);
  digitalWrite(FRONT_MOTOR, LOW);
  analogWrite(11, 255);  //activate turn motor
} //turnLeft

void turnRight(){
  digitalWrite(FRONT_BRAKE, LOW);
  digitalWrite(FRONT_MOTOR, HIGH);
  analogWrite(11, 255);  //activate turn motor
}  //turnRight()

void goFORWARD(){
  //forWard=true;
  digitalWrite(BACK_BRAKE, LOW);
  digitalWrite(FRONT_BRAKE, HIGH);
  digitalWrite(BACK_MOTOR, HIGH);
} //goFORWARD

//void reverseAndGo_LEFT(int sec){
// delay(75);
// turnRight();
// goBackward(sec);
// STOP(); 
//}//reverseAndGo_LEFT
//
//void reverseAndGo_RIGHT(int sec){
// delay(75);
// turnLeft();
// goBackward(sec);
// STOP();
//} //reverseAndGo_RIGHT

void STOP(){
  digitalWrite(BACK_BRAKE, HIGH);
  digitalWrite(FRONT_BRAKE, HIGH);  //straight
} //STOP

void lcdSetup(){
  pinMode(LCD_BACKLIGHT,OUTPUT);
  digitalWrite(LCD_BACKLIGHT,HIGH);
  lcd.begin(16, 2);
  lcd.print("    WELCOME");
  lcd.setCursor(1,1);
  lcd.print("TO GROUND DRONE");
}//lcdSetup  

void carSetupAndStart(){
  pinMode(BACK_MOTOR,OUTPUT);
  pinMode(FRONT_MOTOR, OUTPUT);
  pinMode(BACK_BRAKE, OUTPUT);
  pinMode(FRONT_BRAKE, OUTPUT); 
}  //carSetupAndStart

