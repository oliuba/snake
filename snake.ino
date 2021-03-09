#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <AlignedJoy.h>

Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);
#define PIN_JOY1_X   2  //(up  down)
#define PIN_JOY1_Y   1  //(left  right)
//#define PIN_BUTTON 2

AlignedJoy joystick_1(PIN_JOY1_X, PIN_JOY1_Y);

int x_location[1000];
int y_location[1000];
int apple_x;
int apple_y;
int lenght = 5;
char last = 'r';
long random_x;
long random_y;

bool is_on_field(){
  int x = x_location[0];
  int y = y_location[0];
  return ((x>=1) and (x<83) and (y>=1) and (y<47)) ;
}

bool check_pixel(int start, int x, int y){
  for (int i=start; i<lenght; i++){
    if ((x_location[i] == x) and (y_location[i] == y)){
      return false;
    }
  }
  return true;
}

void right(){
    x_location[0] += 1;
}

void left(){
    x_location[0] -= 1;
}

void up(){
    y_location[0] -= 1;
}

void down(){
    y_location[0] += 1;
}


void move(int x, int y){
  display.drawPixel(x_location[lenght-1], y_location[lenght-1], WHITE);
  
  for (int i=lenght-1; i>0; i--) {
    x_location[i] = x_location[i-1];
    y_location[i] = y_location[i-1];
  }
  
  if ((0 <= x) and (x <= 100) and (200 <= y) and (y <= 800) and (last != 'r')){
    last = 'l';
  }
  else if ((800 <= x) and (x <= 1024) and (400 <= y) and (y <= 600) and (last != 'l')){
    last = 'r';
  }
  else if ((300 <= x) and (x <= 800) and (800 <= y) and (y <= 1024) and (last != 'u')){
    last = 'd';
  } 
  else if ((300 <= x) and (x <= 800) and (0 <= y) and (y <= 100) and (last != 'd')){
    last = 'u';
  }
  
  switch (last){
    case 'u': up(); break;
    case 'd': down(); break;
    case 'l': left(); break;
    default: right();
    
  }
  display.drawPixel(x_location[0], y_location[0], BLACK);
  display.display();
}

void set_apple(){
  random_x = random(1, 83);
  random_y = random(1, 47);
  
  while (not (check_pixel(0, random_x, random_y))){
    random_x = random(1, 83);
    random_y = random(1, 47);
  }
  display.drawPixel(random_x, random_y, BLACK);
  apple_x = random_x;
  apple_y = random_y;  
}

void check_apple(){
  if ((x_location[0] == apple_x) and (y_location[0] == apple_y)){
    lenght ++;
    set_apple();
  }
}


void game_over(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(15, 15);
  display.print("GAME OVER");
  display.setCursor(15, 25);
  display.print("Score: ");
  display.print(lenght);
  display.display();
}


void setup(){
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP);
  
  display.begin();
  display.setContrast(40);
  display.clearDisplay();

  display.drawRect(0, 0, 84, 48, BLACK);
  
  randomSeed(analogRead(0));
  set_apple();

  attachInterrupt(0, reset_apple, FALLING);
  
  for (uint8_t i=0; i<lenght; i++) {
    x_location[i] = 42 - i;
    y_location[i] = 24;
    display.drawPixel(x_location[i], y_location[i], BLACK);
  }
  display.display(); 
  delay(200);
  
  int x, y;
  while (check_pixel(1, x_location[0], y_location[0]) and is_on_field()){
    x = joystick_1.read(X);
    y = joystick_1.read(Y);
    move(x,y);
    Serial.print(x);
    Serial.print(' ');
    Serial.println(y);
    check_apple();
    delay(200);
  }
  delay(300);
  game_over();
}

void reset_apple() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200)
  {
     display.drawPixel(apple_x, apple_y, WHITE);
     set_apple();
  }
  last_interrupt_time = interrupt_time;
}

void loop() {
  // put your main code here, to run repeatedly:

}
