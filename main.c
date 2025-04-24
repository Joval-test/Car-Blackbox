/*
 * File:   main.c
 * Author: Joval
 *
 * Created on 28 March, 2024, 2:06 PM
 */
#include "main.h"

static void init_config()
{
    init_adc();
    init_matrix_keypad();
    init_clcd();
    init_timer0();
    init_i2c();
	init_ds1307();
//    address=200;
//    for(int i=0;i<4;i++)
//    {
//        write_24c02(address++,'0');
//    }
    address=200;
    for(int i=0;i<4;i++)
    {
        data = read_24c02(address++);
        org_pass[i]=data;
    }
}

void main(void) 
{
    init_config();
    dashboard();
    while(1)
    {
        key=read_switches(STATE_CHANGE);
        while(key==MK_SW12 && main_flag>0)
        {
            key = read_switches(LEVEL_CHANGE);
            
            if(delay++ >= 50000)
            {
                delay=0;
                key=MK_SW14;
                CLEAR_DISP_SCREEN;
            }
        }
        if(delay>500 && delay<50000)
        {
            key=MK_SW12;
            delay=0;
        }
        while(key == MK_SW11 && main_flag>0)
        {
            key = read_switches(LEVEL_CHANGE);
            if(delay++ >= 50000)
            {
                delay=0;
                key=MK_SW13;
                CLEAR_DISP_SCREEN;
            }
        }
        if(delay>500 && delay<50000)
        {
            key=MK_SW11;
            delay=0;
        }
        if(key==MK_SW11 && key_flag==0 && main_flag==0)
        {
            key_flag=1;
            key=0xff;
            CLEAR_DISP_SCREEN;
        }
        if(key_flag==1)
        {
            authorisation();
        }
        else if(main_flag>0)
        {
            
            if(main_flag==1)
            {
                disp_menu();   
            }
            else if(main_flag==2)
            {
                view_log();
            }
            else if(main_flag==3)
            {
                download_log();
            }
            else if(main_flag==4)
            {
                clear_log();
            }
            else if(main_flag==5)
            {
                set_time();
            }
            else if(main_flag==6)
            {
                change_pass();
            }
        }
        else
        {
            dashboard();
        }
       
    }
}
