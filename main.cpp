/************************************************\
*                -Alarmanlegg-                   *
*               Prosjektoppgave 3                *
*        Progammering og mikrokontrollere        *
*                                                *
*                 Skrevet av                     *
*         Andreas Lyth og Nina Storbakk          *
*                                                *
\************************************************/

#include "mbed.h"
#include "dogm.h"
#include "tastatur.h"
#include "stdio.h"
#include "SRF08.h"
#define SIZE 4 //Antall kretser 

/*Deklarerer globale variabler*/
AnalogIn circuit[] = { p16, p17, p18, p19 };
DigitalOut leds[] = {p21, p23, p25, p27, p22, p24, p26, p28, p29}; //leds knappealarm


DogmLCD163 lcd(p11, p13, p15, p20); //lcd
Tastatur3x3 taster(p7, p6, p5, p30, p8, p14); //tastatur
SRF08 srf02(p9, p10, 0xE0);

/*USB Virtual Serial Port*/
Serial pc(USBTX, USBRX);
/*Deklarerer lokalt filsystem*/
LocalFileSystem local("local");

/*Deklarerer timer*/
Timer timer;
/*Deklarerer tickere*/
Ticker voltage;
Ticker readPc;
Ticker readTast;
Ticker range;

/*Funksjonsdeklerasjoner*/
extern "C" void mbed_reset();
void readAnIn();
void inputPC();
void inputTast();
void readInput();
void clearSignal(void);
void alarmLog(int y, int x);
void execCmd(int c);
void resetSingle(int x);
int inpConvert(int x);
void findRange(void);
bool log1m=false;
bool log2m=false;
/* Oppretter filpeker*/
FILE *fp;


int main(){
    fp = fopen("/local/log.txt", "w"); //oppretter fil p&#65533; lokalt filsystem
    clearSignal();//sl&#65533;r av alle lys
    timer.start();//starter timer
    lcd.write("Alarmanlegg     aktivt.");
    
    /*Starter tickere*/
    voltage.attach(&readAnIn, 0.2);
    readPc.attach(&inputPC, 1.0);
    readTast.attach(&inputTast, 0.75);
    range.attach(&findRange, 0.2);
    while (1) {}//soerger at programmet ikke stopper u&#65533;nsket.
}


/*Leser av analoge innganger fra kretsene, setter
led og kaller opp alarmLog() ved spenningsfall*/
void readAnIn() {
    int i;

    for (i=0; i<SIZE; i++) {
        char buffer[48];
        if ( circuit[i] < 0.25 ||(circuit[i] > 0.3 && circuit[i] < 0.35 )) {
            if (leds[i + SIZE]==0) {//tester om alarm er satt for kun &#65533; loggf&#65533;re &#65533;n gang
                leds[i + SIZE]=1;                
                alarmLog(1, 2*i+2);//parametere logtype and alarm nummer
                lcd.clear();
                sprintf(buffer,"Alarm nummer %d  er aktivert",2*i+2);
                lcd.write(buffer);
            }
        }
        if (circuit[i]<0.25 ||(circuit[i]>0.48 && circuit[i]<0.52)) {
            if (leds[i]==0) {
                leds[i]=1;
                alarmLog(1, 2*i+1);
                lcd.clear();
                sprintf(buffer,"Alarm nummer %d  er aktivert",2*i+1);
                lcd.write(buffer);
            }
        }
        if (circuit[i] <= 0.1) {//setter og loggf&#65533;rer alarm ved spenningsfall
            leds[2*i] = leds[2*i +1] = 1;
            alarmLog(4, i);
            lcd.clear();
            sprintf(buffer,"Signalfeil paa   krets %d",i);
            lcd.write(buffer);
        }
    }
}


//sjekker om input fra PC foreligger
void inputPC() {
    if (pc.readable()) {
        readInput();
    }
}
void inputTast(){
    int x;
    x=taster.read();
    if(x>-1 && x<10){
    resetSingle(x);
    }
    else{return;}
}

//tolker input fra PC
void readInput() {
    int i;
    char cmd[10];
    for (i=0; i<11; i++) {
        cmd[i]=pc.getc();

        if (cmd[i]=='\n') {
            break;
        }
    }
    if (cmd[0]=='q' || cmd[0]=='Q' ||//quit
         cmd[0]=='r' || cmd[0]=='R' ||//reset
         cmd[0]=='s' || cmd[0]=='S') {//start
        execCmd(cmd[0]);
    }
}

