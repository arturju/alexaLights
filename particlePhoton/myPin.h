#ifndef MYPIN_H
#define MYPIN_H

typedef struct pinStruct {
    int pinNum;
    int state;
    int valueNow;
    int valueEnd;
}Pin;

#endif