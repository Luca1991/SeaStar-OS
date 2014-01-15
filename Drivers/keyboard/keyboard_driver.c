#include <string.h>
#include <ctype.h>
#include <Hal.h>
#include <drivers/keyboard_driver.h>

extern void i86_keyboard_irq(); // ASM INTERRUPT FUNCTION

enum KEYBOARD_ENCODER_IO{
	KEYBOARD_ENC_INPUT_BUF = 0x60,
	KEYBOARD_ENC_CMD_REG = 0x60
};

enum KEYBOARD_ENC_CMDS{
	KEYBOARD_ENC_CMD_SET_LED = 0xED,
	KEYBOARD_ENC_CMD_ECHO = 0xEE,
	KEYBOARD_ENC_CMD_SCAN_CODE_SET = 0xF0,
	KEYBOARD_ENC_CMD_ID = 0xF2,
	KEYBOARD_ENC_CMD_AUTODELAY = 0xF3,
	KEYBOARD_ENC_CMD_ENABLE = 0xF4,
	KEYBOARD_ENC_CMD_RESETWAIT = 0xF5,
	KEYBOARD_ENC_CMD_RESETSCAN = 0xF6,
	KEYBOARD_ENC_CMD_ALL_AUTO = 0xF7,
	KEYBOARD_ENC_CMD_ALL_MAKEBREAK = 0xF8,
	KEYBOARD_ENC_CMD_ALL_MAKEONLY = 0xF9,
	KEYBOARD_ENC_CMD_MAKEBREAK_AUTO = 0xFA,
	KEYBOARD_ENC_CMD_SINGLE_AUTOREPEAT = 0xFB,
	KEYBOARD_ENC_CMD_SINGLE_MAKEBREAK = 0xFC,
	KEYBOARD_ENC_CMD_SINGLE_BREAKONLY = 0xFD,
	KEYBOARD_ENC_CMD_RESEND = 0xFE,
	KEYBOARD_ENC_CMD_RESET = 0xFF
};

enum KEYBOARD_CTRL_IO{
	KEYBOARD_CTRL_STATS_REG = 0x64,
	KEYBOARD_CTRL_CMD_REG = 0x64
};

enum KEYBOARD_CTRL_STATS_MASK{
	KEYBOARD_CTRL_STATS_MASK_OUT_BUF = 1,		// 00000001
	KEYBOARD_CTRL_STATS_MASK_IN_BUF = 2,		// 00000001
	KEYBOARD_CTRL_STATS_MASK_SYSTEM = 4,		// And so on..
	KEYBOARD_CTRL_STATS_MASK_CMD_DATA = 8,
	KEYBOARD_CTRL_STATS_MASK_LOCKED = 0x10,
	KEYBOARD_CTRL_STATS_MASK_AUX_BUF = 0x20,
	KEYBOARD_CTRL_STATS_MASK_TIMEOUT = 0x40,
	KEYBOARD_CTRL_STATS_MASK_PARITY = 0x80
};

enum KEYBOARD_CTRL_CMDS{
	KEYBOARD_CTRL_CMD_READ = 0x20,
	KEYBOARD_CTRL_CMD_WRITE = 0x60,
	KEYBOARD_CTRL_CMD_SELF_TEST = 0xAA,
	KEYBOARD_CTRL_CMD_INTERFACE_TEST = 0xAB,
	KEYBOARD_CTRL_CMD_DISABLE = 0xAD,
	KEYBOARD_CTRL_CMD_ENABLE = 0xAE,
	KEYBOARD_CTRL_CMD_READ_IN_PORT = 0xC0,
	KEYBOARD_CTRL_CMD_READ_OUT_PORT = 0xD0,
	KEYBOARD_CTRL_CMD_WRITE_OUT_PORT = 0xD1,
	KEYBOARD_CTRL_CMD_READ_TEST_INPUTS = 0xE0,
	KEYBOARD_CTRL_CMD_SYSTEM_RESET = 0xFE,
	KEYBOARD_CTRL_CMD_MOUSE_DISABLE = 0xA7,
	KEYBOARD_CTRL_CMD_MOUSE_ENABLE = 0xA8,
	KEYBOARD_CTRL_CMD_MOUSE_PORT_TEST = 0xA9,
	KEYBOARD_CTRL_CMD_MOUSE_WRITE = 0xD4	
};

