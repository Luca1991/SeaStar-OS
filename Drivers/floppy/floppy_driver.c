#include <Hal.h>
#include <drivers/floppy_driver.h>

extern void i86_floppy_irq(); // ASM IRQ function

// Floppy disk controller main I/0 ports
enum FLOPPYDISK_IO{
	FLOPPYDISK_DOR = 0x3f2,
	FLOPPYDISK_MSR = 0x3f4,
	FLOPPYDISK_FIFO = 0x3f5,
	FLOPPYDISK_CTRL = 0x3f7
};

// Not extended floppy disk controller commands (bits 0-4)
enum FLOPPYDISK_CMD{
	FDC_CMD_READ_TRACK = 2,
	FDC_CMD_SPECIFY = 3,
	FDC_CMD_CHECK_STAT = 4,
	FDC_CMD_WRITE_SECT = 5,
	FDC_CMD_READ_SECT = 6,
	FDC_CMD_CALIBRATE = 7,
	FDC_CMD_CHECK_INT = 8,
	FDC_CMD_FORMAT_TRACK = 0xd,
	FDC_CMD_SEEK = 0xf
};

// Additional extended floppy disk controller commands (bits 5-7)
enum FLOPPYDISK_CMD_EXT{
	FDC_CMD_EXT_SKIP = 0x20,
	FDC_CMD_EXT_DENSITY = 0x40,
	FDC_CMD_EXT_MULTITRACK = 0x80
};

// DOR = Digital Output Register
enum FLOPPYDISK_DOR_MASK{
	FLOPPYDISK_DOR_MASK_DRIVE0 = 0,		// 00000000
	FLOPPYDISK_DOR_MASK_DRIVE1 = 1,         // 00000001
	FLOPPYDISK_DOR_MASK_DRIVE2 = 2,		// 00000010
	FLOPPYDISK_DOR_MASK_DRIVE3 = 3,         // 00000011
	FLOPPYDISK_DOR_MASK_RESET = 4,          // 00000100
	FLOPPYDISK_DOR_MASK_DMA = 8,            // 00001000
	FLOPPYDISK_DOR_MASK_DRIVE0_MOTOR = 16,  // 00010000
	FLOPPYDISK_DOR_MASK_DRIVE1_MOTOR = 32,  // 00100000
	FLOPPYDISK_DOR_MASK_DRIVE2_MOTOR = 64,  // 01000000
	FLOPPYDISK_DOR_MASK_DRIVE3_MOTOR = 128  // 10000000
};

// MSR = Main Status Register 
enum FLOPPYDISK_MSR_MASK{
	FLOPPYDISK_MSR_MASK_DRIVE1_POS_MODE = 1, // 00000001
	FLOPPYDISK_MSR_MASK_DRIVE2_POS_MODE = 2, // 00000010
	FLOPPYDISK_MSR_MASK_DRIVE3_POS_MODE = 4, // 00000100
	FLOPPYDISK_MSR_MASK_DRIVE4_POS_MODE = 8, // 00001000
	FLOPPYDISK_MSR_MASK_BUSY = 16,           // 00010000 
	FLOPPYDISK_MSR_MASK_DMA = 32,            // 00100000
	FLOPPYDISK_MSR_MASK_DATAIO = 64,         // 01000000
	FLOPPYDISK_MSR_MASK_DATAREG = 128        // 10000000
};

// ST0 = Status Port 0
enum FLOPPYDISK_ST0_MASK{
	FLOPPYDISK_ST0_MASK_DRIVE0 = 0,		// 00000000
	FLOPPYDISK_ST0_MASK_DRIVE1 = 1,         // 00000001
	FLOPPYDISK_ST0_MASK_DRIVE2 = 2, 	// 00000010
	FLOPPYDISK_ST0_MASK_DRIVE3 = 3,		// 00000011
	FLOPPYDISK_ST0_MASK_HEADACTIVE = 4,	// 00000100
	FLOPPYDISK_ST0_MASK_NOTREADY = 8,	// 00001000
	FLOPPYDISK_ST0_MASK_UNITCHECK = 16,	// 00010000
	FLOPPYDISK_ST0_MASK_SEEKEND = 32,	// 00100000
	FLOPPYDISK_ST0_MASK_INTCODE = 64	// 01000000
};

