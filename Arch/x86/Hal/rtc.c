#include <Hal.h>
#include <rtc.h>


enum CMOS{
	CMOS_CMD_ADDR = 0x70,
	CMOS_DATA_ADDR  = 0x71
};

enum RTC{
      	RTC_SECOND_REG = 0x00,
	RTC_MINUTE_REG  = 0x02,
	RTC_HOUR_REG  = 0x04,
	RTC_DAY_REG  = 0x07,
	RTC_MONTH_REG  = 0x08,
	RTC_YEAR_REG  = 0x09,
	RTC_CENTURY_REG  = 0x32,
	RTC_STATUS_REG_A = 0x0A,
	RTC_STATUS_REG_B = 0x0B
};

unsigned char second;
unsigned char minute;
unsigned char hour;
unsigned char day;
unsigned char month;
int year;

 

int get_update_in_progress_flag() {
	outportb(CMOS_CMD_ADDR, RTC_STATUS_REG_A);
	return (inportb(CMOS_DATA_ADDR) & 0x80);
}
 
 
unsigned char get_RTC_reg(int reg) {
	outportb(CMOS_CMD_ADDR, reg);
	return inportb(CMOS_DATA_ADDR);
}
 
 
void read_rtc(unsigned char *fhour,unsigned char *fminute,unsigned char *fsecond,unsigned char *fday,unsigned char *fmonth,int *fyear) {



	int century;
	unsigned char last_second;
	unsigned char last_minute;
	unsigned char last_hour;
	unsigned char last_day;
	unsigned char last_month;
	int last_year;
	unsigned char last_century;
	unsigned char registerB;
 
      // We need to check twice for the same values in order to avoid inconsistent results
      
 
	while (get_update_in_progress_flag());                // Make sure an update isn't in progress
	second = get_RTC_reg(RTC_SECOND_REG);
	
	minute = get_RTC_reg(RTC_MINUTE_REG);
	hour = get_RTC_reg(RTC_HOUR_REG);
	day = get_RTC_reg(RTC_DAY_REG);
	month = get_RTC_reg(RTC_MONTH_REG);
	year = get_RTC_reg(RTC_YEAR_REG);
	 
	century = get_RTC_reg(RTC_CENTURY_REG);

 
	do{
		
		last_second = second;
		last_minute = minute;
		last_hour = hour;
		last_day = day;
		last_month = month;
		last_year = year;
		last_century = century;
 
		while (get_update_in_progress_flag());           // Make sure an update isn't in progress
		second = get_RTC_reg(RTC_SECOND_REG);
		minute = get_RTC_reg(RTC_MINUTE_REG);
		hour = get_RTC_reg(RTC_HOUR_REG);
		day = get_RTC_reg(RTC_DAY_REG);
		month = get_RTC_reg(RTC_MONTH_REG);
		year = get_RTC_reg(RTC_YEAR_REG);
		if(RTC_CENTURY_REG != 0) 
			century = get_RTC_reg(RTC_CENTURY_REG);

	} while( (last_second != second) || (last_minute != minute) || (last_hour != hour) ||
               (last_day != day) || (last_month != month) || (last_year != year) ||
               (last_century != century) );
 
	registerB = get_RTC_reg(RTC_STATUS_REG_B);
 
	// Convert BCD to binary values if necessary

	if (!(registerB & 0x04)) {
		second = (second & 0x0F) + ((second / 16) * 10);
		minute = (minute & 0x0F) + ((minute / 16) * 10);
		hour = ( (hour & 0x0F) + (((hour & 0x70) / 16) * 10) ) | (hour & 0x80);
		day = (day & 0x0F) + ((day / 16) * 10);
		month = (month & 0x0F) + ((month / 16) * 10);
		year = (year & 0x0F) + ((year / 16) * 10);
	
		century = (century & 0x0F) + ((century / 16) * 10);
		
	}
 
	
	year += century * 100; // Set the full 4 digit year
	
	
	
	*fhour = hour;
	*fminute = minute;
	*fsecond = second;
	*fday = day;
	*fmonth = month;
	*fyear = year;
	

}


