#include "tastatur.h"
#include "stdio.h"

 Tastatur3x3::Tastatur3x3(PinName r1, PinName r2, PinName r3, PinName k1, PinName k2, PinName k3) : bus_r(r1, r2, r3), bus_k(k1, k2, k3){

    
 
 }
 
int Tastatur3x3::read(){
 for(int l=0; l<3; l++) {
            bus_r = 0;
    }    
    for(int i=0; i<3; i++) {
            bus_r = 1 << i;     
  
        if(bus_k==1){return 1+(i*3);}
        if(bus_k==2){return 2+(i*3);}
        if(bus_k==4){return 3+(i*3);}
    }    
    return -1;
} 