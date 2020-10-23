
#include <Arduino.h>
// library 4 digit display
#include <TM1637Display.h>

// 4 digit display connection pins (Digital Pins)
#define CLK 7
#define DIO 6

// Sonar pins
#define trigPin 3
#define echoPin 2 

// buzzer
#define buzzer 11

// LED 
#define ledRed 12

// Buttons
#define buttonPinUp 8
#define buttonPinDown 9

// define variables sonar
float duration;
int distance;
int range;

// define variable with start value
int inside = 0;
int allowed = 5;
int i = 0;

// give params to library
TM1637Display display(CLK, DIO);
        
void setup()
{
  
  Serial.begin (9600);
  pinMode(buttonPinUp, INPUT);
  pinMode(buttonPinDown, INPUT);

  pinMode(ledRed, OUTPUT);
  pinMode(buzzer, OUTPUT);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  display.setBrightness(0x0f);
}

void loop()
{
  // alarm
  if (inside == allowed) {
    // LED on
    digitalWrite(ledRed, HIGH);
    delay(50);
  } else if (inside >= allowed) {
    digitalWrite(ledRed, HIGH);
    // alarm buz and blinking LED
    tone(buzzer, 500);
    delay(700);
    noTone(buzzer);
    digitalWrite(ledRed, LOW);
    delay(500);
  } else {
    digitalWrite(ledRed, LOW);
  }

  // increase allowed.
  if (digitalRead(buttonPinUp) == HIGH) {
    // turn LED on:
    allowed++;
    display.showNumberDec(allowed, false);
    delay(200);
  }
  
  // decrease allowed.
  if (allowed != 0 && digitalRead(buttonPinDown) == HIGH) {
    // turn LED on:
    allowed--;
    display.showNumberDec(allowed, false);
    delay(200);
  }

  // if increasing or decreasing in not on going
  if (digitalRead(buttonPinDown) != HIGH && digitalRead(buttonPinUp) != HIGH) {

    // sonar
    digitalWrite(trigPin, LOW); 
    delayMicroseconds(2); 
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10); 
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    //do calculation total distance
    distance = duration*0.034/2;
    delay(10);

    // if init distance is done.
    if (i == 5) {
        // if inside value is not 0 and distance is between half of the range and normal range.
        if  (inside != int(0) && distance < range && distance > range/2) {
          inside--;
          delay(1500);
        } 
        delay(10);
        // if distance is smaller then middlepoint range
        if (distance < range/2) {
          inside++;
          delay(500);
        } 
        // show total people inside
        display.showNumberDec(inside, false);
    } else {
      // check what the average distance is between PeopleCounter and wall due inaccurate sonar.
      i++;
      range += distance;
      display.showNumberDec(range, false); // Expect: _301
      if (i == 5){
        // calculate average and then do a offset, because of inaccurate sonar.
        range = (range/5) * 0.90;
        // higher then 250 not allowed due inaccurate readings at that distance
        if(range > 250) { 
          range = 220;
        }
      }
    }
  }
}
