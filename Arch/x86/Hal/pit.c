#include <Hal.h>
#include "idt.h"
#include "pic.h"
#include "pit.h"


#define	I86_PIT_REG_COUNTER0	0x40
#define	I86_PIT_REG_COUNTER1	0x41
#define	I86_PIT_REG_COUNTER2	0x42
#define	I86_PIT_REG_COMMAND	0x43


// Global tick count
static volatile uint32_t _pit_ticks = 0;

static uint8_t _pit_isinit = 0;

extern void _i86_pit_irq();



void _i86_pit_irq_c(){
	_pit_ticks++;		
	interruptdone(0);
}

uint32_t i86_pit_set_tick_count(uint32_t i){

	uint32_t ret = _pit_ticks;
	_pit_ticks = i;
	return ret;

}

uint32_t i86_pit_get_tick_count(){
	
	return _pit_ticks;

}

void i86_pit_send_command (uint8_t cmd){

	outportb(I86_PIT_REG_COMMAND,cmd);

}

void i86_pit_send_data(uint16_t data, uint8_t counter){

	uint8_t port = 0;
	
	if(counter == I86_PIT_OCW_COUNTER_0)
		port = I86_PIT_REG_COUNTER0;
	else if(counter == I86_PIT_OCW_COUNTER_1)
		port = I86_PIT_REG_COUNTER1;
	else 
		port = I86_PIT_REG_COUNTER2;

	outportb(port,(uint8_t)data);

}

uint8_t i86_pit_read_data(uint16_t counter){

	uint8_t port = 0;
	
	if(counter == I86_PIT_OCW_COUNTER_0)
		port = I86_PIT_REG_COUNTER0;
	else if(counter == I86_PIT_OCW_COUNTER_1)
		port = I86_PIT_REG_COUNTER1;
	else 
		port = I86_PIT_REG_COUNTER2;

	return inportb(port);	

}

void i86_pit_start_counter (uint32_t freq, uint8_t counter, uint8_t mode){

	if(freq==0)
		return;

	uint16_t divisor = (uint16_t) (1193181 / (uint16_t)freq);

	uint8_t ocw = 0;

	ocw = (ocw & ~I86_PIT_OCW_MASK_MODE) | mode;
	ocw = (ocw & ~I86_PIT_OCW_MASK_RL) | I86_PIT_OCW_RL_DATA;
	ocw = (ocw & ~I86_PIT_OCW_MASK_COUNTER) | counter;
	i86_pit_send_command(ocw);

	i86_pit_send_data (divisor & 0xff,0);
	i86_pit_send_data ((divisor>>8) & 0xff,0);	

	_pit_ticks=0;

}
extern void _i86_gen();
void i86_pit_initialize(){

	setvect(32,_i86_pit_irq,0);
	_pit_isinit = 1;

}


uint8_t i86_pit_is_initialized(){

	return 	_pit_isinit ;

}
