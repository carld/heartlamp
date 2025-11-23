#include <8051.h>

void Timer0_ISR(void) __interrupt(1) {
	//   TF0 = 0; // reset  interrupt flag
}

void delay_10ms(void) {
  TMOD &= 0xF0; // clear lower bits for timer
  TMOD |= 0x01 ; // set timer0 in mode 1
  TH0 = 0xD8; //initial value for 10 ms 
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
	if (t <= 0) return;
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

void all_off(void) {
	P0 = 0xFF;
	P1 = 0xFF;
	P2 = 0xFF;
	P3 = 0xFF;
}

void all_on(void) {
	P0 = 0x00;
	P1 = 0x00;
	P2 = 0x00;
	P3 = 0x00;
}

void all(state) {
	P0 = state;
	P1 = state;
	P2 = state;
	P3 = state;
}

/* clockwise from top middle, 0 is first to the right side of valley */
__code unsigned char led_masks[32] = {
    0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01,  // P2 bits 7-0
    0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01,  // P3 bits 7-0
    0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01,  // P1 bits 7-0
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80   // P0 bits 0-7
};

void set_led(unsigned char index, unsigned char value) {
	if (index >= 32) index = index - 32; // wrap around

    unsigned char mask = led_masks[index];
    
    if (index < 8) {
        if (value) P2 |= mask; else P2 &= ~mask;
    } else if (index < 16) {
        if (value) P3 |= mask; else P3 &= ~mask;
    } else if (index < 24) {
        if (value) P1 |= mask; else P1 &= ~mask;
    } else {
        if (value) P0 |= mask; else P0 &= ~mask;
    }
}

/* zero is the bottom, 13 is the top */
void set_led_pair(unsigned char index, unsigned char value) {
	if (index > 12) return;

	if (index < 8) {
		set_led(15 - index, value);
		set_led(16 + index, value);
	} else {
		switch(index) {
			case 8:
				set_led(7, value);
				set_led(24, value);
				break;
			case 9:
				set_led(0, value);
				set_led(6, value);
				set_led(25, value);
				set_led(31, value);
				break;
			case 10:
				set_led(1, value);
				set_led(5, value);
				set_led(26, value);
				set_led(30, value);
				break;
			case 11:
				set_led(2, value);
				set_led(4, value);
				set_led(27, value);
				set_led(29, value);
				break;
			case 12:
				set_led(3, value);
				set_led(28, value);
				break;
		}
	}
}

/* clockwise lap */
void lap_cw(delay) {
	int i;
	all_off();
	for (i = 0; i < 32; i++) {
		set_led(i, 0);
		delay_ms(delay);
		set_led(i, 1);
	}
}

/* counter clockwise lap */
void lap_ccw(delay) {
	int i;
    all_off();
	for (i = 31; i >= 0; i--) {
		set_led(i, 0);
		delay_ms(delay);
		set_led(i, 1);
	}
}

/* fill up from bottom, level riser like water filling a bucker */
void fill(delay) {
	int step = 0;
    all_off();
	
	for (step = 0; step < 13; step++) {
		set_led_pair(step, 0);
		delay_ms(delay);
	}
}

/* light them all up on both sides starting at the bottom */
void bottomup(delay) {
	int step = 0;
    all_off();

	for (step = 0; step < 16; step++) {
		set_led(15 - step, 0);
		set_led(16 + step, 0);
		delay_ms(delay);
	}

	delay_ms(delay);
}

void middleout(delay, state) {
	int x, y;
	all(~state);
	for (x = 8, y = 8; x <= 16 && y >= 0; x++, y--) {
		set_led(x, state);
		set_led(y, state);
		set_led(x+15, state);
		set_led(y+15, state);
		delay_ms(delay);
	}

	delay_ms(delay);
}


/* extinguish both sides starting from the top */
void bottomdown(delay) {
	int step = 0;
	all_on();
	for (step = 15; step >= 0; step--) {
		set_led(15 - step, 1);
		set_led(16 + step, 1);
		delay_ms(delay);
	}

	delay_ms(delay);
}

void cylon(delay) {
	int step = 0;
    all_off();
	for (step = 0; step < 16; step++) {
		set_led(15 - step, 0);
		set_led(16 + step, 0);
		delay_ms(delay);
		set_led(15 - step, 1);
		set_led(16 + step, 1);
	}
	delay_ms(delay);
	for (step = 15; step >=0 ; step--) {
		set_led(15 - step, 0);
		set_led(16 + step, 0);
		delay_ms(delay);
		set_led(15 - step, 1);
		set_led(16 + step, 1);
	}
	delay_ms(delay);
}

const unsigned char sine_table[360] = {
    6, 6, 6, 6, 6, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 9, 9, 9, 9, 9,
    9, 9, 9, 9, 9, 9, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 10, 10, 11,
    11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
    11, 11, 11, 11, 11, 11, 11, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 11, 11, 11, 11, 11, 11,
    11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
    11, 11, 10, 10, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 9, 9, 9, 9, 9,
    9, 9, 9, 9, 9, 9, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 6, 6, 6, 6
};
/* three led wide bar that bounces */
void rasterbar(delay){
	int x;

   all_off();
	
	for (x = 0; x < 360; x+=10) {
		set_led_pair(sine_table[x]-1, 0);
		set_led_pair(sine_table[x], 0);
		set_led_pair(sine_table[x]+1, 0);
		delay_ms(delay);
		set_led_pair(sine_table[x]-1, 1);
		set_led_pair(sine_table[x], 1);
		set_led_pair(sine_table[x]+1, 1);
	}
	delay_ms(delay);
}

void line(int delay, int length, int times) {
	unsigned char buffer[32] = { 0xff };
	int step, x;

	for (step = 0; step < (32 * times) + length; step ++) {
		for (x = 0; x < 32; x++) {
			buffer[x] = 0xff;
		}
		for (x = 0 ; x < length; x++) {
			int offset = step + x;
			if (offset >= 32) offset = offset % 32;
			buffer[offset] = 0;
		}
		for (x = 0 ; x < 32; x++) {
			set_led(x, buffer[x]);
		}
		delay_ms(delay);
	}
}

void main(void) {

    while(1) {
		//rasterbar(20);
		line(20, 16, 10);
		middleout(50, 0x00);
		middleout(50, 0xff);
		middleout(50, 0x00);
		middleout(50, 0xff);
		middleout(50, 0x00);
		middleout(50, 0xff);
		cylon(20);
		cylon(20);
		cylon(20);
		lap_cw(20);
		lap_cw(20);
		lap_ccw(20);
		lap_ccw(20);
		lap_cw(20);
		lap_cw(20);
		lap_ccw(20);
		lap_ccw(20);
		lap_cw(20);
		lap_cw(20);
		lap_ccw(20);
		lap_ccw(20);
		bottomup(100);
		bottomdown(100);
		bottomup(100);
		bottomdown(100);
		bottomup(100);
		bottomdown(100);
		all_off();
		all_on();
		delay_ms(2000);
		all_off();
		delay_ms(20);

    }
}

