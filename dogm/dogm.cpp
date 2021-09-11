#include "dogm.h"

 DogmLCD163::DogmLCD163(PinName si, PinName clk,  PinName rs, PinName csb) : spi(si, NC, clk), _rs(rs), _csb(csb){
    
    initialize();
 
 }
 
 void DogmLCD163::initialize(){
    _rs=0;
    int commands[9]={0x39, 0x1D, 0x50,0x6C,0x7C, 0x38, 0x0F, 0x06,0x01};
    for(int i=0; i<9;i++){
        spi.write(commands[i]);
        wait_us(20);
    }
    wait_ms(75);                 
}
void DogmLCD163::write(char string[]){
    _rs=0;
    spi.write(00000001); 
    wait(0.01); 
    int x=0;
    _rs = 1;
    _csb = 0;
    while (string[x]  != 0x00){
        wait_us(30);
        spi.write( string[x] );
        x++;
        if (string[x]=='\0'){break;}
      } 
}
void DogmLCD163::writecmd(int x){
    _rs=0;
    _csb = 0;
    spi.write(x);
    

}
void DogmLCD163::clear(){
    _rs=1;
    _csb = 0;
    spi.write(00000001);
    wait_ms(80);

}