// ST0 intcode types
enum FLOPPYDISK_ST0_INTCODE_TYPE{
	FLOPPYDISK_ST0_TYPE_NORMAL = 0,
	FLOPPYDISK_ST0_TYPE_ABNORMAL_ERR = 1,
	FLOPPYDISK_ST0_TYPE_INVALID_ERR = 2,
	FLOPPYDISK_ST0_TYPE_NOTREADY = 3
};

// GAP3 Sizes (GAP3 = Spaces between sectors on physical disk)
enum FLOPPYDISK_GAP3_LENGTH{
	FLOPPYDISK_GAP3_LENGTH_STD = 42, 
	FLOPPYDISK_GAP3_LENGTH_5_14 = 32,
	FLOPPYDISK_GAP3_LENGTH_3_5 = 27 // This is the 3.5'' GAP3 length
};


// Floppy disk sectors (formula 2^sectornumber *128)
enum FLOPPYDISK_SECTOR_DTL{
	FLOPPYDISK_SECTOR_DTL_128 = 0,
	FLOPPYDISK_SECTOR_DTL_256 = 1,
	FLOPPYDISK_SECTOR_DTL_512 = 2,
	FLOPPYDISK_SECTOR_DTL_1024 = 4
};

const int FLOPPY_IRQ = 6; // floppy irq
const int FLOPPY_SECTORS_PER_TRACK = 18; // 18 Sectros per track
int DMA_BUFFER = 0; // dma transfer buffer addr.. it can be changed using floppydisk_set_dma(int addr)..
const int FDC_DMA_CHANNEL = 2; // FDC uses DMA CHANNEL 2!
extern void sleep(int sec); 
static uint8_t _CurrentDrive = 0;
static volatile uint8_t _FloppyDiskIRQ = 0;

// Initialize DMA
int dma_init_floppydisk(uint8_t* buffer, unsigned length){
	union{
		uint8_t byte[4];
		unsigned long l;	
	}a,c;

	a.l=(unsigned)buffer;
	c.l=(unsigned)length-1;
	// Sanity Check (Check for buffer issues) 
	if((a.l >> 24) || (c.l >>16) || (((a.l & 0xffff)+c.l)>>16)){
		return 0;
	}

	dma_reset(1);
	dma_mask_channel(FDC_DMA_CHANNEL); // Mask channel 2
	dma_reset_flipflop(1); // Reset flipflop of DMAC 1 
	dma_set_address(FDC_DMA_CHANNEL, a.byte[0],a.byte[1]); // Buffer addr
	dma_reset_flipflop(1);
	dma_set_count(FDC_DMA_CHANNEL,c.byte[0],c.byte[1]); // Count
	dma_set_read(FDC_DMA_CHANNEL);
	dma_unmask_all(1);

	return 1;
}

void floppydisk_set_dma(int addr){
	DMA_BUFFER = addr;
}


// Return floppy disk controller status
uint8_t floppydisk_read_status(){
	return inportb(FLOPPYDISK_MSR);
}

// Write to DOR Register (Digital Output Register)
void floppydisk_write_dor(uint8_t val){
	outportb(FLOPPYDISK_DOR,val);
}


// send command byte to floppy disk controller
void floppydisk_send_command(uint8_t cmd){
	int i=0;	
	for(i=0;i<500;i++) //wait until the datareg is ready
		if(floppydisk_read_status() & FLOPPYDISK_MSR_MASK_DATAREG)
			return outportb(FLOPPYDISK_FIFO,cmd);
}

// read data from floppy disk controller
uint8_t floppydisk_read_data(){
	int i=0;
	for(i=0;i<500;i++) //wait until the datareg is ready
		if(floppydisk_read_status() & FLOPPYDISK_MSR_MASK_DATAREG)
			return inportb(FLOPPYDISK_FIFO);
	return 0;
}

// write configuration control register
void floppydisk_write_ccr(uint8_t val){
	outportb(FLOPPYDISK_CTRL,val); 
}

