#ifndef ALEXALIGHTS_H
#define ALEXALIGHTS_H

/*-----------------------------------------------------------------------------
 ** Global Defines/Typedefs/Enums/Macros.
 **---------------------------------------------------------------------------*/
#define FADE_DONE       1
#define FADING_DOWN     2
#define FADING_UP       3

typedef struct pinStruct {
    int pinNum;
    int state;
    int valueNow;
    int valueEnd;
}Pin;

/*-----------------------------------------------------------------------------
 ** Function: IsTime()
 **
 ** Arguments:
 ** *timeMark   : pointer to value of timeMark variable. Gets compared with millis()
 ** timeInterval: time that must elapse before function returns true
 **---------------------------------------------------------------------------*/
boolean IsTime(unsigned long *timeMark, unsigned long timeInterval) {
    if (millis() - *timeMark >= timeInterval) {
        *timeMark = millis();
        return true;
    }
    return false;
}

/*-----------------------------------------------------------------------------
 ** Function: smoothPinCtrl()
 **
 ** Arguments:
 ** *pinName : pointer to pin structure
 ** analogCmd: cmd to set .valueEnd of pinName to. 0 -255
 **---------------------------------------------------------------------------*/
void smoothPinCtrl(Pin *pinName, int analogCmd){
    pinName->valueEnd = constrain(analogCmd, 0, 255);
    pinName->state    = (pinName->valueEnd > pinName->valueNow) ? FADING_UP:FADING_DOWN;
}

#endif