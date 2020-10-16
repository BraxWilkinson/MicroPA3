#include "mbed.h"

InterruptIn sC(p15,PullUp);
InterruptIn sF(p26,PullDown);
AnalogIn sensor(p18);
BusOut display(p5,p6,p7,p8,p9,p10,p11,p12);

float degree = 0x39;
float tempArray[4];
float samples = 0;
int digitNum = 0;
int mode = 0;

void flipC() {
    degree = 0x39;
    mode = 0;
}                           // end flipC
void flipF() {
    degree = 0x71;
    mode = 1;
}                           // end flipF
void getSamples() {
    samples = 0;
    for (int i=0; i<1000; i++) {     // collect 100 samples from temperature sensor
        samples += sensor;
    }
    samples = samples/1000;
}                           // end tempSensor
float hexConvert(int digit) {
    float hex;
    switch (digit) {
        case 0: hex = 0x3F; break;
        case 1: hex = 0x06; break;
        case 2: hex = 0x5B; break;
        case 3: hex = 0x4F; break;
        case 4: hex = 0x66; break;
        case 5: hex = 0x6D; break;
        case 6: hex = 0x7D; break;
        case 7: hex = 0x07; break;
        case 8: hex = 0x7F; break;
        case 9: hex = 0x6F; break;
    }
    return hex;
}
int voltageToTemp() {
    // convert voltage samples to hex and store in tempArray
    int temp = 0;
    if (mode == 0) {
        temp = (samples-0.5)/0.01;          // perform the calculation to convert from volts to celsius
    } else if (mode == 1) {
        temp = ((samples-0.5)/0.01)*(9/5)+32;       // perform the calculation to convert from volts to fahrenheit
    }
    if (temp < 0) {         // fill tempArray with temperature values
        tempArray[0] = 0b01000000;      // display negative sign
        temp = temp*-1;
        tempArray[1] = hexConvert(temp/10);
        tempArray[2] = hexConvert(temp - (temp/10)*10);
        tempArray[3] = degree;
        return 4;
    } else if (temp >= 0) {
        tempArray[0] = hexConvert(temp/10);         // get 10s place digit
        tempArray[1] = hexConvert(temp - (temp/10)*10);     // get 1s place digit
        tempArray[2] = degree;
        return 3;
    }                       // end if block
    return 0;
}                           // end voltageToTemp
void tempOut() {
    sC.rise(NULL);         // disable interrupts
    sF.rise(NULL);
    getSamples();          // execute sampling from sensor
    digitNum = voltageToTemp();     // convert temp into hex to store in tempArray and retieve # of digits
    for (int i=0; i<=digitNum; i++) {   // display values stored in tempArray
        display = tempArray[i];   
        wait(0.7);
        display = 0;
        wait(0.1);
    }                       // end for loop
    sC.rise(&flipC);          // reenable interrupts
    sF.rise(&flipF);
}                           // end tempOut

int main() {
    flipC();
    sC.rise(&flipC);        // change the display to Celsius
    sF.rise(&flipF);        // change the display to Fahrenheit
    
    while(1) {
        tempOut();
        wait(5);
    }                       // end while
}                           // end main