// Wait for IRQ
inline int floppydisk_wait_for_irq(){
	int timer = 0; // we use a timer to detect a timeout
	while(_FloppyDiskIRQ==0 && timer<=10){
		sleep(20);
		timer++;
	}
	_FloppyDiskIRQ = 0;
	if(timer>=10)
		return 0; // Timeout! We fail to init the floppy disk drive
	else
		return 1;
}

// This is called from the asm floppy irq handler
void i86_floppy_irq_c(){
	_FloppyDiskIRQ=1;
	interruptdone(FLOPPY_IRQ);
}

// Check interrupt status command
void floppydisk_check_int(uint32_t* st0, uint32_t* cyl){
	floppydisk_send_command(FDC_CMD_CHECK_INT);

	*st0 = floppydisk_read_data();
	*cyl = floppydisk_read_data();
}

// Turn the current floppy disk drive motor on/off
void floppydisk_control_motor(int b){
	if(_CurrentDrive > 3 ) // Check if the current drive is valid
		return;

	uint32_t motor = 0;
	
	switch(_CurrentDrive){
		case 0:
			motor = FLOPPYDISK_DOR_MASK_DRIVE0_MOTOR;
			break;
		case 1: 
			motor = FLOPPYDISK_DOR_MASK_DRIVE1_MOTOR;
			break;
		case 2:
			motor = FLOPPYDISK_DOR_MASK_DRIVE2_MOTOR;
			break;
		case 3:
			motor = FLOPPYDISK_DOR_MASK_DRIVE3_MOTOR;
			break;
	}		
	
	// Turn on/off the motor of the current floppy drive
	if(b==1)
		floppydisk_write_dor((uint8_t)(_CurrentDrive | motor | FLOPPYDISK_DOR_MASK_RESET | FLOPPYDISK_DOR_MASK_DMA));
	else
		floppydisk_write_dor(FLOPPYDISK_DOR_MASK_RESET);
	// wait 20ms for the motor to spin up or turn off
	sleep(20);
}

// Configure floppy disk drive
void floppydisk_drive_data (uint8_t stepr, uint8_t loadt, uint8_t unloadt, int dma){
	uint8_t data = 0;
		

	floppydisk_send_command(FDC_CMD_SPECIFY); 
	data = ((stepr & 0xf)<<4) | (unloadt & 0xf);
	floppydisk_send_command(data);
	
	data = ((loadt << 1) | (dma==1)?0:1); 
	floppydisk_send_command(data);
}

int floppydisk_calibrate(uint8_t drive){
	uint32_t st0, cyl;
	int i = 0;
	
	if(drive >= 4) // Check if the drive is valid
		return -2;
	
	floppydisk_control_motor(1); // turn on the floppy drive motor
	for(i=0; i<10; i++){
		floppydisk_send_command(FDC_CMD_CALIBRATE);
		floppydisk_send_command(drive);
		floppydisk_wait_for_irq();
		floppydisk_check_int(&st0,&cyl);

		if(cyl==0){
			floppydisk_control_motor(0);
			return 0; // success! we found cylinder 0!
		} 
	}

	floppydisk_control_motor(0);
	return -1; // Failed to find cylinder 0!
}

// Disable floppy disk controller
void floppydisk_disable_controller(){
	floppydisk_write_dor(0);
}

// Enable floppy disk controller
void floppydisk_enable_controller(){
	floppydisk_write_dor ( FLOPPYDISK_DOR_MASK_RESET | FLOPPYDISK_DOR_MASK_DMA);
}

// Reset floppy disk controller
int floppydisk_reset(){
	uint32_t st0, cyl;
	int i = 0;
	int check_for_timeout = 0;	

	floppydisk_disable_controller();
	floppydisk_enable_controller();
	
	check_for_timeout = floppydisk_wait_for_irq(); 
	
	if(!check_for_timeout)
		return 0;
	for(i=0;i<4;i++)
		floppydisk_check_int(&st0,&cyl); // check int/sense interrupt to all drivers

	floppydisk_write_ccr(0); // set transfer speed to 500kb/s (standard)

	floppydisk_drive_data(3,16,240,1); // mechanical drive info.. steprade = 3ms, loadtime =16ms, unloadtime=240ms

	floppydisk_calibrate(_CurrentDrive); // Calibrate the disk

	return 1;
}	

