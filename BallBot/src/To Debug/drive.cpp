//
//  drive.cpp
//  
//
//  Created by Sam Burton on 03/11/2018.
//

#include "drive.h"
#include <cmath>

// DEFINES FOR AUTON MODES
#define DRIVEMODE_USER 0
#define DRIVEMODE_TIME -1
#define DRIVEMODE_DIST -2
#define DRIVEMODE_CUSTOM -3
#define DRIVEMODE_TURN 1
#define TURNMODE_GYRO 1
#define TURNMODE_ENCODER 2


// CONSTRUCTOR

Drive::Drive() {
    
    // CONFIG
    driveBrain = NULL;
    
    // DRIVER
    slewRate = 1;
    deadZone = 0;
    arcadeMode = false;
    controlsSet = false;
    leftSpeed = 0;
    rightSpeed = 0;
    
    // POSITION TRACKING
    targetX = 0;
    targetY = 0;
    targetS = 0;
    xPosition = 0;
    yPosition = 0;
    lastRightEnc = 0;
    lastLeftEnc = 0;
    trackingTicksPerTile = 1;
    trackingTicksPerDegree = 1;
    
    // STATE
    direction = 0;
    temperature = 0;
    power = 0;
    current = 0;
    
    // AUTON TUNING
    ticksPerTile = 1;
    ticksPerDegree = 1;
    autoTimeOut = 0;
    turnAccepted = 1;
    turnRate = 1;
    turnPulse = 0;
    pulseTime = 1;
    pulsePause = 0;
    minSpeed = 0;
    maxTurn = 100;
    minForward = 0;
    
    // AUTON FUNCTION FLAGS
    rightPower = 0;
    leftPower = 0;
    targetDirection = 0;
    distance = 0;
    targetDistance = 0;
    autoSpeed = 0;
    autoTime = 0;
    autoMode = DRIVEMODE_USER;
    autonComplete = false;
    currentTime = 0;
    recordedTime = 0;
    leftRunSpeed = 0;
    rightRunSpeed = 0;
    recordedDistLeft = 0;
    recordedDistRight = 0;
    turnMode = TURNMODE_GYRO;
    lastAngle = 0;
    drivingToPos = false;
    
}

~Drive::Drive() {
    
}


// CONFIG

void Drive::setBrain(vex::brain* b) {
    driveBrain = b;
}
void Drive::addMotorLeft(vex::motor* m) {
    motorsLeft.push_back(m);
}
void Drive::addMotorRight(vex::motor*) {
    motorsRight.push_back(m);
}


// DRIVER PREFERENCES

void Drive::setSlewRate(double r) {
    slewRate = r;
}
void Drive::setDeadZone(double d) {
    deadZone = d;
}
void Drive::setTankJoy(vex::controller::axis* l,vex::controller::axis* r) {
    arcadeMode = false;
    leftSideJoy = l;
    rightSideJoy = r;
    controlsSet = true;
}
void Drive::setArcadeJoy(vex::controller::axis* p,vex::controller::axis* t) {
    arcadeMode = true;
    powerJoy = p;
    turnJoy = t;
    controlsSet = true;
}


// POSITION TRACKING

void Drive::setPosition(double x, double y, double d) {
    xPosition = x;
    // lastRightEnc = getRightEnc();
    yPosition = y;
    // lastLeftEnc = getLeftEnc();
    direction = d;
}
void Drive::setDirection(double d) {
    direction = d;
}
void Drive::setTrackingTicksPerTile(double t) {
    trackingTicksPerTile = t;
}
void Drive::setTrackingTicksPerDegree(double t) {
    trackingTicksPerDegree = d;
}
void Drive::trackPosition() {
    double leftDiff = getLeftEnc() - lastLeftEnc;   // Find encoder changes
    double rightDiff = getRightEnc() - lastRightEnc;
    
    double angleChange = (rightDiff - leftDiff)/2;  // Find angle change
    angleChange *= trackingTicksPerDegree;
    
    double distChange = (leftDiff + rightDiff)/2;   // Find lin. dist change
    distChange *= trackingTicksPerTile;
    
    direction += angleChange;   // Find cumulative direction
    xPosition += distChange * cos(direction * M_PI / 180);  // find cumulative xPos
    yPosition += distChange * sin(direction * M_PI / 180);  // find cumulative yPoS
}


