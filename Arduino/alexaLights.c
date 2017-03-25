/*-----------------------------------------------------------------------------
 **                        � Arturo Jumpa
 ** File: alexaLights.c
 **
 ** Description:
 ** Connect's particle photon's API with Amazon's lambda function
 **---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 ** Includes
 ** Any struct definitions must be put in a separate .h file because the IDE
 ** parses sketch, extracts function prototypes and moves them to the top
 **---------------------------------------------------------------------------*/
#include "myPin.h"

/*-----------------------------------------------------------------------------
 ** Global Defines/Typedefs/Enums/Macros.
 **---------------------------------------------------------------------------*/
Pin pinD1       = {.pinNum = 1, .state = 0, .valueNow = 0, .valueEnd = 0};
Pin pinD2       = {.pinNum = 2, .state = 0, .valueNow = 0, .valueEnd = 0};
Pin pinD3       = {.pinNum = 3, .state = 0, .valueNow = 0, .valueEnd = 0};
Pin *pOutPin[]  = {&pinD1, &pinD2};
Pin *pInPin[]   = {&pinD3};

#define OUT_ARRAY_SIZE  sizeof(pOutPin)/sizeof(pOutPin[0])
#define IN_ARRAY_SIZE   sizeof(pInPin)/sizeof(pInPin[0])
#define FADE_DONE       1
#define FADING_DOWN     2
#define FADING_UP       3

unsigned long int pinUpdateInterval  = 15;          // update fade value every 15 ms */
unsigned long     pinTimeMark        = 0;
unsigned long int pirTimeOutInterval = 30*1000;     // no movement for 30s 
unsigned long     pirTimeMark        = 0;
unsigned long int autoUpdateInterval = 15*60*100;   // no command for 15mins
unsigned long     autoTimeMark       = 0;

int               partyFlag          = 0;
int               autoMode           = 0;

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
 ** Function: turnPinOnOff()
 **
 ** Description: This function gets called by Particle's API.
 ** Arguments:   Sample string '7,0'. '1,44'
 **---------------------------------------------------------------------------*/
int turnPinOnOff(String args){
    Pin     *pinPointer;
    autoMode     = 0;
    autoTimeMark = millis();                        // reset autoMode timer

    int pos = args.indexOf(',');
    if(pos == -1)     return -1;                    // return error if function argument has no comma
    
    String  strPin      = args.substring(0, pos);   // separate 'pin' part of the string
    String  strValue    = args.substring(pos + 1);  // separate 'value' part of the string
    
    int     pinNumber   = strPin.toInt();
    switch (pinNumber){
        case 1:
            Serial.println("Pin D1 Selected");
            pinPointer = &pinD1;
            break;
        case 2:
            Serial.println("Pin D2 Selected");
            pinPointer = &pinD2;
            break;
        default:
            Serial.println("Error");
    }
    
    // 'strValue' is a number, presumably from 0 to 100
    int     pinValue    = constrain(strValue.toInt(), 0, 100);
    smoothPinCtrl(pinPointer, map(pinValue, 0, 100, 0, 255) );
    
    Serial.print("pinNum ");  Serial.print(pinNumber);   Serial.print(", ");
    Serial.print("Value: ");  Serial.print(pinValue);    Serial.println();
    
    for(int i=0; i < OUT_ARRAY_SIZE; i++){
        Serial.print("num: ");          Serial.print(pOutPin[i]->pinNum);
        Serial.print(", state: ");      Serial.print(pOutPin[i]->state);
        Serial.print(", valueEnd: ");   Serial.print(pOutPin[i]->valueEnd); Serial.println("");
    }
    
    return 1;
}

/*-----------------------------------------------------------------------------
 ** Function: partyFunc()
 **
 ** Description:
 ** Enables partFlag
 **---------------------------------------------------------------------------*/
int partyFunc(String args){
    Serial.print("Party time!!");
    partyFlag = 1;
    return 1;
}

/*-----------------------------------------------------------------------------
 ** Function: setup()    --- Initializer code
 **
 ** Description:
 ** sets up serial communication, GPIOs and connects with Spark API
 **---------------------------------------------------------------------------*/
void setup() {
    Serial.begin(115200);
    for(int i=0; i < OUT_ARRAY_SIZE; i++)   pinMode(pOutPin[i]->pinNum, OUTPUT);
    for(int i=0; i < IN_ARRAY_SIZE; i++)    pinMode(pInPin[i]->pinNum, INPUT);

    Spark.function("myFunction", turnPinOnOff);
    Spark.function("globalParty", partyFunc);
}


/*-----------------------------------------------------------------------------
 ** Function: loop()    --- Main Loop
 **
 ** Description:
 ** State machine lives here
 **---------------------------------------------------------------------------*/
void loop() {
    
    // Respond to Pin state changes
    for(int i=0; i < OUT_ARRAY_SIZE; i++){
        switch(pOutPin[i]->state){
            case FADING_UP:
                if (pOutPin[i]->valueNow >= pOutPin[i]->valueEnd)  {pOutPin[i]->state = FADE_DONE;   break;}
                if( IsTime(&pinTimeMark,pinUpdateInterval) ){
                    pOutPin[i]->valueNow += 1;
                    analogWrite(pOutPin[i]->pinNum, pOutPin[i]->valueNow);
                }
                break;
                
            case FADING_DOWN:
                if (pOutPin[i]->valueNow <= pOutPin[i]->valueEnd)  {pOutPin[i]->state = FADE_DONE;   break;}
                if( IsTime(&pinTimeMark,pinUpdateInterval) ){
                    pOutPin[i]->valueNow -= 1;
                    analogWrite(pOutPin[i]->pinNum, pOutPin[i]->valueNow);
                }
                break;
                
            case FADE_DONE:
                break;
        }
    }

    // Read inputs
    for(int i=0; i < IN_ARRAY_SIZE; i++)    pInPin[i]->valueNow = digitalRead(pInPin[i]->pinNum);

    // Update other states

    if(autoMode){
        // If PIR interval has elapsed, fade to 0
        if (IsTime(&pirTimeMark, pirTimeOutInterval)){
            digitalWrite(pinD2.pinNum, LOW);
            smoothPinCtrl(&pinD1, 0);
        }
        // If we have seen a rising edge, reset timer. Make sure pinD1 is high
        if (pinD3.valueNow == HIGH && pinD3.valueEnd == LOW){
            smoothPinCtrl(&pinD1, 255);
            digitalWrite(pinD2.pinNum, HIGH);          // turn indicator LED on
            pirTimeMark     = millis();                // reset PIR timer
        }
    }


    if(partyFlag){
        for(int i =0; i<=30; i++){
            digitalWrite(pOutPin[0]->pinNum, LOW);
            digitalWrite(pOutPin[1]->pinNum, HIGH);
            delay(100);
            digitalWrite(pOutPin[0]->pinNum, HIGH);
            digitalWrite(pOutPin[1]->pinNum, LOW);
            delay(100);
        }
        digitalWrite(pOutPin[0]->pinNum, LOW);
        digitalWrite(pOutPin[1]->pinNum, LOW);
        partyFlag = 0;
    }


    if(IsTime(&autoTimeMark, autoUpdateInterval))   autoMode = 1;   // enable autoMode
    
    // Remember last state: assign previos readings to variables
    for(int i=0; i < IN_ARRAY_SIZE; i++)    pInPin[i]->valueEnd = pInPin[i]->valueNow;


    
}