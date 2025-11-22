#include <8051.h>

void Timer0_ISR(void) __interrupt(1) {
	//   TF0 = 0; // reset  interrupt flag
}

void delay_10ms(void) {
  TMOD &= 0xF0; // clear lower bits for timer
  TMOD |= 0x01 ; // set timer0 in mode 1
  TH0 = 0xD8; //initial value for 50 ms 
  TL0 = 0xF0;
  ET0 = 1 ; // enable timer0 interrupt
  EA = 1 ; // enable interrupts
  TR0 = 1; // timer0 start

  PCON |= 0x01;  // Set IDL bit in PCON register

  // MCU will idle until timer overflow interrupt

/*
  while (TF0 == 0) {
	__asm
		nop
	__endasm;
  } // check overflow condition
*/

  TR0 = 0; // Stop Timer
  TF0 = 0; // Clear flag
}

void delay_50ms(void) {
  TMOD &= 0xF0; // clear lower bits for timer
  TMOD |= 0x01 ; // set timer0 in mode 1
  TH0 = 0x3C; //initial value for 50 ms 
  TL0 = 0xB0;
  ET0 = 1 ; // enable timer0 interrupt
  EA = 1 ; // enable interrupts
  TR0 = 1; // timer0 start

  PCON |= 0x01;  // Set IDL bit in PCON register

  // MCU will idle until timer overflow interrupt

/*
  while (TF0 == 0) {
	__asm
		nop
	__endasm;
  } // check overflow condition
*/

  TR0 = 0; // Stop Timer
  TF0 = 0; // Clear flag
}

/* this is not a precise delay because of the operations involved in setting up the timer interrupt*/
void delay_ms(t) {
	int i;
	if (t < 50) {
		for (i = 0; i < (t / 10); i++) {
			delay_10ms();
		}
		return;
	} else {
		for (i = 0; i < (t / 50); i++) {
			delay_50ms();
		}
	}
}

/* clockwise lap */
void lap_cw(delay){
	int port, bit;
	int state = 0xFF;
    P0 = state;
    P1 = state;
    P2 = state;
    P3 = state;
	for(port = 0; port < 4; port++) {
		for(bit = 0; bit < 8; bit++) {
            switch(port) {
                case 0: P2 = ~(0x80 >> bit); break;
                case 1: P3 =  ~(0x80 >> bit); break;
                case 2: P1 =  ~(0x80 >> bit); break;
                case 3: P0 =  ~(0x01 << bit); break;
            }
	        delay_ms(delay);
		}

		// reset the port to the default state
		switch(port) {
               case 0: P2 = state; break;
               case 1: P3 = state; break;
               case 2: P1 = state; break;
               case 3: P0 = state; break;
           }
	}
}

/* fill up from bottom */
void fill(delay) {
	int step = 0;
	int bit = 0;
	int ps[4] = { 0xFF , 0xFF, 0xFF, 0xFF }; // clockwise
    P0 = ps[3];
    P1 = ps[2];
    P2 = ps[0];
    P3 = ps[1];
	
	/* 13 steps */
	for (step = 0; step < 8; step++) {
		ps[1] &= ~(0x01 << step);
		ps[2] &= ~(0x80 >> step);		
	    P1 = ps[2];
	    P3 = ps[1];
		delay_ms(delay);
	}
	{
		ps[0] &= ~(0x01);
		ps[3] &= ~(0x01);
	    P0 = ps[3]; P2 = ps[0];
		delay_ms(delay);
		
		ps[0] &= ~(0x02) & ~(0x80);
		ps[3] &= ~(0x02) & ~(0x80);
	    P0 = ps[3]; P2 = ps[0];
		delay_ms(delay);
		
		ps[0] &= ~(0x04) & ~(0x40);
		ps[3] &= ~(0x04) & ~(0x40);
	    P0 = ps[3]; P2 = ps[0];
		delay_ms(delay);
		
		ps[0] &= ~(0x08) & ~(0x20);
		ps[3] &= ~(0x08) & ~(0x20);
	    P0 = ps[3]; P2 = ps[0];
		delay_ms(delay);
		
		ps[0] &= ~(0x10);
		ps[3] &= ~(0x10);
	    P0 = ps[3]; P2 = ps[0];
		delay_ms(delay);
	}
}

void bottomup(delay) {
	int step = 0;
	int bit = 0;
	int ps[4] = { 0xFF , 0xFF, 0xFF, 0xFF }; // clockwise
    P0 = ps[3];
    P1 = ps[2];
    P2 = ps[0];
    P3 = ps[1];
	
	for (step = 0; step < 8; step++) {
		ps[1] &= ~(0x01 << step);
		ps[2] &= ~(0x80 >> step);		
	    P1 = ps[2];
	    P3 = ps[1];
		delay_ms(delay);
	}
	for (step = 0; step < 8; step ++) {
		ps[0] &= ~(0x01 << step);
		ps[3] &= ~(0x01 << step);
	    P0 = ps[3]; P2 = ps[0];
		delay_ms(delay);
	}

	delay_ms(delay);
	delay_ms(delay);
	delay_ms(delay);
}

void cylon(delay) {
	int step = 0;
	int bit = 0;
	int ps[4] = { 0xFF , 0xFF, 0xFF, 0xFF }; // clockwise
    P0 = ps[3];
    P1 = ps[2];
    P2 = ps[0];
    P3 = ps[1];
	
	for (step = 0; step < 8; step++) {
		ps[1] = ~(0x01 << step);
		ps[2] = ~(0x80 >> step);		
	    P1 = ps[2];
	    P3 = ps[1];
		delay_ms(delay);
	}
	P1 = ps[2] = 0xFF;
	P3 = ps[1] = 0xFF;
	for (step = 0; step < 8; step ++) {
		ps[0] = ~(0x01 << step);
		ps[3] = ~(0x01 << step);
	    P0 = ps[3]; P2 = ps[0];
		delay_ms(delay);
	}
	P0 = ps[3] = 0xFF;
	P2 = ps[0] = 0xFF;

	for (step = 0; step < 8; step ++) {
		ps[0] = ~(0x80 >> step);
		ps[3] = ~(0x80 >> step);
	    P0 = ps[3]; P2 = ps[0];
		delay_ms(delay);
	}
	P0 = ps[3] = 0xFF;
	P2 = ps[0] = 0xFF;
	for (step = 0; step < 8; step++) {
		ps[1] = ~(0x80 >> step);
		ps[2] = ~(0x01 << step);		
	    P1 = ps[2];
	    P3 = ps[1];
		delay_ms(delay);
	}
	P1 = ps[2] = 0xFF;
	P3 = ps[1] = 0xFF;
}

void all_off(delay) {
	P0 = 0xFF;
	P1 = 0xFF;
	P2 = 0xFF;
	P3 = 0xFF;
	delay_ms(delay);
}

void all_on(delay) {
	P0 = 0x00;
	P1 = 0x00;
	P2 = 0x00;
	P3 = 0x00;
	delay_ms(delay);
}

void main(void) {

    while(1) {
		cylon(20);
		cylon(20);
		cylon(20);

		lap_cw(50);
		lap_cw(50);
		lap_cw(50);
//		fill(50);
		bottomup(100);
		bottomup(100);
		bottomup(100);
		all_off(500);
		all_on(5000);
    }
}