// STATE GETTERS

double Drive::getDirection() {
    return direction;
}
double Drive::getRightEnc() {
    double tot = 0;
    for (int i = 0; i < motorsRight.size(); i++) {
        tot += motorsRight[i]->position(vex::positionUnits::raw);
    }
    tot/=(motorsLeft.size()+motorsRight.size());
    return tot;
}
double Drive::getLeftEnc() {
    double tot = 0;
    for (int i = 0; i < motorsLeft.size(); i++) {
        tot += motorsRight[i]->position(vex::positionUnits::raw);
    }
    tot/=(motorsLeft.size()+motorsLeft.size());
    return tot;
}
double Drive::getTemperature() {
    double tot = 0;
    for (int i = 0; i < motorsLeft.size(); i++) {
        tot += motors[i]->temperature(vex::percentUnits::pct);
    }
    for (int i = 0; i < motorsRight.size(); i++) {
        tot += motors[i]->temperature(vex::percentUnits::pct);
    }
    temperature = tot/(motorsLeft.size()+motorsRight.size());
    return temperature;
}
double Drive::getPower() {
    double tot = 0;
    for (int i = 0; i < motorsLeft.size(); i++) {
        tot += motors[i]->power(vex::powerUnits::watt);
    }
    for (int i = 0; i < motorsRight.size(); i++) {
        tot += motors[i]->power(vex::powerUnits::watt);
    }
    power = tot/(motorsLeft.size()+motorsRight.size());
    return power;
}
double Drive::getCurrent() {
    double tot = 0;
    for (int i = 0; i < motorsLeft.size(); i++) {
        tot += motors[i]->current(vex::currentUnits::amp);
    }
    for (int i = 0; i < motorsRight.size(); i++) {
        tot += motors[i]->current(vex::currentUnits::amp);
    }
    current = tot/(motorsLeft.size()+motorsRight.size());
    return current;
}


// SETTERS FOR AUTON TUNING

void Drive::setTicksPerTile(double t) {
    ticksPerTile = t;
}
void Drive::setTicksPerDegree(double t) {
    ticksPerDegree = t;
}
void Drive::setMinForwardSpeed(double d) {
    minForward = d;
}
void Drive::setTimeOut(double t) {
    autoTimeOut = t;
}
void Drive::setTurnMode(int m) {
    turnMode = m;
}
void Drive::setTurnAccepted(double t) {
    turnAccepted = t;
}
void Drive::setTurnRate(double t) {
    turnRate = t;
}
void Drive::setPulseTime(double p) {
    pulseTime = p;
}
void Drive::setPulsePause(double p) {
    pulsePause = p
}
void Drive::setMinTurnSpeed(double m) {
    minSpeed = m;
}
void Drive::setMaxTurnSpeed(double m) {
    maxTurn = m;
}


// AUTON FUNCTIONS

