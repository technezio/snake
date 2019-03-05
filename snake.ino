/*
 * Simple remake of the classic Snake game, using Arduino and
 * an LED matrix, optimized for MAX7219 driver.
 * By GLGPrograms
 * GPL-3.0
*/

#include <ledMatrix.h>
#include "snake_class.h"

ledMatrix myMatrix(8,10,12);  // Data, Latch, Clock

Snake mySnake;

#define PIN_X_AXIS A1
#define PIN_Y_AXIS A2

#define TIME_STEP 500

boolean eaten = true;
char val;
byte x;
byte y;
int joyX;
int joyY;
unsigned long int old_timestamp, new_timestamp;

void setup()
{
  Serial.begin(9600);
  myMatrix.setBrightness(2);
  randomSeed( analogRead(A0) );
}

void loop()
{
  val = '0';
  if( Serial.available() > 0 )
  {
    val = Serial.read();
    // Flush the buffer
    while( Serial.available() )
      Serial.read();
  }

  // Read joystick
  joyX = analogRead( PIN_X_AXIS );
  joyY = analogRead( PIN_Y_AXIS );

  // Evaluate serial command
  switch(val)
  {
  case 'w':
    mySnake.changeDir(UP);
    break;
  case 'a':
    mySnake.changeDir(LEFT);
    break;
  case 's':
    mySnake.changeDir(DOWN);
    break;
  case 'd':
    mySnake.changeDir(RIGHT);
    break;
  }

  // Evaluate joystick position
  // WARNING!! Bigger X is right, bigger Y is down
  if( joyX > 600 ) mySnake.changeDir(RIGHT);
  else if( joyX < 400 ) mySnake.changeDir(LEFT);
  else if( joyY > 600 ) mySnake.changeDir(DOWN);
  else if( joyY < 400 ) mySnake.changeDir(UP);
  joyX = joyY = 500;

  // Next move
  new_timestamp = millis();
  if( (new_timestamp - old_timestamp) > TIME_STEP ) {
    mySnake.move();
    old_timestamp = new_timestamp;
  }
  
  // If apple has been eaten, draw new one
  if( eaten )
  {
    mySnake.newApple();
    eaten = false;
  }
  // else check if the snake eats it
  else if ( mySnake.eat() )
  {
    eaten = true;
    mySnake.append();
  }

  snakePrint(mySnake);
  delay(10);
}

void snakePrint(const Snake& s)
{
  Snake::Dot *tmp = s.head;
  // Clear matrix
  myMatrix.flush();
  // Print snake
  for (byte i = 0; i < s.lenght; i++)
  {
    myMatrix.dot(tmp -> x, tmp -> y);
    tmp = tmp -> next;
  }
  // Print apple
  myMatrix.dot(s.apple[0], s.apple[1]);
  // Actually display matrix content
  myMatrix.load();
}
