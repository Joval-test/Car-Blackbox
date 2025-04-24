#include "main.h"
void write_24c02(unsigned char address, unsigned char data)
{
	i2c_start();
	i2c_write(0xA0);
	i2c_write(address);
	i2c_write(data);
	i2c_stop();
    for(unsigned long int i=5000;i>0;i--);
}

unsigned char read_24c02(unsigned char address)
{
	unsigned char data;

	i2c_start();
	i2c_write(0xA0);
	i2c_write(address);
	i2c_rep_start();
	i2c_write(0xA1);
	data = i2c_read();
	i2c_stop();

	return data;
}

static void get_time(void)
{
	clock_reg[0] = read_ds1307(HOUR_ADDR);
	clock_reg[1] = read_ds1307(MIN_ADDR);
	clock_reg[2] = read_ds1307(SEC_ADDR);

	if (clock_reg[0] & 0x40)
	{
		time[0] = '0' + ((clock_reg[0] >> 4) & 0x01);
		time[1] = '0' + (clock_reg[0] & 0x0F);
	}
	else
	{
		time[0] = '0' + ((clock_reg[0] >> 4) & 0x03);
		time[1] = '0' + (clock_reg[0] & 0x0F);
	}
	time[2] = ':';
	time[3] = '0' + ((clock_reg[1] >> 4) & 0x0F);
	time[4] = '0' + (clock_reg[1] & 0x0F);
	time[5] = ':';
	time[6] = '0' + ((clock_reg[2] >> 4) & 0x0F);
	time[7] = '0' + (clock_reg[2] & 0x0F);
	time[8] = '\0';
}

void dashboard() 
{
    get_time();
    clcd_print("TIME    EV  SP",LINE1(2));
    clcd_print(time,LINE2(0));
    speed_val=((read_adc(CHANNEL4))/10.25);
    secs=0;
   
    if(key == MK_SW1 && delay<1)
    {
        if(i==7)
        {
            i=1;
            event_capture();
        }
        else if(i!=6)
        {
            i++;
            event_capture();
        }
    }
    else if(key == MK_SW2 && delay<1)
    {
        if(i==7)
        {
            i=1;
            event_capture();
        }
        if(i!=1 && i>0)
        {
            i--;
            event_capture();
        }
    }
    else if(key==MK_SW3 && i!=7 && delay<1)
    {
        i=7;
        event_capture();
    }
    clcd_print(gear[i],LINE2(10));
    clcd_putch(speed_val/10+'0',LINE2(14));
    clcd_putch(speed_val%10+'0',LINE2(15));
    
}


void event_capture(void)
{
    if(no_event<=10)
    {
        ++no_event;
    }
    if(addr>=99)
    {
        event_flag=1;
        addr=0x00;
    }
    if(event_flag)
    {
        line++;
        if(line>10)
        {
            line=0;
        }
    }
    for(int j = 0;j<8;j++)
    {
        if(j==2 ||j==5)
        {
            continue;
        }
        write_24c02(addr++,time[j]);
    }
    write_24c02(addr++,gear[i][0]);
    write_24c02(addr++,gear[i][1]);
    write_24c02(addr++,speed_val/10+'0');
    write_24c02(addr++,speed_val%10+'0');
}