enum KEYBOARD_ERROR{
	KEYBOARD_ERR_BUF_OVERRUN = 0,
	KEYBOARD_ERR_ID_RET = 0x83AB,
	KEYBOARD_ERR_BAT = 0xAA,
	KEYBOARD_ERR_ECHO_RET = 0xEE,
	KEYBOARD_ERR_ACK = 0xFA,
	KEYBOARD_ERR_BAT_FAILED = 0xFC,
	KEYBOARD_ERR_DIAG_FAILED = 0xFD,
	KEYBOARD_ERR_RESEND_CMD = 0xFE,
	KEYBOARD_ERR_KEY = 0xFF
};

static char _scancode;
static int _numlock, _scrolllock, _capslock;
static int _shift, _alt, _ctrl;
static int _kkeyboard_bat_response = 0;
static int _kkeyboard_diag_response = 0;
static int _kkeyboard_resend_response = 0;
static int _kkeyboard_disable = 0;

static int _kkeyboard_scancode_std [] = {
	KEY_UNKNOWN, // 0
	KEY_ESCAPE, // 1
	KEY_1,	//2
	KEY_2,	//3
	KEY_3, 	//4
	KEY_4,  //5
	KEY_5,  //6
	KEY_6,	//7
	KEY_7,	//8
	KEY_8,	//9
	KEY_9,	//Oxa
	KEY_0,	//0xb
	KEY_MINUS, //0xc
	KEY_EQUAL, //0xd
	KEY_BACKSPACE,	//0xe
	KEY_TAB,	//0xf
	KEY_Q,	//0x10
	KEY_W,	//0x11
	KEY_E,	//0x12
	KEY_R,	//0x13
	KEY_T,	//0x14
	KEY_Y,	//0x15
	KEY_U,	//0x16
	KEY_I,	//0x17
	KEY_O,	//0x18
	KEY_P,	//0x19
	KEY_LEFTBRACKET, //0x1a
	KEY_RIGHTBRACKET, //0x1b
	KEY_RETURN, //0x1c
	KEY_LCTRL, //0x1d
	KEY_A, //0x1e
	KEY_S, //0x1f
	KEY_D, //0x20
	KEY_F, //0x21
	KEY_G, //0x22
	KEY_H, //0x23
	KEY_J, //0x24
	KEY_K, //0x25
	KEY_L, //0x26
	KEY_SEMICOLON, //0x27
	KEY_QUOTE, //0x28
	KEY_GRAVE, //0x29
	KEY_LSHIFT, //0x2a
	KEY_BACKSLASH, //0x2b
	KEY_Z, //0x2c
	KEY_X, //0x2d
	KEY_C, //0x2e
	KEY_V, //0x2f
	KEY_B, //0x30
	KEY_N, //0x31
	KEY_M, //0x32
	KEY_COMMA, //0x33
	KEY_DOT, //0x34
	KEY_SLASH, //0x35
	KEY_RSHIFT, //0x36
	KEY_KP_ASTERISK, //0x37
	KEY_RALT, //0x38
	KEY_SPACE, //0x39
	KEY_CAPSLOCK, //0x3a
	KEY_F1, //0x3b
	KEY_F2, //0x3c
	KEY_F3, //0x3d
	KEY_F4, //0x3e
	KEY_F5, //0x3f
	KEY_F6, //0x40
	KEY_F7, //0x41
	KEY_F8, //0x42
	KEY_F9, //0x43
	KEY_F10, //0x44
	KEY_KP_NUMLOCK, //0x45
	KEY_SCROLLLOCK, //0x46
	KEY_HOME, //0x47
	KEY_KP_8, //0x48
	KEY_PAGEUP, //0x49
	KEY_KP_2, //0x50
	KEY_KP_3, //0x51
	KEY_KP_0, //0x52
	KEY_KP_DECIMAL, //0x53
	KEY_UNKNOWN, //0x54
	KEY_UNKNOWN, //0x55
	KEY_UNKNOWN, //0x56
	KEY_F11, //0x57
	KEY_F12 //0x58
};

const int INVALID_SCANCODE = 0;

uint8_t keyboard_ctrl_read_status();
void keyboard_ctrl_send_cmd(uint8_t cmd);
uint8_t keyboard_enc_read_buf();
void keyboard_enc_send_cmd(uint8_t);
void i86_keyboard_irq();

// Read status from keyboard controller
uint8_t keyboard_ctrl_read_status(){
	return inportb(KEYBOARD_CTRL_STATS_REG);
}