void resetSingle(int x){
    char buffer[48];
    if(x>8 && leds[8]==1){
        pc.printf("hallo");
        leds[8]=0;
        lcd.clear();
        lcd.write("Alarm 9 er      tilbakestilt."); 
        log1m = log2m= false;
        alarmLog(8, 9);    
        return;           
    }
    else if(leds[inpConvert(x)]==1 && x<9){
        leds[inpConvert(x)]=0;
        lcd.clear();
        sprintf(buffer,"Alarm %d er      tilbakestilt.",x);
        lcd.write(buffer);
        alarmLog(8, x);
        
    }
}

int inpConvert(int x){
    int y;
    int convert[]={0,4,1,5,2,6,3,7};
    y=convert[x-1];
    return y;
    
}


//setter alarmsignaler lave
void clearSignal(void) {
    int i = 0;

    for (i = 0; i < (SIZE+SIZE+1); i++) {
        leds[i] = 0;
    }
    log1m = log2m =false;
   
}


/*skriver til fil ved utl&#65533;st alarm.
parametere er loggtype og alarmnummer.
Loggf&#65533;rer ogs&#65533; tiden for utl&#65533;st alarm*/
void alarmLog(int y, int x) {
    

    if (y==1) {
        fprintf(fp,"Alarm nr: %d aktivert. Tid: %f\r\n", x,timer.read());
        
    }
    if (y==2) {
        fprintf(fp,"Alarmanlegg tilbakestilt.  Tid: %f\r\n", timer.read());
    }
    if (y==3) {
        fprintf(fp,"Alarmanlegg deaktivert.  Tid: %f\r\n", timer.read());
    }
    if (y==4) {
        fprintf(fp,"Signalfeil pa alarmkrets %d.  Tid: %f\r\n",x, timer.read());
        pc.printf("Signalfeil pa alarmkrets %d. \r\n",x);
    }
   
    
    
      if (y==5) {
        fprintf(fp,"Objekt detektert nærmere enn 2 meter. Tid: %f\r\n", timer.read());
        lcd.clear();
        lcd.write("Objekt detektert Nærmere enn 2m.");
    }
    if (y==6) {
        fprintf(fp,"Objekt forsvunnet. Tid: %f\n\r", timer.read());
        lcd.clear();
        lcd.write("Objekt           forsvunnet.");
    }
    if (y==7) {
        fprintf(fp,"avstandsalarm aktivert. Tid: %f\n\r", timer.read());
        lcd.clear();
        lcd.write("Avstandsalarm   utløst");
    }
     if (y==8){
        fprintf(fp,"Alarm nr: %d tilbakestilt.  Tid: %f\r\n",x,timer.read());
     }   

}


/*utf&#65533;rer valgt kommando kalt fra bruker*/
void execCmd(int c) {
    
    if (c == 'r' || c == 'R') {
        clearSignal();
        alarmLog(2, 0);
        pc.printf("\r\nAlarmanlegg tilbakestilt.");
        lcd.clear();
        lcd.write("Alarmanlegg    tilbakestilt.");
    }
    if (c == 'q' || c == 'Q') {
        clearSignal();
        voltage.detach();
        range.detach();
        readTast.detach();
        
        alarmLog(3, 0);
         pc.printf("\r\nAlarmen er deaktivert.\r\nFor a reaktivere alarm skriv start\r\nLoggen vil bli slettet.\r\n");
        lcd.clear();
        lcd.write("Alarmanlegg     deaktivert.");
        fclose(fp);
    }
    if (c == 's' || c == 'S') {
        pc.printf("\r\nAlarmanlegg reaktivert.");
        lcd.clear();
        lcd.write("Alarmanlegg     reaktivert.");
        mbed_reset();
    }
}


/*leser av avstandsmåler og aktiverer siste led i array*/
void findRange(void) {
    if (log2m == false && srf02.read() < 201.00) {
        pc.printf("objekt detektert. avtand: %.2f\n\r", srf02.read());
        alarmLog(5, 0);
        log2m = true;
        return;
    }
    if (log2m == true && srf02.read() > 201.00) {
        pc.printf("objekt forsvunnet\n\r\r");
        alarmLog(6, 0);
        log2m = false;
        return;
    }
    if (log1m == false && srf02.read() < 101.00) {
        leds[8] = 1;
        alarmLog(7, 0);
        log1m = true;
        return;
    }
}

