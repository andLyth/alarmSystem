#define TASTATUR_LIB
#include "mbed.h" 
//#include "BusOut.h"
//#include "BusIn.h"
class Tastatur3x3 {

public:
    Tastatur3x3(PinName r1, PinName r2, PinName r3, PinName k1, PinName k2, PinName k3);
    int read();
   
private:
    

   BusOut bus_r;
   BusIn bus_k;
};