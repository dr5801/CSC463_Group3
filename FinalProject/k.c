/*
 * Authors: Drew Rife, Brett Pare, Michael Pulliam
 * Professor: Dr. Girard
 * CSC 463
 * Final Project
 */

#define RIGHT_MOTOR 1
#define LEFT_MOTOR 0
#define GRAB_MOTOR 3
#define LEFT_IR 3 //NEED TO UPDATE THIS
#define RIGHT_IR 4 //NEED TO UPDATE THIS
#define LEFT_LIGHT 2
#define RIGHT_LIGHT 6
#define FRONT_LIGHT 5 //NEED TO UPDATE THIS
#define UPHILL 0
#define DOWNHILL 1
#define LEFT 0
#define RIGHT 1
#define TRUE 1
#define FALSE 0
#define NUM_INTERSECTIONS 5 // number of intersections on the board

///////////////////////////////////////////
#define DEFAULT_MOTOR_SPEED 80.0
#define MOTOR_RATIO 2.0

float sensorValues[2];
float motorSpeeds[2];
float difference;
float differencePercentage;
float fuzzy;
///////////////////////////////////////////

int transporting = FALSE;
int directionOfTravel = UPHILL;
int sideChosen = LEFT;
int clawClosed = FALSE;
int objectsDelivered = 0;
int intersection = 0;
int atDropZone = FALSE;
int terminate = FALSE;

/*
 * Turns the robot in the specific direction for a brief time while ignoring downward IR sensors, but stops when it hits a line again
 * TL;DR switches from one line to another in the specified direction
 */
void turn(int turnDirection)
{
    float initialTime = seconds();
    float timeElapsed;
    sideChosen = turnDirection;
    switch(turnDirection)
      {
        case LEFT:
          motor(RIGHT_MOTOR, 90);
        motor(LEFT_MOTOR, -15);
        break;
        case RIGHT:
          motor(LEFT_MOTOR, 90);
        motor(RIGHT_MOTOR, -15);
        break;
    }
    while((initialTime + 1.75) > seconds()); //Get off the line
    
    if( turnDirection == LEFT )
      {
        while(analog(RIGHT_IR) < 180);
    }
    else
      {
        while(analog(LEFT_IR) < 180);
    }
    
    alloff();
}

/*
 * turns the robot 180 degrees
 */
void turn180(int turnDirection)
{
    float time;
    switch(turnDirection)
      {
        case LEFT:
          motor(RIGHT_MOTOR, 80);
        motor(LEFT_MOTOR, -80);
        break;
        case RIGHT:
          motor(LEFT_MOTOR, 80);
        motor(RIGHT_MOTOR, -80);
        break;
    }
    time = seconds();
    while((time + 2.5) > seconds());
    
    if( turnDirection == LEFT )
      {
        while(analog(RIGHT_IR) < 180);
    }
    else
      {
        while(analog(LEFT_IR) < 180);
    }
    
}

/*
 * Calculate the absolute value of an input floating point value
 */
float abs(float input)
{
    if(input < 0.0)
      {
        return input * (-1.0);
    }    
    else
      {
        return input;
    }
}

/*
 * Obtain values from the light sensors and save the values in an array 
 */
void getSensorValues()
{
    sensorValues[0] = (float)analog(LEFT_IR);
    sensorValues[1] = (float)analog(RIGHT_IR);
}

/*
 * Find the difference between the sensor values
 */
void calculateDifference()
{
    float max_sensor_reading = 255.0;
    difference = sensorValues[0]-sensorValues[1];
    differencePercentage = abs(difference) / max_sensor_reading;
    
    /* generate a fuzzy value */
    fuzzy = 1.0 - (abs(difference)/255.0) * MOTOR_RATIO;
}

/*
 * Calculate the resulting motor values
 */
void calculateMotorSpeeds()
{
    if(difference > 0.0) 
      {
        /* if the left sensor is reading a higher value than the right, calculate the speed for the left motor */
        motorSpeeds[LEFT] = DEFAULT_MOTOR_SPEED;
        motorSpeeds[RIGHT] = DEFAULT_MOTOR_SPEED * fuzzy;
    }
    else if(difference < 0.0)
      {
        /* if the left sensor is reading a lower value than the right, calculate the speed for the right motor */
        motorSpeeds[RIGHT] = DEFAULT_MOTOR_SPEED;
        motorSpeeds[LEFT] = DEFAULT_MOTOR_SPEED * fuzzy;
    }
    else
    {
        /* on the black line, keep going straight */
        motorSpeeds[LEFT] = DEFAULT_MOTOR_SPEED;
        motorSpeeds[RIGHT] = DEFAULT_MOTOR_SPEED;
    }
    
}

/*
 * moves the robot forward
 */
