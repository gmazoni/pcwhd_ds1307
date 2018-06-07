# pcwhd_ds1307
PCWHD Compiler schematic to communicate with DS1307 RTC

Example of usage:

#define RTC_SDA PIN_B0

#define RTC_SCL PIN_B1

char dds_e[8][4] = {"","Seg","Ter","Qua","Qui","Sex","Sab","Dom"};
 
ds1307_get_date();
ds1307_get_time();
 
printf(lcd_putc,"\fHoje eh %s\n%02d:%02d:%02d - %02d/%02d",dds_e[dds],hor,min,seg,dia,mes);
