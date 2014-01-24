#include <Hal.h>
#include "dma.h"

void dma_mask_channel(uint8_t chan){
	if(chan <=4)
		outportb(DMA0_CHANMASK_REG,(1<<(chan-1)));
	else
		outportb(DMA1_CHANMASK_REG,(1<<(chan-5)));
}

void dma_unmask_channel(uint8_t chan){
	if(chan <=4)
		outportb(DMA0_CHANMASK_REG,chan);
	else
		outportb(DMA1_CHANMASK_REG,chan);
}

void dma_unmask_all(int dma){
	if(dma==0)
		outportb(DMA0_CLEAR_MASK_REG,0xff);
	else if(dma==1)
		outportb(DMA1_CLEAR_MASK_REG,0xff);
}

void dma_reset(int dma){
	if(dma==0)
		outportb(DMA0_INTER_REG,0xff);
	else if(dma==1)
		outportb(DMA1_INTER_REG,0xff);
}

void dma_reset_flipflop(int dma){
	if(dma>=2)  
		return;
	outportb((dma==0) ? DMA0_CLEARBYTE_FLIPFLOP_REG : DMA1_CLEARBYTE_FLIPFLOP_REG, 0xff);
}

void dma_set_address(uint8_t chan,uint8_t low,uint8_t high){
	if(chan>8)
		return;
	unsigned short port = 0;
	switch(chan){
		case 0:
			port = DMA0_CHAN0_ADDR_REG;
			break;
		case 1:
			port = DMA0_CHAN1_ADDR_REG;
			break;
		case 2:
			port = DMA0_CHAN2_ADDR_REG;
			break;
		case 3:
			port = DMA0_CHAN3_ADDR_REG;
			break;
		case 4:
			port = DMA1_CHAN4_ADDR_REG;
			break;
		case 5:
			port = DMA1_CHAN5_ADDR_REG;
			break;
		case 6:
			port = DMA1_CHAN6_ADDR_REG;
			break;
		case 7:
			port = DMA1_CHAN7_ADDR_REG;
			break;
	}

	outportb(port,low);
	outportb(port,high);
}

void dma_set_count(uint8_t chan, uint8_t low, uint8_t high){
	if(chan>8)
		return;

	unsigned short port = 0;
	switch(chan){
		case 0:
			port = DMA0_CHAN0_COUNT_REG;
			break;
		case 1:
			port = DMA0_CHAN1_COUNT_REG;
			break;
		case 2:
			port = DMA0_CHAN2_COUNT_REG;
			break;
		case 3:
			port = DMA0_CHAN3_COUNT_REG;
			break;
		case 4:
			port = DMA1_CHAN4_COUNT_REG;
			break;
		case 5:
			port = DMA1_CHAN5_COUNT_REG;
			break;
		case 6:
			port = DMA1_CHAN6_COUNT_REG;
			break;
		case 7:
			port = DMA1_CHAN7_COUNT_REG;
			break;
	}

	outportb(port,low);
	outportb(port,high);

}

void dma_set_mode(uint8_t chan, uint8_t mode){
	int dma = (chan < 4) ? 0:1;
	int fchan = (dma==0) ? chan : chan-4;

	dma_mask_channel(chan);
	outportb((chan<4) ? DMA0_MODE_REG : DMA1_MODE_REG, fchan | mode);
	dma_unmask_all(dma);
}

void dma_set_read(uint8_t chan){
	dma_set_mode(chan, DMA_MODE_READ_TRANSFER | DMA_MODE_TRANSFER_SINGLE);
}

void dma_set_write(uint8_t chan){
	dma_set_mode(chan, DMA_MODE_WRITE_TRANSFER | DMA_MODE_TRANSFER_SINGLE);
}

void dma_set_external_page_register(uint8_t reg, uint8_t val){
	if(reg>14)
		return;

	unsigned short port = 0;
	switch(reg){
		case 1: 
			port = DMA_PAGE_CHAN1_ADDRBYTE2;
			break;
		case 2:
			port = DMA_PAGE_CHAN2_ADDRBYTE2;
			break;
		case 3:
			port = DMA_PAGE_CHAN3_ADDRBYTE2;
			break;
		case 4:
			return; // This register is used to connect slave DMAc to Master DMAc
		case 5:
			port = DMA_PAGE_CHAN5_ADDRBYTE2;
			break;
		case 6:
			port = DMA_PAGE_CHAN6_ADDRBYTE2;
			break;
		case 7:
			port = DMA_PAGE_CHAN7_ADDRBYTE2;
			break;
	}
	
	outportb(port,val);
	
}
