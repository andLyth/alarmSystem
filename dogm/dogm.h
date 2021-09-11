#define DOGM_LIB
#include "mbed.h" 
class DogmLCD163 {

public:
    DogmLCD163(PinName clk, PinName si, PinName rs, PinName csb);
    
    void initialize();
    void write(char string[]);
    void writecmd(int x);
    void clear();
private:
    
    SPI spi;
    DigitalOut _rs;
    DigitalOut _csb;
};