// Read Sector using CHS format
void floppydisk_read_sector_imp(uint8_t head, uint8_t track, uint8_t sector){
	uint32_t st0,cyl;
	int i =0;
	
	dma_init_floppydisk((uint8_t*)DMA_BUFFER,512); // set DMA for read transfer
	dma_set_read(FDC_DMA_CHANNEL);
	floppydisk_send_command(FDC_CMD_READ_SECT | FDC_CMD_EXT_MULTITRACK | FDC_CMD_EXT_SKIP | FDC_CMD_EXT_DENSITY);
	floppydisk_send_command (head << 2 | _CurrentDrive);
	floppydisk_send_command (track);
	floppydisk_send_command (head);
	floppydisk_send_command (sector);
	floppydisk_send_command (FLOPPYDISK_SECTOR_DTL_512);
	floppydisk_send_command (((sector+1) >= FLOPPY_SECTORS_PER_TRACK) ? FLOPPY_SECTORS_PER_TRACK : sector +1);
	floppydisk_send_command (FLOPPYDISK_GAP3_LENGTH_3_5);
	floppydisk_send_command (0xff);
	floppydisk_wait_for_irq();

	for(i=0;i<7;i++)
		floppydisk_read_data();

	floppydisk_check_int(&st0,&cyl); // Tell to the FDC that we handled the interrupt
}

int floppydisk_seek (uint8_t cyl, uint8_t head){
	uint32_t st0, cyl0;
	int i=0;	

	if(_CurrentDrive >= 4) // Check if the drive is valid
		return -1;

	for(i=0;i<10;i++){
		floppydisk_send_command(FDC_CMD_SEEK);
		floppydisk_send_command((head) << 2 | _CurrentDrive);
		floppydisk_send_command(cyl);

		floppydisk_wait_for_irq();
		floppydisk_check_int(&st0,&cyl0);

		if(cyl0 == cyl)
			return 0; // We found the desired track/cylinder !
	}
	
	return -1; // We fail to find the desired track/cylinder !
}

// Convert LBA format to CHS format
void floppydisk_lba2chs (int lba, int *head, int *track, int *sector){
	*head = ( lba % (FLOPPY_SECTORS_PER_TRACK * 2)) / (FLOPPY_SECTORS_PER_TRACK);
	*track = lba / (FLOPPY_SECTORS_PER_TRACK * 2);
	*sector = lba % FLOPPY_SECTORS_PER_TRACK + 1;
}

// Install floppy disk driver
int floppydisk_install(int irq){
	int timeout=0;
	setvect(irq, i86_floppy_irq,0); // install irq handler

	timeout=floppydisk_reset(); // reset floppy drive controller
	
	if(!timeout)
		return 0;
	floppydisk_drive_data(13,1,0xf,1); // set drive informations

	return 1;
}

// Set current working drive
void floppydisk_set_working_drive (uint8_t drive){
	if(drive<4) // Check if drive is a valid drive
		_CurrentDrive = drive;
}

// Get current working drive
uint8_t floppydisk_get_working_drive(){
	return _CurrentDrive;
}

int is_fdd_present(){ // check if a valid floppy drive is present
	unsigned char check;      
	outportb(0x70, 0x10);
	sleep(100); // wait a moment before reading the cmos response
	check = inportb(0x71);
	check = check >> 4;   // Get the master fdd state

	if(check==0)	      // if c == 0, no floppy drive found..
		return 0;
	else
		return 1;    // else, floppy drive is present
}

// Read sector using LBA format
uint8_t* floppydisk_read_sector(int secLBA){
	if(_CurrentDrive >= 4) // Check if we are using a valid floppy drive
		return 0;

	int check_fdd = is_fdd_present();
	
	if(!check_fdd)
		return 0;

	int head=0, track=0, sector=1;
	floppydisk_lba2chs(secLBA,&head,&track,&sector); // convert LBA to CHS

	// turn on motor and seek to track
	floppydisk_control_motor(1);
	if(floppydisk_seek((uint8_t)track,(uint8_t)head)!=0)
		return 0;

	floppydisk_read_sector_imp((uint8_t)head,(uint8_t)track,(uint8_t)sector); // read the sector
	floppydisk_control_motor(0); // turn off motor
	
	return (uint8_t*) DMA_BUFFER;

}