void authorisation(void)
{
    unsigned char usr_pass[5];

    if (count < 4) 
    {
        clcd_print("Enter Password", LINE1(0));
        if(secs==3)
        {
            key_flag=0;
            count=0;
            flag=3;
            CLEAR_DISP_SCREEN;
        }
        if (key == MK_SW11 && delay < 1) 
        {
            secs=0;
            sec_flag=0;
            usr_pass[count] = '0';
            clcd_putch('*', LINE2(count));
            count++;
        } 
        else if (key == MK_SW12 && delay < 1) 
        {
            secs=0;
            sec_flag=0;
            usr_pass[count] = '1';
            clcd_putch('*', LINE2(count));
            count++;
        }
        if (delay1++ == 5) 
        {
            clcd_putch('_', LINE2(count));
        } 
        else if (delay1 == 10) 
        {
            delay1 = 0;
            clcd_putch(' ', LINE2(count));
        }
    }
    else if (count == 4) 
    {
        usr_pass[count] = '\0';

        if (!(strcmp(org_pass, usr_pass))) 
        {
            CLEAR_DISP_SCREEN;
            main_flag = 1;
            key_flag = 0;
            count = 0;
            attempt = 3;
            return;
        } 
        else 
        {
            attempt--;
            CLEAR_DISP_SCREEN;
            for (unsigned int i = 0; i < 3000; i++)
            {
                clcd_print("INCORRECT PASS", LINE1(0));
                clcd_print("ATTEMPT LEFT - ", LINE2(0));
                clcd_putch(attempt + '0', LINE2(15));
            }
            CLEAR_DISP_SCREEN;
            count = 0;
        }
    }
    if (attempt == 0) 
    {
        static int once = 1;
        if(once)
        {
            sec = 120;
            once =0;
        }
        count = 5;
        clcd_print("WAIT FOR - ", LINE1(0));
        clcd_putch((sec % 10) + '0', LINE2(3));
        clcd_putch(((sec / 10) % 10) + '0', LINE2(2));
        clcd_putch((sec / 100) + '0', LINE2(1));
    }

    if (sec == 0) 
    {
        flag = 1;
        key_flag = 0;
        attempt = 3;
        count=0;
        sec = 120;
        CLEAR_DISP_SCREEN;
        main_flag = 0;
        return;
    }
    
}

void disp_menu()
{
    if(star_flag==0)
    {
        clcd_putch('*',LINE1(0));
        clcd_putch(' ',LINE2(0));
    }
    else
    {
        clcd_putch(' ',LINE1(0));
        clcd_putch('*',LINE2(0));
    }
    clcd_print(menu[main_count],LINE1(1));
    clcd_print(menu[main_count+1],LINE2(1));
    if(key==MK_SW13)
    {
        if(star_flag==0)
        {
            open_option(main_count);
        }
        else
        {
            open_option(main_count+1);
        }
    }
    if(key == MK_SW12)
    {
        menu_flag=1;
        if(star_flag==0)
        {
            star_flag=1;
        }
        else if(main_count<3)
        {
            main_count++;
            CLEAR_DISP_SCREEN;
        }
    }
    if(key==MK_SW11)
    {
        menu_flag=1;
        if(star_flag==1)
        {
            star_flag=0;
        }
        else if(main_count>0)
        {
            main_count--;
            CLEAR_DISP_SCREEN;
        }
    }
    if(key==MK_SW14)
    {
        main_flag=0;
        key_flag=0;
        CLEAR_DISP_SCREEN;
    }
}

void open_option(int num)
{
    switch(num)
    {
        case 0:
            main_flag=2;
            break;
        case 1:
            main_flag=3;
            break;
        case 2:
            main_flag=4;
            break;
        case 3:
            main_flag=5;
            break;
        case 4:
            main_flag=6;
            break;
                
    }
    CLEAR_DISP_SCREEN;
}