// Send command to keyboard controller
void keyboard_ctrl_send_cmd(uint8_t cmd){
	// we need to wait for the keyboard input buffer to be clear
	while(1)
		if((keyboard_ctrl_read_status() & KEYBOARD_CTRL_STATS_MASK_IN_BUF) == 0)
			break;
	// now that we know that the input buffer is clear, we can send our command
	outportb(KEYBOARD_CTRL_CMD_REG,cmd);
}

// Read keyboard encoder buffer
uint8_t keyboard_enc_read_buf(){
	return inportb(KEYBOARD_ENC_INPUT_BUF);
}

// Send command to keyboard encoder
void keyboard_enc_send_cmd(uint8_t cmd){
	// we need to wait for the keyboard controller input buffer to be clear because in order to send a cmd to the encoder, the message have to pass from the 		   controller too..
	while(1)
		if((keyboard_ctrl_read_status() & KEYBOARD_CTRL_STATS_MASK_IN_BUF) == 0)
			break;
	// now that we know that the input buffer is clear, we can send our command
	outportb(KEYBOARD_ENC_CMD_REG,cmd);

}

//keyboard interrupt handler
void i86_keyboard_irq_c(){
	static int extendedscancode = 0;
	
	int code = 0;
	// Read the scancode only if the controller output buffer is full
	if(keyboard_ctrl_read_status() & KEYBOARD_CTRL_STATS_MASK_OUT_BUF){
		code = keyboard_enc_read_buf(); // Read scancode	
		if(code == 0xE0 || code == 0xE1) // check if this is an extended scancode
			extendedscancode = 1;	
		else{      
			extendedscancode = 0; //single scancode, or second part of extended scancode
			if(code & 0x80){ // is this a break code ? (break code = key released!!)
				code -= 0x80; // convert break code into its make code equivalent
				int key = _kkeyboard_scancode_std[code]; // grab the key
				switch(key){ // check if a special key has been released and set it
					case KEY_LCTRL:
					case KEY_RCTRL:
						_ctrl = false;
						break;
					case KEY_LSHIFT:
					case KEY_RSHIFT:
						_shift = false;
						break;				
				}
			}
			else{    // this is a make code (make code = key pressed!!)
				_scancode = code;
				int key = _kkeyboard_scancode_std[code]; // grab the key
				switch(key){	// test if the user is holding down a special key and set it
					case KEY_LCTRL:
					case KEY_RCTRL:
						_ctrl = 1;
						break;
					case KEY_LSHIFT:
					case KEY_RSHIFT:
						_shift = 1;
						break;
					case KEY_LALT:
					case KEY_RALT:
						_alt = 1;
						break;
					case KEY_CAPSLOCK:
						_capslock = (_capslock==1) ? 0 : 1;
						kkeyboard_set_leds (_numlock, _capslock, _scrolllock);
						break;
					case KEY_KP_NUMLOCK:
						_numlock = (_numlock==1) ? 0 : 1;
						kkeyboard_set_leds(_numlock, _capslock, _scrolllock);
						break;
					case KEY_SCROLLLOCK:
						_scrolllock = (_scrolllock==1) ? 0 : 1;
						kkeyboard_set_leds(_numlock, _capslock, _scrolllock);
				}
			}
		}
		switch(code){ // Error check
			case KEYBOARD_ERR_BAT_FAILED:
				_kkeyboard_bat_response = 0;
				break;
			case KEYBOARD_ERR_DIAG_FAILED:
				_kkeyboard_diag_response = 0;
				break;
			case KEYBOARD_ERR_RESEND_CMD:
				_kkeyboard_resend_response = 1;
				break;		
		}	
	}

	interruptdone(0); // Interrupt done
}

int kkeyboard_get_scroll_lock(){
	return _scrolllock;
}

int kkeyboard_get_numlock(){
	return _numlock;
}

int kkeyboard_get_capslock(){
	return _capslock;
}

int kkeboard_get_ctrl(){
	return _ctrl;
}

int kkeyboard_get_alt(){
	return _alt;
}

int kkeyboard_get_shift(){
	return _shift;
}

void kkeyboard_ignore_resend(){
	_kkeyboard_resend_response = 0;
}

int kkeyboard_check_resend(){
	return _kkeyboard_resend_response;
}

int kkeyboard_get_diagnostic_respose(){
	return _kkeyboard_diag_response;
}

