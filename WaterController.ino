/* Andrew Campbell
    Final Project: Water Controller
    CSIS 2810
*/
// including libraries
#include <Wire.h>
#include "RTClib.h"
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4

Adafruit_SSD1306 display(OLED_RESET);// resetting screen
RTC_DS1307 RTC;
//initializing variables/counters
int page = 0;
int lClick = 0;
int mClick = 0;
int rClick = 0;
int counter = 0;
int counterMin = 0;
int water = 0;
int onOff = 0;

void setup () {
  //Starting screen
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(1500);
  display.clearDisplay();
  Wire.begin();
  RTC.begin();

  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}
void loop () {
  DateTime now = RTC.now();
  // setting up pins
  pinMode(7, OUTPUT);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);

  if (page == 0) { // Showing time page
    refresh();
    display.setTextSize(2);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(20, 4);
    display.print(now.hour(), DEC);
    display.print(':');
    display.print(now.minute(), DEC);
    display.print(':');
    display.print(now.second(), DEC);
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(35, 22);
    display.print(now.month(), DEC);
    display.print('/');
    display.print(now.day(), DEC);
    display.print('/');
    display.print(now.year(), DEC);
    display.display();
  }
  if (page == 1) { // second screed high lighting hour
    refresh();
    header();
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 11); display.print(" >Hour: ");
    display.setCursor(0, 20);  display.print (" Minute:");
    display.display();
  }
  if (page == 2 ) { // second screen high lighting minute
    refresh();
    header();
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 11); display.print(" Hour: ");
    display.setCursor(0, 20);  display.print (" >Minute:");
    display.display();
    lClick = 1;
  }
  if (page == 3 ) { // screen to change hour
    refresh();
    display.setCursor(22, 0);  display.print("   Set Hour");
    display.drawLine (0, 9, 128, 9, WHITE);
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 11); display.print("Hour: ");  display.print(counter);
    display.display();
    mClick --;
  }
  if (page == 4 ) { // screen to change min
    refresh();
    display.setCursor(22, 0);  display.print("  Set Minute");
    display.drawLine (0, 9, 128, 9, WHITE);
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 11); display.print("Minute: ");  display.print(counterMin);
    display.display();
  }
  if (page == 5 ) { // checking set time
    refresh();
    display.setCursor(22, 0);  display.print("  The Set Time ");
    display.drawLine (0, 9, 128, 9, WHITE);
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(0, 11); display.print(counter); display.print(" : "); display.print(counterMin);
    display.display();
    delay(1000);
  }
  if (!digitalRead(12)) { //adding a counter to water when button is clicked
    water++;
  }
  if (water == 1) { // turning on water manually with a button
    waterOn();
  }
  if (water == 2) { // turning off water manually with a button
    waterOff();
    water = 0;
  }
  if (!digitalRead(8)) { // adding counter when button is pressed
    lClick ++;
  }
  if (!digitalRead(9)) { // adding counter when button is pressed
    mClick ++;
  }
  if (!digitalRead(10)) { // adding counter when button is pressed
    rClick ++;
  }
  if (lClick == 0 && rClick == 0 && mClick == 0) { // taking you to time display page
    page = 0;
  }
  if (lClick == 1 && rClick == 0 && mClick == 0) { // taking you to time water on page
    page = 1;
  }
  if (lClick == 1 && rClick == 0 && mClick == 1) { // taking you to set hour page
    page = 2;
  }
  if (lClick == 1 && rClick == 0 && mClick == 2) { // taking you back to second screen high lighting hour
    page = 1;
    mClick = 0;
  }
  if (lClick == 2 && rClick == 0 && mClick == 0) { // taking you to time display page
    page = 0;
    lClick = 0;
    mClick = 0;
  }
  if (lClick == 1 && mClick == 0 && rClick == 1) { // going to page 3 set hour
    page = 3;
  }
  if (!digitalRead(9) &&  rClick == 1 && mClick < 1 ) { //adding to counter hour checking if less than 24
    counter ++;
    if (counter == 24) {
      counter = 0;
    }
  }
  if (!digitalRead(10) &&  rClick == 2) { //going back to page 1
    lClick = 1;
    mClick = 0;
    rClick = 0;
    page = 1;
  }
  if (lClick == 1 && mClick == 1 && rClick == 1) { // go to set minute
    page = 4;
  }
  if (!digitalRead(9) &&  mClick > 1 && rClick == 1) { //adding to counterMinut
    counterMin++;
    counterMin * 10;
    if (counterMin == 60) {
      counterMin = 1;
    }
  }
  if (!digitalRead(10) &&  mClick == 2) { //going back to page 1
    lClick = 1;
    mClick = 0;
    rClick = 0;
    page = 1;
  }
  if (!digitalRead(9) && lClick == 0 ) { // checking alarm time
    mClick = 0;
    rClick = 0;
    page = 5;
  }/////////////////////////////////////////
  if (now.hour() == counter && now.minute() == counterMin && lClick == 0 && onOff == 0) { // turning on water at set time, setting on off counter to avoid it staying on when button pressed
    digitalWrite(7, HIGH);
    onOff = 1;
  }
  if ( now.minute() != counterMin) { // changing the counter to 0 so alarm can trigger
    onOff = 0;
  }
  if (now.minute() == 0 | now.minute() == 15  | now.minute() == 10 | now.minute() == 45) { // turning off water if on every 15 minutes
    digitalWrite(7, LOW);
    water = 0;
    onOff = 0;
  }
  if (!digitalRead(12) && water == 2) { // turning off water manually when its on.
    digitalWrite(7, LOW);
    water = 0;
  }

  //////////////////////////////////////////////////////////////
  // used for troubleshoting, checking the counters. ///////////
  Serial.print(" //Left Click: "); Serial.print(lClick);
  Serial.print("  //mClick: "); Serial.print(mClick);
  Serial.print(" //rClick: "); Serial.print(lClick);
  Serial.print(" //Counter: "); Serial.print(counter);
  Serial.print(" //CounterMin: "); Serial.print(counterMin);
  Serial.print(" //Water: "); Serial.print(water);
  Serial.print(" //onOff: "); Serial.print(onOff);
  ///////////////////////////////////////////////////////////////
}
void refresh() // clearing screen for next one
{
  display.display();
  delay(00);
  display.clearDisplay();
}
void header() // setting the header up for time water on
{
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(22, 0);  display.print("Time Water On");
  display.drawLine (0, 9, 128, 9, WHITE);
}
void waterOn()// turning water on
{
  digitalWrite(7, HIGH);
}
void waterOff() //turning water off
{
  digitalWrite(7, LOW);
}
