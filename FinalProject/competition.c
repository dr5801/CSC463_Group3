/**
 * Authors: Drew Rife, Brett Pare, Michael Pulliam
 * Professor: Dr. Girard
 * CSC 463
 * Final Project
 */

#define RIGHT_MOTOR 2
#define LEFT_MOTOR 0
#define GRAB_MOTOR 1
#define LEFT_IR 4 //NEED TO UPDATE THIS
#define RIGHT_IR 5 //NEED TO UPDATE THIS
#define FRONT_IR 7 //NEED TO UPDATE THIS
#define UPHILL 0
#define DOWNHILL 1
#define LEFT 0
#define RIGHT 1
#define TRUE 1
#define FALSE 0

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
int obtaining = FALSE; //current going to get a block
int gathered = FALSE; //true when obtained a block
int directionOfTravel = UPHILL;
int sideChosen = LEFT;
int clawClosed = FALSE;
int objectsDelivered = 0;

/**
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
        bk(RIGHT_MOTOR);
        fd(LEFT_MOTOR);
        break;
        case RIGHT:
        bk(LEFT_MOTOR);
        fd(RIGHT_MOTOR);
        break;
    }
    while((initialTime + 1.0) > seconds()); //Get off the line
    
    /*
     * Keep turning until we find another line
     */
    if(turnDirection == LEFT)
    {
	bk(RIGHT_MOTOR);
	fd(LEFT_MOTOR);
    }
    else
    {
	bk(LEFT_MOTOR);
	fd(RIGHT_MOTOR);
    }
    while(analog(!turnDirection) > 100);
    
    alloff();
}

/**
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

/**
 * Obtain values from the light sensors and save the values in an array 
 */
void getSensorValues()
{
    sensorValues[0] = (float)analog(LEFT_IR);
    sensorValues[1] = (float)analog(RIGHT_IR);
}

/**
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

/**
 * Calculate the resulting motor values
 */
void calculateMotorSpeeds()
{
    if(difference < 0.0) 
    {
        /* if the left sensor is reading a higher value than the right, calculate the speed for the left motor */
        motorSpeeds[LEFT] = DEFAULT_MOTOR_SPEED;
        motorSpeeds[RIGHT] = DEFAULT_MOTOR_SPEED * fuzzy;
    }
    else if(difference > 0.0)
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
 * moves the robot backward
 */
void moveBackward()
{
    getSensorValues();
    calculateDifference();
    claculateMotorSpeeds();

    motor(LEFT_MOTOR, ((int)motorSpeeds[0]) * -1);
    motor(RIGHT_MOTOR, ((int)motorSpeeds[1]) * -1);
}

/*
 * Checks both downward facing IR sensors, and if they both report a large value, that means we are at an intersection
 */
int checkIntersection()
{
    return (analog(LEFT_IR) > 150 && analog(RIGHT_IR) > 150);
}

/* 
 * returns true or false if one of the side sensors detects light from a block 
 */
int foundLight()
{
    return (analog(LEFT_IR) < 150 || analog(RIGHT_IR) < 150);
}

/*
 * returns the direction in which the light source is from
 */
int getDirectionForLight()
{
    if(analog(LEFT_IR) < 15)
    {
	return LEFT;
    }
    else
    {
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
	motor(GRAB_MOTOR, -40);
	currentTime = seconds();
	while((currentTime + 3) > seconds());
	alloff();
	clawClosed = FALSE;
	gathered = TRUE;
	transporting = TRUE;
    }
}

/*
 * closes the claw to obtain the block.
 * the claw closes until the block with the light source is reading a low enough value for the light sensor (the lower the value the closer it is)
 */
void closeClaw()
{
    if(clawClosed)
    {
	openClaw();
    }

    motor(GRAB_MOTOR, 40);
    while(analog(FRONT_IR) > 10);
    alloff();
    clawClosed = TRUE;
    gathered = TRUE;
    transporting = TRUE;
}

/*
 * obtains the object
 */
void obtainObject()
{
    obtaining = TRUE;
    turn(getDirectionForLight());
     
    //TODO: 30 might need to be changed for how close it gets the light
    while(analog(FRONT_IR) > 30)
    {
	    moveForward();
    }

    closeClaw();
}

/*
 * when the robot doesn't have an object, it need to keep move foward until it finds the intersection
 */
void notTransporting()
{
    if(directionOfTravel == UPHILL)
    {
	while(!checkIntersection())
	{
	    moveForward();
	}
    }
    else
    {
	while(!checkIntersection())
	{
            moveBackward();
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
void transporting()
{
    // object is obtained, move back to where you turned at the intersection
    while(!checkIntersection())
    {
	moveBackward();
    }
    alloff();

    // turn to reface forward towards block dropzone
    turn(!sideChosen);
    directionOfTravel = UPHILL;

    // TODO: wait until the light source changes -- value may need changed
    while(analog(FRONT_IR) < 30)
    {
	moveForward();
    }

    transporting = FALSE;
    gathered = FALSE;
    obtaining = FALSE;  
    objectsDelivered++; 
}

/**
 * main method to run the robot 
 */
int main() 
{   
    while(!stop_button())
    {
	while(objectsDelivered < 3)
	{
            if(!transporting)
	    {
                notTransporting();
	    }
	    else
	    {
	        transporting();
		turn(RIGHT);
		directionOfTravel = DOWNHILL;
	    }
	}

	//last object was just delivered so backup all the way to the start point
	moveBackward();
    }
}