void Drive::driveTime(double s, double d, double t) {
    // speed, direction, distance, time
    autoSpeed = s;
    autoMode = DRIVEMODE_TIME;
    autoTimeOut = t;
    targetDirection = d;
    recordedTime = driveBrain->timer(vex::timeUnits::msec);
}
void Drive::driveDist(double s, double dir, double dist, double t = 10) {
    // speed, direction, distance, timeout
    autoSpeed = s;
    targetDirection = dir;
    autoMode = DRIVEMODE_DIST;
    autoTimeOut = t;
    recordedTime = driveBrain->timer(vex::timeUnits::msec);
    recordedDistLeft = 0;
    for (int i = 0; i < motorsLeft.size(); i++) {
        recordedDistLeft += motorsLeft[i]->rotation(vex::rotationUnits::deg);
    }
    recordedDistLeft /= motorsLeft.size();
    recordedDistRight = 0;
    for (int i = 0; i < motorsRight.size(); i++) {
        recordedDistRight += motorsRight[i]->rotation(vex::rotationUnits::deg);
    }
    recordedDistRight /= motorsRight.size();
    
    targetDistance = dist * ticksPerTile; + (recordedDistRight + recordedDistLeft)/2
    
}
void Drive::driveCustom(double s, double d, double t = 10) {
    // speed, direction, timeout
    recordedTime = driveBrain->timer(vex::timeUnits::msec);
    autoSpeed = s;
    autoMode = DRIVEMODE_CUSTOM;
    autoTimeOut = t*1000;
    targetDirection = d;
}
void Drive::turnTo(double a, double t = -1) {
    // angle, timeout
    recordedTime = driveBrain->timer(vex::timeUnits::msec);
    targetDirection = a;
    autoTimeOut = t*1000;
    autoMode = DRIVEMODE_TURN;
    turnMode = TURNMODE_GYRO;
}
void Drive::turnRelative(double a, double t = -1) {
    // angle, timeout
    recordedTime = driveBrain->timer(vex::timeUnits::msec);
    targetDirection = direction + a;
    autoTimeOut = t*1000;
    autoMode = DRIVEMODE_TURN;
    turnMode = TURNMODE_GYRO;
}
void Drive::turnRelativeEncoder(double a, double t = -1) {
    // angle, timeout
    recordedTime = driveBrain->timer(vex::timeUnits::msec);
    targetDirection = direction + a;
    autoTimeOut = t*1000;
    autoMode = DRIVEMODE_TURN;
    turnMode = TURNMODE_ENCODER;
    recordedDistLeft = 0;
    for (int i = 0; i < motorsLeft.size(); i++) {
        recordedDistLeft += motorsLeft[i]->rotation(vex::rotationUnits::deg);
    }
    recordedDistLeft /= motorsLeft.size();
    recordedDistRight = 0;
    for (int i = 0; i < motorsRight.size(); i++) {
        recordedDistRight += motorsRight[i]->rotation(vex::rotationUnits::deg);
    }
    recordedDistRight /= motorsRight.size();
    targetDistance = (a * ticksPerDegree) + (recordedDistRight - recordedDistLeft)/2
}
void Drive::runAtSpeed(double s) {
    leftRunSpeed = s;
    rightRunSpeed = s;
    speedOverride = true;
}
void Drive::runAtSpeeds(double l, double r) {
    leftRunSpeed = l;
    rightRunSpeed = r;
    speedOverride = true;
}
void Drive::turnToPoint(double x, double y, double t = -1) {
    double dx = x - xPosition;
    double dy = y - yPosition;
    double dir = atan(dy/dx);
    turnTo(dir);
}
void Drive::driveTo(double s, double x, double y, double t = 10) {
    drivingToPos = true;
    targetX = x;
    targetY = y;
    targetS = s;
    double dx = x - xPosition;
    double dy = y - yPosition;
    double dir = atan(dy/dx);
    double dist = hypot(x,y);
    driveDist(s, dir, dist, t);
}
void Drive::stop() {
    autoTime = 0;
    autoMode = DRIVEMODE_USER;
    autoSpeed = 0;
    speedOverride = false;
    drivingToPos = false;
}


// CALL EVERY LOOP