void moveForward()
{
    getSensorValues();
    calculateDifference();
    calculateMotorSpeeds();
    
    motor(LEFT_MOTOR,(int)motorSpeeds[0]);
    motor(RIGHT_MOTOR,(int)motorSpeeds[1]);
}

/*
 * Checks both downward facing IR sensors, and if they both report a large value, that means we are at an intersection
 */
int checkIntersection()
{
    return ((analog(LEFT_IR) > 200) && (analog(RIGHT_IR) > 200));
}

/* 
 * returns true or false if one of the side sensors detects light from a block 
 */
int foundLight()
{
    return (analog(LEFT_LIGHT) < 230 || analog(RIGHT_LIGHT) < 230);
}

/*
 * returns the direction in which the light source is from
 */
int getDirectionForLight()
{
    if(analog(LEFT_LIGHT) < analog(RIGHT_LIGHT))
      {
        return LEFT;
    }
    else
      {
        beep();
        return RIGHT;
    }
}

/*
 * opens the claw if it was closed
 * TODO: probably need to adjust time of this open**********
 */
void openClaw()
{
    float currentTime;
    if(clawClosed)
      {
        // ******* I wanted the motor speed to be slow at start to make sure it isn't opening it up too fast ...... seconds can also be adjusted ********
        motor(GRAB_MOTOR, 30);
        currentTime = seconds();
        while((currentTime + 1.0) > seconds());
        alloff();
        clawClosed = FALSE;
        transporting = TRUE;
    }
}

/*
 * closes the claw to obtain the block.
 * the claw closes until the block with the light source is reading a low enough value for the light sensor (the lower the value the closer it is)
 */
void closeClaw()
{
    float time;
    if(clawClosed)
      {
        openClaw();
    }
    
    motor(GRAB_MOTOR, -30);
    time = seconds();
    while((time + 1.0) > seconds());
    alloff();
    clawClosed = TRUE;
    transporting = TRUE;
}

/*
 * obtains the object
 */
void obtainObject()
{
    turn(getDirectionForLight());
    
    //TODO: 30 might need to be changed for how close it gets the light
    while(analog(FRONT_LIGHT) > 100)
      {
        moveForward();
    }
    alloff();
    
    closeClaw();
}

/*
 * when the robot doesn't have an object, it need to keep move foward until it finds the intersection
 */
void notTransporting()
{
    float time;
    while(!checkIntersection())
      {
        moveForward();
    }
    
    alloff();
    if(directionOfTravel == UPHILL)
      {
        intersection++;
    }
    else
      {
        if(!atDropZone)
          {
            intersection--;
        }
        else
          {
            atDropZone = FALSE;
        }
    }
    
    if(foundLight())
      {
        obtainObject();
    }
}

/*
 * object is obtained and the robot is officially transporting the block now
 */
void transportingObject()
{
    float time;
    
    turn180(RIGHT);
    
    // object is obtained, move back to where you turned at the intersection
    while(!checkIntersection())
      {
        moveForward();
    }
    alloff();
    
    // turn to reface forward towards block dropzone
    turn(sideChosen);
    directionOfTravel = UPHILL;
    
    /*
     * have the robot move forward until it gets the last intersection (drop zone)
     */
    while(intersection != NUM_INTERSECTIONS)
      {
        while(!checkIntersection())
          {
            moveForward();
        }
        
        intersection++;
        time = seconds();
        while((time + 1.0) > seconds());
        printf("Intersection: %d\n", intersection);
    }
    
    // found the drop off, move forward a tiny bit
    atDropZone = TRUE;
    moveForward();
    time = seconds();
    while((time + 1.0) > seconds());
    alloff();
    
    openClaw();
    
    transporting = FALSE;  
    objectsDelivered++; 
    
    while(!checkIntersection())
      {
        backUp();
    }
    alloff();
}

/*
 * backups the robot
 */
void backUp()
{
    bk(LEFT_MOTOR);
    bk(RIGHT_MOTOR);
}

/*
 * a separate process that monitors the stop button to indicate when
 * the program should be terminated
 */
void monitorStopButton()
{
    while(!terminate)
      {
        terminate = stop_button();
    }
    alloff();
}


/**
 * main method to run the robot 
 */
int main() 
{   
    int pid = start_process(monitorStopButton());
    while(!start_button());
    
    while(objectsDelivered < 3 && !terminate)
      {
        if(!transporting)
          {
            notTransporting();
        }
        else
          {
            transportingObject();
            directionOfTravel = DOWNHILL;
        }
    }
    
    if(!terminate)
      {
        //last object was just delivered so backup all the way to the start point
        turn(RIGHT);
        moveForward();
    }
    
    kill_process(pid);
    return 0;
}