void view_log()
{
    line1=line;
    if(line1>9)
    {
        line1=0;
    }
    if(line1<0)
    {
        line1=9;
    }
    if(no_event==0)
    {
        for(unsigned int i=0;i<4000;i++)
        {
            clcd_print("No events found",LINE1(0));   
        }
        main_flag=1;
        CLEAR_DISP_SCREEN;
    }
    else
    {
        int view_addr = log_count * 10;
        clcd_print("LOGS", LINE1(5));
        clcd_putch(log_count + 48, LINE2(0));
        clcd_print(" ", LINE2(1));
        for (int j = 2; j < 10; j++) 
        {
            if (j == 4 || j == 7) 
            {
                clcd_putch(':', LINE2(j));
            }
            else 
            {
                data = read_24c02(view_addr++);
                clcd_putch(data, LINE2(j));
            }
        }
        clcd_print("  ", LINE2(10));
        data = read_24c02(view_addr++);
        clcd_putch(data, LINE2(11));
        data = read_24c02(view_addr++);
        clcd_putch(data, LINE2(12));
        clcd_print("  ", LINE2(13));
        data = read_24c02(view_addr++);
        clcd_putch(data, LINE2(14));
        data = read_24c02(view_addr++);
        clcd_putch(data, LINE2(15));
        if (key == MK_SW11 && (log_count > 0)) 
        {
            line1--;
            log_count--;
        }
        if (key == MK_SW12 && (log_count < no_event)) 
        {
            line1++;
            log_count++;
        }
    }
    if (key == MK_SW14) 
    {
        main_flag = 1;
        CLEAR_DISP_SCREEN;
    }
}
void download_log() 
{
    init_uart();
    char arr[17];
    if (no_event == 0) 
    {
        puts("LOG EMPTY");
        for (unsigned int i = 0; i < 4000; i++)
             clcd_print("Empty log", LINE1(0));
        main_flag = 1;
        CLEAR_DISP_SCREEN;
    } 
    else 
    {
        for (unsigned int i = 0; i < 6000; i++);
        {
            clcd_print("DOWNLOADING", LINE1(3));
            clcd_print("LOGS...", LINE2(3));
        }
        for (int i = 0; i < no_event; i++) 
        {
            arr[0] = i + 48;
            arr[1] = ' ';
            for (int j = 2; j < 10; j++) 
            {
                if (j == 4 || j == 7) 
                {
                    arr[j] = ':';
                }
                else
                {
                    data = read_24c02(add++);
                    arr[j] = data;
                }
            }
            for (int k = 10; k < 16; k++) 
            {
                if(k==10||k==13)
                {
                    arr[k]=' ';
                }
                else
                {
                    data = read_24c02(add++);
                    arr[k] = data;
                }
            }
            arr[16] = '\0';
            puts(arr);
            puts("\n\r");
            for(unsigned long int i=0;i<5000;i++);
        }
        CLEAR_DISP_SCREEN;
        main_flag = 1;    
        for(unsigned int i=0;i<4000;i++);
        {
            clcd_print("DOWNLOAD DONE", LINE1(1));
        }
        CLEAR_DISP_SCREEN;
    }
}

void clear_log()
{
    for(unsigned int i=0;i<4000;i++)
    {
        clcd_print("Clearing logs...",LINE1(0));
    }
    no_event=0;
    addr=0;
    main_flag=1;
    CLEAR_DISP_SCREEN;
    for(unsigned int i=0;i<4000;i++)
    {
        clcd_print("Logs deleted",LINE1(0));
    }
    CLEAR_DISP_SCREEN;
}

void set_time()
{
    clcd_print("HH:MM:SS",LINE1(3));
    
    if(key==MK_SW12)
    {
        time_field++;
        if(time_field==3)
        {
            time_field=0;
        }
    }
    if(time_field==0)
    {
        if(delay1<500)
        {
            clcd_putch(' ',LINE2(3));
            clcd_putch(' ',LINE2(4));
            delay1++;
        }
        else if(delay1<1000)
        {
            
            clcd_print(time,LINE2(3));
            delay1++;
        }
        else
        {
            delay1=0;
        }
    }
    else if(time_field==1)
    {
        if(delay2<500)
        {
            clcd_putch(' ',LINE2(6));
            clcd_putch(' ',LINE2(7));
            delay2++;
        }
        else if(delay2<1000)
        {
            clcd_print(time,LINE2(3));
            delay2++;
        }
        else
        {
            delay2=0;
        }
    }
    else if(time_field==2)
    {
        if(delay3<500)
        {
            clcd_putch(' ',LINE2(9));
            clcd_putch(' ',LINE2(10));
            delay3++;
        }
        else if(delay3<1000)
        {
            clcd_print(time,LINE2(3));
            delay3++;
        }
        else
        {
            delay3=0;
        }
        
    }
    if(key==MK_SW11)
    {
        if(time_field==0)
        {
            if(time[0]<'2')
            {
                if(time[1]<'9')
                {
                    time[1]++;
                }
                else
                {
                    time[1]='0';
                    time[0]++;
                }
            }
            else
            {
                if(time[1]<'3')
                {
                    time[1]++;
                }
                else
                {
                    time[1]='0';
                    time[0]='0';
                }
            }
        }
        if(time_field==1)
        {
            if(time[3]<'6')
            {
                if(time[4]<'9')
                {
                    time[4]++;
                }
                else
                {
                    time[3]++;
                    time[4]='0';
                    if(time[3]=='6')
                    {
                        time[3]='0';
                    }
                }
            }
            else
            {
                time[3]='0';
                time[4]='0';
            }
        }
        if(time_field==2)
        {
            if(time[6]<'6')
            {
                if(time[7]<'9')
                {
                    time[7]++;
                }
                else if(time[7]=='9')
                {
                    time[7]='0';
                    time[6]++;
                    if(time[6]=='6')
                    {
                        time[6]='0';
                    }
                }
            }
            else
            {
                    time[6]='0';
                    time[7]='0';
            }
        }
    }
    if(key==MK_SW13)
    {
        t=(time[0]-'0'<<4)|(time[1]-'0');
        write_ds1307(HOUR_ADDR,t);
        t=(time[3]-'0'<<4)|(time[4]-'0');
        write_ds1307(MIN_ADDR,t);
        t=(time[6]-'0'<<4)|(time[7]-'0');
        write_ds1307(SEC_ADDR,t);
        CLEAR_DISP_SCREEN;
        for(unsigned int i=0;i<4000;i++)
        {
            clcd_print("Time saved",LINE1(0));
        }
        main_flag=1;
        time_field=0;
        CLEAR_DISP_SCREEN;
	}
    if(key==MK_SW14)
    {
        CLEAR_DISP_SCREEN;
        for(unsigned int i=0;i<4000;i++)
        {
            clcd_print("Time not saved",LINE1(0));
        }
        CLEAR_DISP_SCREEN;
        main_flag=1;
        time_field=0;
    }
}