int kkeyboard_get_bat_response(){
	return _kkeyboard_bat_response;
}

uint8_t kkeyboard_get_last_scan(){
	return _scancode;
}

void kkeyboard_set_leds (int num, int caps, int scroll){
	uint8_t data = 0;
	
	data = (scroll==1) ? (data | 1) : (data & 1);
	data = (num==1) ? (num | 2) : (num & 2);
	data = (caps==1) ? (num | 4) : (num & 4);

	keyboard_enc_send_cmd(KEYBOARD_ENC_CMD_SET_LED);
	keyboard_enc_send_cmd(data);
}

KEYCODE kkeyboard_get_last_key(){
	return (_scancode!=INVALID_SCANCODE) ? ((KEYCODE)_kkeyboard_scancode_std[(KEYCODE)_scancode]) : (KEY_UNKNOWN);
}

void kkeyboard_discard_last_key(){
	_scancode = INVALID_SCANCODE;
}

char kkeyboard_key_to_ascii (KEYCODE code){
	uint8_t key = code;

	if(isascii(key)){
		if(_shift==1 || _capslock==1)
			if(key >= 'a' && key <= 'z')
				key -= 32;
		if((_shift==1) && (_capslock==0)){
			if(key >= '0' && key <= '9')
				switch(key){
					case '0':
						key = KEY_RIGHTPARENTHESIS;
						break;
					case '1':
						key = KEY_EXCLAMATION;
						break;
					case '2':
						key = KEY_AT;
						break;
					case '3':
						key = KEY_EXCLAMATION;
						break;
					case '4':
						key = KEY_HASH;
						break;
					case '5':
						key = KEY_PERCENT;
						break;
					case '6':
						key = KEY_CARRET;
						break;
					case '7':
						key = KEY_AMPERSAND;
						break;
					case '8':
						key = KEY_ASTERISK;
						break;
					case '9':
						key = KEY_LEFTPARENTHESIS;
						break;
				}
			else{
				switch(key){
					case KEY_COMMA:
						key = KEY_LESS;
						break;
					case KEY_DOT:
						key = KEY_GREATER;
						break;
					case KEY_SLASH:
						key = KEY_QUESTION;
						break;
					case KEY_SEMICOLON:
						key = KEY_COLON;
						break;
					case KEY_QUOTE:
						key = KEY_QUOTEDOUBLE;
						break;
					case KEY_LEFTBRACKET:
						key = KEY_LEFTCURL;
						break;
					case KEY_RIGHTBRACKET:
						key = KEY_RIGHTCURL;
						break;
					case KEY_GRAVE:
						key = KEY_TILDE;
						break;
					case KEY_MINUS:
						key = KEY_UNDERSCORE;
						break;
					case KEY_PLUS:
						key = KEY_EQUAL;
						break;
					case KEY_BACKSLASH:
						key = KEY_BAR;
						break;
				}		
			}
		}
	
		return key; //return ascii char
			
	}
	return 0; // scancode is not a valid ascii char
}

void kkeyboard_disable(){
	keyboard_ctrl_send_cmd(KEYBOARD_CTRL_CMD_DISABLE);
	_kkeyboard_disable = 1;
}

void kkeyboard_enable(){
	keyboard_ctrl_send_cmd(KEYBOARD_CTRL_CMD_ENABLE);
	_kkeyboard_disable = 0;
}

int kkeyboard_is_disabled(){
	return _kkeyboard_disable;
}

void kkeyboard_reset_system(){
	keyboard_ctrl_send_cmd(KEYBOARD_CTRL_CMD_WRITE_OUT_PORT);
	keyboard_enc_send_cmd(0xfe);
}

int kkeyboard_self_test(){
	keyboard_ctrl_send_cmd(KEYBOARD_CTRL_CMD_SELF_TEST);
	while(1)
		if(keyboard_ctrl_read_status() & KEYBOARD_CTRL_STATS_MASK_OUT_BUF)
			break;

	return (keyboard_enc_read_buf() == 0x55) ? 1 : 0;
}

void kkeyboard_install (int irq){

	setvect(irq, i86_keyboard_irq);

	_kkeyboard_bat_response = 1;
	_scancode = 0;
	_numlock = _scrolllock = _capslock = 0;
	kkeyboard_set_leds(_numlock, _capslock, _scrolllock);
	_shift = _alt = _ctrl = 0;
		
}
