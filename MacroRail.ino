#define BLYNK_PRINT SwSerial
#define BLYNK_MSG_LIMIT 0
#include <SoftwareSerial.h>
SoftwareSerial SwSerial(10, 11); // RX, TX
#include <BlynkSimpleStream.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "32HTfa1ZP7Cw4sNYdYoCvixtjXBKw5YC"; /*
    defines pins numbers
*/
const int stepPin = 3;
const int dirPin = 4;

bool moveForward = false;
bool moveBackward = false;
const double pitch = 2;       // pitch in mm
const double stepAngle = 1.8; // angle per step
double distance = 0;
double numImages = 0;
bool runIt = false;
bool autoMode = false;
double startStep = 0;
double endStep = 0;
double currentStep = 0;

void terminalout(String out)
{
  Blynk.virtualWrite(V2, out + "\n");
}

void setup()
{
  SwSerial.begin(9600);
  /*
     Sets the two pins as Outputs
  */
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  Serial.begin(9600);
  Blynk.begin(Serial, auth);
}

void takeImage()
{
  // stabilise period
  delay(1000);
  // TODO : do shutter things
}

void loop()
{

  Blynk.run();

  doThings();
}

void doThings()
{
  if (moveForward)
  {
    move(true);
  }

  if (moveBackward)
  {

    move(false);
  }

  if (runIt)
  {
    runIt = false; // stop it running more than once
    runProcess();
  }
}

void move(bool forwards)
{
  if (forwards)
    digitalWrite(dirPin, HIGH);
  else
    digitalWrite(dirPin, LOW);

  digitalWrite(stepPin, HIGH);
  delay(10);
  digitalWrite(stepPin, LOW);
  delay(10);

  if (forwards)
    currentStep++;
  else
    currentStep--;
}

void runProcess()
{
  terminalout("Running process");
  if (autoMode)
  {
    terminalout("Auto mode");
    // check that start is before end
    if (startStep < endStep)
    {
      // we are ok to go
      double numberOfStepsTotal = endStep - startStep;
      double numberOfStepsPerShot = numberOfStepsTotal / numImages;
      // move the carriage to the start if it's not already there
      terminalout("Start Step = " + String(startStep,0));
      terminalout("End Step = " + String(endStep,0));
      terminalout("Number of images = " + String(numImages,0));
       
      while (currentStep > startStep)
      {
        move(false);
      }
      for (int i = 0; i < numImages; i++)
      {
        terminalout("Taking image " + String(i) + " at location " + String(currentStep,0));
        takeImage();
        terminalout("Image taken");
        for (int j = 0; j < numberOfStepsPerShot; j++)
        {
          move(true);
        }
        terminalout("Moved to next image");
      }
      terminalout("finished");
    }
  }

  else
  {
    terminalout("Manual Mode");
    // move forward
    digitalWrite(dirPin, HIGH);

    // calculate  amount to move per step
    double distanceToMove = distance / numImages;
    // so need to move "distance to move" numImages times
    // convert distance to move to steps
    double eachstep = pitch / (360.0 / stepAngle);
    double stepsPerDistance = distanceToMove / eachstep;

    for (int i = 0; i < numImages; i++)
    {
      takeImage();
      for (int j = 0; j < stepsPerDistance; j++)
      {
        move(true);
      }
    }
  }

  
}

BLYNK_CONNECTED()
{
  Blynk.syncAll();
}

/*
   move forward
*/
BLYNK_WRITE(V0)
{
  int pinValue = param.asInt();
  if (pinValue == 1)
  {
    moveForward = true;
  }
  else
  {
    moveForward = false;
  }
}

// move backwards
BLYNK_WRITE(V1)
{
  int pinValue = param.asInt();
  if (pinValue == 1)
  {
    moveBackward = true;
  }
  else
  {
    moveBackward = false;
  }
}

// distance
BLYNK_WRITE(V3)
{
  distance = param.asInt();
}

// num images
BLYNK_WRITE(V4)
{
  numImages = param.asInt();
}

// go button on image and distance
BLYNK_WRITE(V5)
{
  int pinValue = param.asInt();
  if (pinValue == 1)
  {
    runIt = true;
    terminalout("Runit\n");
  }
}

// manual or auto
BLYNK_WRITE(V6)
{
  int pinValue = param.asInt();
  if (pinValue == 1)
  {
    autoMode = true;
  }
  else
    autoMode = false;
}

// set start
BLYNK_WRITE(V7)
{
  int pinValue = param.asInt();
  if (pinValue == 1)
  {
    startStep = currentStep;
  }
}

// set end
BLYNK_WRITE(V8)
{
  int pinValue = param.asInt();
  if (pinValue == 1)
  {
    endStep = currentStep;
  }
}
