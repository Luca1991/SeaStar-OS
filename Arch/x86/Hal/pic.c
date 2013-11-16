#include <Hal.h>
#include "pic.h"


// PIC1 register port addr
#define I86_PIC1_REG_COMMAND	0x20
#define I86_PIC1_REG_STATUS	0x20
#define I86_PIC1_REG_DATA	0x21	
#define I86_PIC1_REG_IMR	0x21

//PIC2 register port addr
#define I86_PIC2_REG_COMMAND	0xA0
#define I86_PIC2_REG_STATUS	0xA0
#define I86_PIC2_REG_DATA	0xA1
#define I86_PIC2_REG_IMR	0xA1


// Initialization Control Word 1 bit masks (ICW1 bit masks)
#define I86_PIC_ICW1_MASK_IC4	0x1		// 00000001
#define I86_PIC_ICW1_MASK_SNGL	0x2		// 00000010
#define I86_PIC_ICW1_MASK_ADI	0x4		// 00000100
#define I86_PIC_ICW1_MASK_LTIM	0x8		// 00001000
#define I86_PIC_ICW1_MASK_INIT	0x10		// 00010000

// Initialization Control Word 4 bit mask (ICW4 bit masks - ICW2 and ICW3 doesn't require bit masks)
#define I86_PIC_ICW4_MASK_UPM	0x1		// 00000001
#define I86_PIC_ICW4_MASK_AEOI	0x2		// 00000010
#define I86_PIC_ICW4_MASK_MS	0x4		// 00000100
#define I86_PIC_ICW4_MASK_BUF	0x8		// 00001000
#define I86_PIC_ICW4_MASK_SFNM	0x10		// 00010000


// Initialization Command 1 control bits

#define I86_PIC_ICW1_IC4_EXPECT			1	// 1
#define I86_PIC_ICW1_IC4_NO			0
#define I86_PIC_ICW1_SNGL_YES			2	// 10
#define I86_PIC_ICW1_SNGL_NO			0
#define I86_PIC_ICW1_ADI_CALLINTERVAL4		4	// 100
#define I86_PIC_ICW1_ADI_CALLINTERVAL8		0
#define I86_PIC_ICW1_LTIM_LEVELTRIGGERED	8	// 1000
#define I86_PIC_ICW1_LTIM_EDGETRIGGERED		0
#define I86_PIC_ICW1_INIT_YES			0x10	// 10000
#define I86_PIC_ICW1_INIT_NO			0

// Initialization Command 4 control bits

#define I86_PIC_ICW4_UPM_86MODE			1	// 1
#define I86_PIC_ICW4_UPM_MCSMODE		0
#define I86_PIC_ICW4_AEOI_AUTOEOI		2	// 10
#define I86_PIC_ICW4_AEOI_NOAUTOEOI		0
#define I86_PIC_ICW4_MS_BUFFERMASTER		4	// 100
#define I86_PIC_ICW4_MS_BUFFERSLAVE		0
#define I86_PIC_ICW4_BUF_MODEYES		8	// 1000
#define I86_PIC_ICW4_BUF_MODENO			0
#define I86_PIC_ICW4_SFNM_NESTEDMODE		0x10	// 10000
#define I86_PIC_ICW4_SFNM_NOTNESTED		0


// Send command to pics
inline void i86_pic_send_command (uint8_t cmd, uint8_t picNum){
	
	if(picNum>1)		// We have 2 pics (0 and 1)
		return;

	uint8_t reg = 0;
	
	if(picNum == 0)	
		reg = I86_PIC1_REG_COMMAND;
	else if(picNum == 1)
		reg = I86_PIC2_REG_COMMAND;


	outportb(reg,cmd);

}


// Send data to pics
inline void i86_pic_send_data(uint8_t data, uint8_t picNum){

	if(picNum>1)		// We have 2 pics (0 and 1)
		return;

	uint8_t reg = 0;

	if(picNum == 0)	
		reg = I86_PIC1_REG_DATA;
	else if(picNum == 1)
		reg = I86_PIC2_REG_DATA;


	outportb(reg,data);

}

// Read data from pics
inline uint8_t i86_pic_read_data(uint8_t picNum){

	if(picNum>1)		// We have 2 pics (0 and 1)
		return 0;

	uint8_t reg = 0;	

	if(picNum == 0)	
		reg = I86_PIC1_REG_DATA;
	else if(picNum == 1)
		reg = I86_PIC2_REG_DATA;


	return inportb(reg);

}


// Inizialite pics
void i86_pic_initialize(uint8_t base0, uint8_t base1){

	uint8_t icw = 0;
	
	// Disable hardware interrupts
	disable_interrupt();

	icw = (icw & ~I86_PIC_ICW1_MASK_INIT) | I86_PIC_ICW1_INIT_YES;
	icw = (icw & ~I86_PIC_ICW1_MASK_IC4) | I86_PIC_ICW1_IC4_EXPECT;

	// Send ICW1
	i86_pic_send_command(icw,0);
	i86_pic_send_command(icw,1);
	
	// Send ICW2 (base addr of the irq's)
	i86_pic_send_data(base0,0);
	i86_pic_send_data(base1,1);

	// Send ICW3 (connection between master and sleve)
	// (ICW3 Master PIC = IR that connect to 2nd PIC in binary format)
	// (ICW3 Slave PIC = IR that connect to 1st PIC in decimal format)
	i86_pic_send_data(0x04,0);
	i86_pic_send_data(0x02,1);

	// Send ICW4 (enable i86 mode)
	icw = (icw & ~I86_PIC_ICW4_MASK_UPM) | I86_PIC_ICW4_UPM_86MODE;

	i86_pic_send_data (icw,0);
	i86_pic_send_data (icw,1);
	
}