void Drive::run() {
    // This is where the fun begins
    
    trackPosition();        // keep track of where we are on the field
    
    double forward = 0;
    double turn = 0;
    
    // Calculate useful information
    currentTime = driveBrain->timer(vex::timeUnits::msec);
    
    double currentDistLeft = 0;
    for (int i = 0; i < motorsLeft.size(); i++) {
        currentDistLeft += motorsLeft[i]->rotation(vex::rotationUnits::deg);
    }
    currentDistLeft /= motorsLeft.size();
    double currentDistRight = 0;
    for (int i = 0; i < motorsRight.size(); i++) {
        currentDistRight += motorsRight[i]->rotation(vex::rotationUnits::deg);
    }
    currentDistRight /= motorsRight.size();
    currentDist = (currentDistRight + currentDistLeft)/2;
    
    // auto functions
    if (autoMode != DRIVEMODE_USER) {
        
        if (drivingToPos) {         // keep calculating new angle & distance to stay on-target
            driveTo(targetS, targetX, targetY);
        }
        
        forward = autoSpeed;
        
        if (autoMode == DRIVEMODE_TURN)  {
            forward = 0;
            autoSpeed = 0;
        }
        
        if (autoMode == DRIVEMODE_DIST) {
            forward *= abs(((targetDistance - currentDist)/(1*ticksPerTile) + minForward));
                // slow down when within 1 tile
            if (forward > 100) forward = 100;
            if (forward < -100) forward = -100;
            
            // Terminate contition for distance
            if (autoSpeed > 0) {
                if (currentDist > targetDistance) autonComplete = true;
            }
            else {
                if (currentDist < targetDistance) autonComplete = true;
            }
        }
        
        if (currentTime > autoTimeOut && autoTimeOut > 0) {
            autonComplete = true;
            drivingToPos = false;
        }
        
        // Turn code
        double driveMag = autoSpeed;
        double seek = targetDirection;
        double angle = 0;
        
        if (turnMode == TURNMODE_GYRO) {
            angle = seek - direction;
        }
        else if (turnMode == TURNMODE_ENCODER) {
            angle = (recordedDistRight - recordedDistLeft)/2;
            angle -= (currentDistRight - currentDistLeft)/2;
            angle /= ticksPerDegree;
        }
        
        if (angle < 0) angle += 360;
        if (angle > 180) angle -= 360;
        
        angle /= (2 * turnRate);
        angle *= 127;
        if (driveMag < minSpeed) {
            angle *= 2;
        }
        
        if (angle < -maxTurn) angle = maxTurn;
        if (angle > maxTurn) angle = maxTurn;
        
        if (driveMag > minSpeed) {
            if (angle < 0) {
                if (angle > -2) {
                    angle = 0;
                }
                else if (angle > -4) {
                    angle = -4
                }
            }
            else {
                if (angle < 2) {
                    angle = 0;
                }
                else if (angle < 4) {
                    angle = 4
                }
            }
        }
        else {
            turn = angle;
            angle = abs(angle);
            if (angle < minSpeed) {
                if (((lastAngle > 0) && (turn < 0)) || ((lastAngle < 0) && (turn > 0))) {
                    angle = 0;
                }
                else {
                    if (angle > minSpeed/5) {
                        angle = minSpeed;
                    }
                    else {
                        turnPulse++;
                        if (turnPulse < pulseTime) {
                            angle = minSpeed;
                        }
                        else {
                            angle = 1;
                            if (turnPulse > pulsePause) {
                                turnPulse = 0;
                            }
                        }
                    }
                }
            }
            if (turn < 0) angle *= -1;
        }
        
        turn = angle;
        
        if (autoSpeed == 0 || autoMode == DRIVEMODE_TURN) {
            if (abs(angle) < turnAccepted && abs(lastAngle) < turnAccepted) {
                autonComplete = true;
            }
        }
        
        lastAngle = angle;
        
        
        // Auto-move is complete, so stop moving
        if (autonComplete) {
            autoMode = DRIVEMODE_USER;
            forward = 0;
            turn = 0;
            autoSpeed = 0;
        }
    }
    
    // User controls
    if (autoMode == DRIVEMODE_USER && controlsSet) {
        if (arcadeMode) {
            // Forward-backward, left-right
            leftSpeed = powerJoy->position(vex::percentUnits::pct) - turnJoy->position(vex::percentUnits::pct);
            rightSpeed = powerJoy->position(vex::percentUnits::pct) + turnJoy->position(vex::percentUnits::pct);
        }
        else {
            // Tank controls
            leftSpeed = leftSideJoy->position(vex::percentUnits::pct);
            rightSpeed = rightSideJoy->position(vex::percentUnits::pct);
        }
        if (leftSpeed*leftSpeed < deadZone*deadZone) leftSpeed = 0;
        if (rightSpeed*rightSpeed < deadZone*deadZone) rightSpeed = 0;
    }
    else {
        leftSpeed = forward - turn;
        rightSpeed = forward + turn;
    }
    
    // Constant-speed override
    if (speedOverride) {
        leftSpeed = leftRunSpeed;
        rightSpeed = rightRunSpeed;
    }
    
    // dampen motors so they don't spike current
    rightPower = rightPower + ( (rightSpeed - rightPower) / slewRate );
    leftPower = leftPower + ( (leftSpeed - leftPower) / slewRate );
    
    // Set motors
    for (int i = 0; i < motorsLeft.size(); i++) {
        motorsLeft[i]->spin(vex::directionType::fwd, leftPower, vex::velocityUnits::pct);
    }
    for (int i = 0; i < motorsRight.size(); i++) {
        motorsRight[i]->spin(vex::directionType::fwd, rightPower, vex::velocityUnits::pct);
    }
}