#include "functions.h"

extern uint16_t PB2_rate;
extern uint16_t PB_event;
extern uint16_t delay_count;
// 0bXX1 for PB1 pressed, 0bX1X for PB2 pressed, 0b1XX for PB3 pressed
extern uint8_t last_state;
extern uint8_t current_state;

void IOinit() {
    TRISBbits.TRISB8 = 0;
    
    TRISAbits.TRISA4 = 1;
    CNPU1bits.CN0PUE = 1;
    CNEN1bits.CN0IE = 1;
    
    TRISBbits.TRISB4 = 1;
    CNPU1bits.CN1PUE = 1;
    CNEN1bits.CN1IE = 1;
    
    TRISAbits.TRISA2 = 1;
    CNPU2bits.CN30PUE = 1;
    CNEN2bits.CN30IE = 1;
}

void TIMINGinit() {
    newClk(500);
    
    T3CONbits.TCKPS = 0b10; // set prescalar to 1:64
    T2CONbits.T32 = 0; // operate timer 2 as 16 bit timer
    IEC0bits.T2IE = 1; //enable timer interrupt
    
    T3CONbits.TCKPS = 1; // set prescaler to 1:8
    T3CONbits.TCS = 0; // use internal clock
    T3CONbits.TSIDL = 0; //operate in idle mode
    IPC2bits.T3IP = 2; //7 is highest and 1 is lowest pri.
    IFS0bits.T3IF = 0;
    IEC0bits.T3IE = 1; //enable timer interrupt
    PR3 = 15625; // set the count value for 0.5 s (or 500 ms)
    TMR3 = 0;
    T3CONbits.TON = 1;
    
    PB2_rate = 2000;
}

void CNinit() {
    PB_event = 0;
    
    IPC4bits.CNIP = 6;
    IFS1bits.CNIF = 0;
    IEC1bits.CNIE = 1;
}

void IOcheck() {
    while (current_state == 0b101) {
        led_toggle();
        delay_ms(4000);
    }
    while (current_state == 0b011) {
        led_toggle();
        delay_ms(500);
    }
    while (current_state == 0b001) {
        led_toggle();
        delay_ms(250);
    }
    while (current_state == 0b010) {
        led_toggle();
        delay_ms(PB2_rate);
    }
    led_off();
    Idle();
    asm("nop"); // this starts executing before CNInterrupt
    PB_event = 0; // this executes after CNInterrupt thanks to the previous nop
}

void delay_ms(uint16_t ms_count) {
    PR2 = 250 * ms_count / 64;
    TMR2 = 0;
    T2CONbits.TON = 1;
    
    Idle();
    
    T2CONbits.TON = 0;
}

void led_on() {
    LATBbits.LATB8 = 1;
}

void led_off() {
    LATBbits.LATB8 = 0;
}

void led_toggle() {
    LATBbits.LATB8 ^= 1;
}