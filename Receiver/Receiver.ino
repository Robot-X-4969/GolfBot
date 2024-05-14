#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
/* Values in clockwise order of buttons starting at 9 o'clock
0 - TURN LEFT
1 - DRIVE
2 - SPEED UP
3 - TURN RIGHT
4 - SLOW DOWN
5 - REVERSE
6 - TOGGLE INTAKE
7 - INTAKE UP
8 - PUSH PUTTER
9 - LAUNCH
10 - PULL PUTTER
11 - INTAKE DOWN
*/



const byte thisAdress[5] = {'R', 'o', 'b', 'o', 'X'}; //Must be same as transmitter
RF24 radio(2, 3); //CE, CSN EDIT #'s

char thisData[36]; //data before updating; 3 instances of data
char data[13]; //updated data
int thisTick = 0;

unsigned long dcTime;
unsigned long pressTime1;//drive
unsigned long pressTime2;//manipulators



Servo wormServo;
Servo intakeServo;

bool safeDrive;
bool connection = false;

double speed = .5; 


void setup(){

  pinMode(4, OUTPUT);

  wormServo.attach(9); //EDIT #
  wormServo.write(0.65*180); //Default pos

  intakeServo.attach(6); //EDIT #
  intakeServo.write(0.7*180);

  radio.begin();
  radio.setDataRate( RF24_250KBPS );
  radio.openReadingPipe(1, thisAdress); //#, adress
  radio.startListening();

  for(int i = 22; i < 40; i++){
    pinMode(i, OUTPUT);
  }
}

void loop(){
  getData(); 
  if(data[0] != thisTick){
    thisTick = data[0];
  if(connection){ //will shut off if not receiving signal from transmitter

    //Safety Stop
    safeDrive = true;

    if(wasPressed(9)){
      setEngage(false, 500);
    }
    if(wasPressed(6)){
      toggleIntakeShaft();
    }

    //Speed controllers 
    if(wasPressed(2)){
      changeSpeed(0.1);
    } else if(wasPressed(4)){
      changeSpeed(-0.1);
    }

    //Driver Handling
    if(isDown(1)){
      Drive(speed);
    } else if(isDown(5)){
      Drive(-speed);
    } else if(isDown(0)){
      Turn(-speed);
    } else if(isDown(3)){
      Turn(speed);
    } else {
      Drive(0);
    }

    if(wasPressed(7)){
      Intake(1);
    } else if(wasPressed(11)){
      Intake(-1);
    } else {
      Intake(0);
    }

    if(wasPressed(8)){
      pullPutter(1);
    } else if(wasPressed(10)){
      pullPutter(-1);
    } else{
      pullPutter(0);
    }
    updateMovement();
  } else {
    shutOff();
  }
}
}

void getData(){
  if( radio.available() ) {
    for(int i = 23; i > -1; i--){
      thisData[i+12] = thisData[i];
    }
    for(int i = 1; i < 13; i++){
      thisData[i-1] = data[i];
    }
    radio.read( &data, sizeof(data));
    dcTime = millis();
    connection = true;
  } else {
    if(dcTime + 1500 <= millis()){ //No signal for 3 seconds
      connection = false;
    }
  }
}

bool isDown(int button){ //! for isUp
  if(data[button+1] == 1){
    return true;
  }
  return false;
}

bool wasPressed(int button){
  if((thisData[button+12] == 0)&&(thisData[button] == 0)&&(data[button+1] == 1)){
    return true;
  }
  return false;
}

