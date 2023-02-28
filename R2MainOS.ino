
#include "Arduino.h"
#include <SoftwareSerial.h>; 
#include <PololuMaestro.h>
#include "StringSplitter.h"
#include "DFRobotDFPlayerMini.h"

#define RADIO_RX 12 //connect to TX pin of HC-12
#define RADIO_TX 13 //connect to RX pin of HC-12
#define SERVO_RX 10 //connect to TX pin of Servo Controller
#define SERVO_TX 11 //connect to RX pin of Servo Controller
#define SOUND_RX 2 //connect to TX pin of DF Mini
#define SOUND_TX 3 //connect to RX pin of DF Mini
 
#define LED 12 
#define UpperUtil 0
#define LowerUtil 1
#define ProbDoor 2
#define DataProb 3
#define GripDoor 4
#define GripArm 5
#define GripJaw 6
#define DataPort 7
#define DomeMotor 8

SoftwareSerial ServoControllerSerial(SERVO_RX,SERVO_TX);
SoftwareSerial radioSerial(RADIO_RX,RADIO_TX);
SoftwareSerial PlayerSerial(SOUND_RX, SOUND_TX); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
int currentPlaying=0;
String strData="";
String cmd ="";
char c;
int lastDome=0;
MiniMaestro maestro(ServoControllerSerial);

void setup() {

radioSerial.begin(9600);
ServoControllerSerial.begin(9600);
PlayerSerial.begin(9600);
Serial.begin(9600);
delay(50);
myDFPlayer.begin(PlayerSerial);
myDFPlayer.volume(30);
myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
randomSeed(analogRead(0)); 


}

void loop() {

  static unsigned long timer = millis();
  if (millis() - timer > 45000) {
    timer = millis();
    processCMD("PlayRND");  //Play random mp3 every 45 second.
  }
  
  if(radioSerial.available())
  {
    do
    {
      c = radioSerial.read();
      if(c=="\n")
      {
        break;
       }
      strData.concat(c);
      
    }while (radioSerial.available()>0);
    
      Serial.println("Command from radio: "+strData);
      radioSerial.flush();  
      processCMD(strData);
      strData="";
      
  }

}
void processCMD (String pros_cmd)
{
  pros_cmd.trim();
  
  if(pros_cmd.length()<3) // no comands are less than 3 chr
  {
    return;  
  }
  
 if(pros_cmd.substring(0,3)=="DOM")
  {
    StringSplitter *splitter = new StringSplitter(pros_cmd, '&', 3);
    int itemCount = splitter->getItemCount();
    if(itemCount>1)
    {
      String item = splitter->getItemAtIndex(1);
      int domepos = item.toInt();
      if(domepos!=lastDome)
      {
        Serial.println("Sending new dome position");
        maestro.setTarget(DomeMotor,domepos);
        lastDome=domepos;
      }
    }
  }
 Serial.println("Processing command "+ pros_cmd);
  if(pros_cmd=="Util1Deploy")
  {
       maestro.setTarget(UpperUtil,863);
  }
  if(pros_cmd=="Util2Deploy")
  {
       maestro.setTarget(LowerUtil,863);
  }
  if(pros_cmd=="Util1Retract")
  {
       maestro.setTarget(UpperUtil,2146);
  }
  if(pros_cmd=="Util2Retract")
  {
       maestro.setTarget(LowerUtil,2159);
  }
  if(pros_cmd=="DeployProb")
  {
       maestro.setTarget(ProbDoor,1416);
       delay(100);
       maestro.setTarget(DataProb,2206);
  }
  if(pros_cmd=="RetractProb")
  {
      maestro.setTarget(DataProb,863);
      delay(100);
      maestro.setTarget(ProbDoor,1772);
  }
  if(pros_cmd=="DeployGrip")
  {
       maestro.setTarget(GripDoor,1322);
       delay(100);
       maestro.setTarget(GripArm,863);
  }
  if(pros_cmd=="RetractGrip")
  {
      maestro.setTarget(GripArm,2159);
      delay(100);
      maestro.setTarget(GripDoor,1724);
  }
  if(pros_cmd=="OpenGrip")
  {
      maestro.setTarget(GripJaw,1517);
      
  }
  if(pros_cmd=="CloseGrip")
  {
      maestro.setTarget(GripJaw,1919);
      
  }
   if(pros_cmd=="PlayRND")//PLAY RANDOM SOUND
  {
   int PlayNum = random(1,162);
   myDFPlayer.play(PlayNum); 
   return;
  }
  if(pros_cmd=="PlayNext") //Play Next
  {
     if(currentPlaying==162)
      {
        currentPlaying=0;
      } 
      currentPlaying++;
      myDFPlayer.play(currentPlaying) ;      
      return;
   }
   if(pros_cmd=="PlayPrev") //Play Prev;
   {
      if(currentPlaying==1 ||currentPlaying==0)
      {
        currentPlaying=163;
      } 
      currentPlaying--;
      myDFPlayer.play(currentPlaying) ;      
      return;
   }
    if(pros_cmd=="PlaySW") //Play Prev;
    {
      myDFPlayer.playFolder(1, 1); 
      return;
    }
    if(pros_cmd=="PlayStop") //Play Prev;
    {
      myDFPlayer.playFolder(5, 1); 
      return;
    }
     if(pros_cmd=="PlayCNT") //Play Cantina;
    {
      myDFPlayer.playFolder(4, 1);
      return; 
    }
     if(pros_cmd=="PlaySCM") //Play Screem;
    {
      myDFPlayer.playFolder(2, 1);
      return; 
    }
    if(pros_cmd=="PlayERR") //Play Screem;
    {
      myDFPlayer.playFolder(6, 1);
      return; 
    }
    if(pros_cmd=="PlaySAD") //Play SAD;
    {
       myDFPlayer.play(137) ;  
      return; 
    }
}