void change_pass()
{
    if(count < 4 && pass_flag==0)
    {
        clcd_print("Enter New Passwd", LINE1(0));
        if (key == MK_SW11) 
        {
            usr_pass[count] = '0';
            clcd_putch('*', LINE2(count));
            count++;
        } 
        else if (key == MK_SW12) 
        {
            usr_pass[count] = '1';
            clcd_putch('*', LINE2(count));
            count++;
        }
        if (delay1++ == 5) 
        {
            clcd_putch('_', LINE2(count));
        } 
        else if (delay1 == 10) 
        {
            delay1 = 0;
            clcd_putch(' ', LINE2(count));
        }
        else
        {
            delay1=0;
        }
    }

    else if (count==4 && pass_flag==0) 
    {
        usr_pass[count] = '\0';
        count=0;
        CLEAR_DISP_SCREEN;
        pass_flag=1;  
    }
    
    if (count1<4 && pass_flag==1) 
    {
        clcd_print("Re-Enter Pass", LINE1(0));
        if (key == MK_SW11) 
        {
            re_usr_pass[count1] = '0';
            clcd_putch('*', LINE2(count1));
            count1++;
        } 
        else if (key == MK_SW12 ) 
        {
            re_usr_pass[count1] = '1';
            clcd_putch('*', LINE2(count1));
            count1++;
        }
        if (delay1++ == 5) 
        {
            clcd_putch('_', LINE2(count1));
        } 
        else if (delay1 == 10) 
        {
            delay1 = 0;
            clcd_putch(' ', LINE2(count1));
        }
        else
        {
            delay1=0;
        }
    }

    if (count1==4 && pass_flag==1) 
    {
        re_usr_pass[count1] = '\0';
        if (strcmp(re_usr_pass, usr_pass) == 0) 
        {
            unsigned char address=200;
            for(int i=0;i<4;i++)
            {
                write_24c02(address++,usr_pass[i]);
            }
            CLEAR_DISP_SCREEN;
            for (unsigned int i = 0; i < 4000; i++)
            {
                clcd_print("Password change", LINE1(0));
                clcd_print("SUCCESSFULL", LINE2(0));
            }
            main_flag = 1;
            pass_flag=0;
            count1=0;
            CLEAR_DISP_SCREEN;
        } 
        else 
        {
            CLEAR_DISP_SCREEN;
            for (unsigned int i = 0; i < 4000; i++)
            {
                clcd_print("Incorrect pass", LINE1(0));
                clcd_print("FAILED", LINE2(0));
            
            }
            pass_flag=0;
            main_flag = 1;
            count1=0;
            CLEAR_DISP_SCREEN;
        }
    }
}