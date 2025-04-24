#ifndef MAIN_H
#define MAIN_H
#include <xc.h>
#include "clcd.h"
#include "eeprom.h"
#include "matrix_keypad.h"
#include "adc.h"
#include <string.h>
#include "timer0.h"
#include "i2c.h"
#include "ds1307.h"
#include "uart.h"

int i=0;
unsigned char key;
char gear[8][3]={"ON","GN","GR","G1","G2","G3","G4","CO"};
char menu[5][16]={"VIEW LOG","DOWNLOAD LOG","CLEAR LOG","SET TIME","CHANGE PASSWD"};  
unsigned char org_pass[5];
unsigned char usr_pass[5];
unsigned char re_usr_pass[5];
char time[9];
char edit_time[9];
unsigned int speed_val=0;
unsigned char addr=0x00;
unsigned char address;
unsigned char tok;
unsigned char data;
unsigned int sec=120;
unsigned short event_flag=0;
unsigned short key_flag=0;
unsigned short star_flag=0;
unsigned short main_flag=0;
unsigned short menu_flag=0;
unsigned short pass_flag=0;
unsigned short main_count=0;
unsigned char clock_reg[3];
int attempt = 3, pass = 0;
unsigned short flag=1;
char count = 0;
char count1 = 0;
char log_count=0;
unsigned short no_event=0;
unsigned long int delay = 0;
unsigned long int delay1 = 0;
unsigned long int delay2 = 0;
unsigned long int delay3 = 0;
unsigned short line=0;
unsigned short line1=0;
unsigned char add = 0;
unsigned short time_field=0;
unsigned short sec_flag=0;
int line_count=0;
unsigned int hr=0;
unsigned int min=0;
unsigned int secs=0;
unsigned char t=0;

void dashboard(void);
void event_capture(void);
void authorisation(void);
void disp_menu(void);
void open_option(int);
void view_log(void);
void download_log(void);
void clear_log(void);
void set_time(void);
void change_pass(void);


#endif