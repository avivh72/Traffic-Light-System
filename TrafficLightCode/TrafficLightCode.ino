//Aviv Haber, Sepehr Hosseini
//January 16, 2017
//Traffic Light Intersection

#include <Servo.h> //import servo library

//ANALOG PINS
int detectorSensor = A0;
int LDRPin = A5;

//THRESHOLDS AND SERVO POSITION
int IRthresh;
int LDRthresh;
int pos = 0;

//VALUES FOR SENSOR READINGS
int IRval = 0;
int LDRValue = 0;
int previous;
int previous2;

//TIMED DELAYS
int servoDelay = 5;
int baseDuration = 2000; //yellow stays on for this long, red and green stays on for double this long unless button is pressed

//BOOLEANS
boolean isMoving = false; //stores whether servo is currently moving
boolean goingUp = true; //stores which direction servo is rotating
boolean buttonState = false; //stores whether button is being pressed
boolean isSped, toBeSped = false;
boolean pressed = false;

//PINS
int detectorPower = 13;
int servoPin = 12;
int LDRLED = 11;

int redMain = 10;
int yellowMain = 9;
int greenMain = 8;

int button = 7;

int redPerp = 6;
int yellowPerp = 5;
int greenPerp = 4;

int redPed = 3;
int greenPed = 2;

//TRAFFIC LIGHT ON/OFF STATES
int redMainState = LOW;
int yellowMainState = LOW;
int greenMainState = HIGH;

int yellowPerpState = LOW;
int redPerpState = HIGH;
int greenPerpState = LOW;

int redPedState = LOW;
int greenPedState = HIGH;

unsigned long time1, time2, time3; //time1 - unsused || time2 - servo  ||  time3 - traffic lights

Servo myservo; //servo object

//runs once at the beginning. sets up pins, sets up serial transmission, and initializes some variables
void setup()
{
  //PIN SETUP
  pinMode(yellowMain, OUTPUT);
  pinMode(greenMain, OUTPUT);
  pinMode(redMain, OUTPUT);
  pinMode (button, INPUT);
  pinMode(detectorPower, OUTPUT);
  pinMode(LDRLED, OUTPUT);
  pinMode(detectorSensor, INPUT);

  digitalWrite(detectorPower, HIGH); //GIVES POWER TO IR RECEVIER

  //TIMER INITIALIZATION
  time1 = millis();
  time2 = millis();
  time3 = millis();

  //SERVO INITIALIZTION
  myservo.attach (servoPin);
  myservo.write (pos);

  //SERIAL INITIALIZATION AND PRINTING INITIAL THRESHOLD FOR IR
  IRthresh = analogRead (detectorSensor)  - 500 + 1 ;
  LDRthresh = analogRead (LDRPin) - 40;

  Serial.begin(9600);
  Serial.print ("thresh:");
  Serial.println (IRthresh);
}

//runs onver and over again. controls flow of methods
void loop()
{
  readings ();

  debugging ();

  changeLight ();

  moveServo ();

  ldrLogic ();
}

//takes input vales for analog and digigtal pins. gets values for ldr, ir, and button
void readings ()
{
  previous = IRval;
  previous2 = LDRValue;
  IRval = analogRead(detectorSensor) - 500 + 0; //use the -500 and +0 as modifiers in case numbers are too high or threshold needs to increase
  LDRValue = analogRead(LDRPin);
  buttonState = digitalRead(button);
}

//prints various values to help me debug. sohuld be removed after done
void debugging()
{
  //DEBUGGING
  Serial.print (baseDuration);
  Serial.print ("           ");
  Serial.print (previous2);
  Serial.print ("           ");
  Serial.print (LDRthresh);
  Serial.print ("           ");
  Serial.println (LDRValue * 3);
}

//tests which lights are on and tests if button is being pressed and applies the appropriate light changes
void changeLight ()
{ 
  //pls explain logic
  //TRAFFIC LIGHT LOGIC
  if (millis() - time3 >= baseDuration)
  {
    if (greenMainState == HIGH)
    {
      greenMainState = LOW;
      yellowMainState = HIGH;
      greenPedState = LOW;
      redPedState = HIGH;
      baseDuration /= 2;
    }
    else if (yellowMainState == HIGH)
    {
      yellowMainState = LOW;
      redMainState = HIGH;
      greenPerpState = HIGH;
      redPerpState = LOW;
      baseDuration *= 2;
    }
    else if (greenPerpState == HIGH)
    {
      yellowPerpState = HIGH;
      greenPerpState = LOW;
      baseDuration /= 2;
    }
    else if (yellowPerpState == HIGH)
    {
      yellowPerpState = LOW;
      redMainState = LOW;
      greenMainState = HIGH;
      redPerpState = HIGH;
      redPedState = LOW;
      greenPedState = HIGH;
      baseDuration *= 2;
    }
    time3 = millis(); //reset timer
  }

  digitalWrite (greenMain, greenMainState);
  digitalWrite (yellowMain, yellowMainState);
  digitalWrite (redMain, redMainState);

  digitalWrite (greenPerp, greenPerpState);
  digitalWrite (yellowPerp, yellowPerpState);
  digitalWrite (redPerp, redPerpState);

  digitalWrite (redPed, redPedState);
  digitalWrite (greenPed, greenPedState);

  if (buttonState == HIGH && !pressed && !isSped)
  {
    if (redMainState == HIGH || yellowMainState == HIGH)
    {
      baseDuration /= 4;
      isSped = true;
    }
    else if (greenMainState == HIGH)
    {
      toBeSped = true;
    }
  }

  if (buttonState == LOW)
  {
    pressed = false;
  }

  if (greenMainState == HIGH && isSped)
  {
    isSped = false;
    baseDuration *= 4;
  }

  if (yellowMainState == HIGH && toBeSped && !isSped)
  {
    toBeSped = false;
    baseDuration /= 2;
    isSped = true;
  }
}


//checks if IR is being blocked and moves servo appropriately
void moveServo ()
{
  //IR LOGIC
  if ((IRval > IRthresh) && (previous >= IRthresh) && !isMoving)
  {
    isMoving = true;
  }

  if (isMoving && (millis() - time2 >= servoDelay))
  {
    if (goingUp)
    {
      pos++;
    }
    else
    {
      pos--;
    }
    if (pos >= 180 && goingUp)
    {
      goingUp = false;
      isMoving = false;
    }
    else if (pos <= 0 && !goingUp)
    {
      goingUp = true;
      isMoving = false;
    }

    myservo.write (pos);
    time2 = millis();
  }

  if (millis() - time2 >= 2000 && !goingUp && !isMoving)
  {
    isMoving = true;
    time2 = millis();
  }
}

//checks ambient light and sees whether to turn on streetlight
void ldrLogic ()
{
  //LDR LOGIC
  if (LDRValue < LDRthresh || previous2 < LDRthresh)
  {
    digitalWrite(LDRLED, HIGH);
  }
  else
  {
    digitalWrite(LDRLED, LOW);
  }
}

