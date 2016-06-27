#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <signal.h>

void handler (int sig)
{
	printf ("nexiting...(%d)n", sig);
	//	exit (0);
}

void perror_exit (char *error)
{
	perror (error);
	handler (9);
}


int main (int argc, char *argv[])
{
	struct input_event ev[64];
	int fdk, rd, value, size = sizeof (struct input_event);
	char name[256] = "Unknown";
	char *kbdDevice = NULL;
	char *mouseDevice = NULL;
	char buf[2];

	char eventCode2Ascii[256];

	//int i2c_type;	// 0-kbd 1-mouse 2-Mouse-buttons
	//int i2c_code;	// 0-mouseX / release button
	// 1-mouseY / press  button 
	//int i2c_value;	// travel distance or key code
	int kbd_action;

	fd_set fds;

	signal(SIGKILL, SIG_IGN);
	signal(SIGSTOP, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGINT, SIG_IGN);

	eventCode2Ascii[1] 	= 0xb1;	//Esc 
	eventCode2Ascii[2] 	= 0x31;	//1  
	eventCode2Ascii[3] 	= 0x32;	//2  
	eventCode2Ascii[4] 	= 0x33;	//3  
	eventCode2Ascii[5] 	= 0x34;	//4  
	eventCode2Ascii[6] 	= 0x35;	//5  
	eventCode2Ascii[7] 	= 0x36;	//6  
	eventCode2Ascii[8] 	= 0x37;	//7  
	eventCode2Ascii[9] 	= 0x38;	//8  
	eventCode2Ascii[10] 	= 0x39;	//9  
	eventCode2Ascii[11] 	= 0x30;	//0  
	eventCode2Ascii[12] 	= 0x2d;	//-  
	eventCode2Ascii[13] 	= 0x3d;	//=  
	eventCode2Ascii[14] 	= 0x08;	//BS  

	eventCode2Ascii[15] 	= 0xB3;	//Tab 
	eventCode2Ascii[16] 	= 0x71;	//q  
	eventCode2Ascii[17] 	= 0x77;	//w  
	eventCode2Ascii[18] 	= 0x65;	//e  
	eventCode2Ascii[19] 	= 0x72;	//r  
	eventCode2Ascii[20] 	= 0x74;	//t  
	eventCode2Ascii[21] 	= 0x79;	//y  
	eventCode2Ascii[22] 	= 0x75;	//u  
	eventCode2Ascii[23] 	= 0x69;	//i  
	eventCode2Ascii[24] 	= 0x6f;	//o  
	eventCode2Ascii[25] 	= 0x70;	//p  
	eventCode2Ascii[26] 	= 0x7b;	//[  
	eventCode2Ascii[27] 	= 0x7d;	//]  
	eventCode2Ascii[28] 	= 0xb0;	//Enter  


	eventCode2Ascii[29] 	= 0x80;	//LCTRL
	eventCode2Ascii[30] 	= 0x61;	//a  
	eventCode2Ascii[31] 	= 0x73;	//s  
	eventCode2Ascii[32] 	= 0x64;	//d  
	eventCode2Ascii[33] 	= 0x66;	//f  
	eventCode2Ascii[34] 	= 0x67;	//g  
	eventCode2Ascii[35] 	= 0x68;	//h  
	eventCode2Ascii[36] 	= 0x6a;	//j  
	eventCode2Ascii[37] 	= 0x6b;	//k  
	eventCode2Ascii[38] 	= 0x6c;	//l  
	eventCode2Ascii[39] 	= 0x3b;	//;  
	eventCode2Ascii[40] 	= 0x27;	//'  
	eventCode2Ascii[43] 	= 0x5c;	//\  


	eventCode2Ascii[41] 	= 0x60;	//`
	eventCode2Ascii[42] 	= 0x81;	//LShift
	eventCode2Ascii[43] 	= 0x5c;	//Back slash 
	eventCode2Ascii[44] 	= 0x7a;	//z  
	eventCode2Ascii[45] 	= 0x78;	//x  
	eventCode2Ascii[46] 	= 0x63;	//c  
	eventCode2Ascii[47] 	= 0x76;	//v  
	eventCode2Ascii[48] 	= 0x62;	//b  
	eventCode2Ascii[49] 	= 0x6e;	//n  
	eventCode2Ascii[50] 	= 0x6d;	//m  
	eventCode2Ascii[51] 	= 0x2c;	//,  
	eventCode2Ascii[52] 	= 0x2e;	//.  
	eventCode2Ascii[53] 	= 0x2f;	///  
	eventCode2Ascii[54] 	= 0x85;	//RShift  

	eventCode2Ascii[56] 	= 0x82;	//LAlt
	eventCode2Ascii[57] 	= 0x20;	//Space  
	eventCode2Ascii[58] 	= 0xc1;	//Caps Lock  
	eventCode2Ascii[59] 	= 0xc2;	//F1 
	eventCode2Ascii[60] 	= 0xc3;	//F2 
	eventCode2Ascii[61] 	= 0xc4;	//F3 
	eventCode2Ascii[62] 	= 0xc5;	//F4 
	eventCode2Ascii[63] 	= 0xc6;	//F5 
	eventCode2Ascii[64] 	= 0xc7;	//F6 
	eventCode2Ascii[61] 	= 0xc8;	//F7 
	eventCode2Ascii[66] 	= 0xc9;	//F8 
	eventCode2Ascii[67] 	= 0xca;	//F9 
	eventCode2Ascii[68] 	= 0xcb;	//F10  
	eventCode2Ascii[68] 	= 0xcc;	//F11  
	eventCode2Ascii[68] 	= 0xcd;	//F12  

//	eventCode2Ascii[71] 	= 0xD2;	//Home  

	eventCode2Ascii[0x69] 	= 0xd8;	//Left  
	eventCode2Ascii[0x6a] 	= 0xd7;	//Right  
	eventCode2Ascii[0x6c] 	= 0xd9;	//Down  
	eventCode2Ascii[0x67] 	= 0xda;	//Up  

	eventCode2Ascii[0x66] 	= 0xd2;	//Home  
	eventCode2Ascii[0x6b] 	= 0xd5;	//End  
	eventCode2Ascii[0x6d] 	= 0xd6;	//Pg Down  
	eventCode2Ascii[0x68] 	= 0xd3;	//Pg Up  

	eventCode2Ascii[97] 	= 0x84;	//RCtrl  
	eventCode2Ascii[100] 	= 0x86;	//RAlt  
	eventCode2Ascii[110] 	= 0xd1;	//Insert  
	eventCode2Ascii[111] 	= 0xd4;	//Delete  

	eventCode2Ascii[125] 	= 0x83;	//LWin
	eventCode2Ascii[127] 	= 0x87;	//RWin

	//eventCode2Ascii[119] 	= 0x85;	//Pause  
	//eventCode2Ascii[69] 	= 0x85;	//Num Lock  
	//eventCode2Ascii[70] 	= 0x85;	//Scroll Lock  
	//eventCode2Ascii[99] 	= 0x85;	//PrintScr  




	//Setup check
	if (argv[1] == NULL){
		printf("Please specify (on the command line) the path to the dev event interface devicen");
		printf("See /proc/bus/input/devices\n");
		exit (0);
	}

	if ((getuid ()) != 0)
		printf ("You are not root! This may not work...n");




	//char *bus = "/dev/i2c-1"; // Pins P9_19 and P9_20 
	int addr = 0x9;          // The I2C address of TMP102 
	//float f,c;


	/*if ((fd_i2c = open(bus, O_RDWR)) < 0) {
		// ERROR HANDLING: you can check errno to see what went wrong 
		perror("Failed to open the i2c bus");
		exit(1);
	}

	else
	{
		perror("After open\n");
	}
	

	if (ioctl(fd_i2c, I2C_SLAVE, addr) < 0) {
		perror("Failed to acquire bus access and/or talk to slave.\n");
		// ERROR HANDLING; you can check errno to see what went wrong 
		exit(1);
	}

	else
	{
		perror("After open\n");
	}*/



	if (argc == 2)
	{
		kbdDevice = argv[1];
	}
	else
	{
		printf("Needs 1 device\n");
		exit;
	}

	//Open Keyboard
	if ((fdk = open (kbdDevice, O_RDONLY)) == -1)
		printf ("%s is not a vaild device.n", kbdDevice);

	printf ("After open\n");
	ioctl (fdk, EVIOCGNAME (sizeof (name)), name);
	printf ("Reading From : %d %s (%s)\n", fdk, kbdDevice, name);


	while (1)
	{
		FD_ZERO(&fds);
		FD_SET(fdk, &fds);

		select (fdk + 1, &fds, NULL, NULL, NULL);

		//printf("Select returns\n");

		if (FD_ISSET(fdk, &fds))
		{
			//printf("kbd has something to say\n");	
			if ((rd = read (fdk, ev, size * 64)) < size)
				perror_exit ("read()");     
			kbd_action = 1;
		}
		int i;
		for(i=0; i<rd/size; i++)
		{
			if (ev[i].type == 1)
			{
				printf ("%d of %d - Code[%x %x %x]\n", i, rd/size, ev[i].type, ev[i].code, ev[i].value);



				if ((ev[i].code >= 65) && (ev[i].code <= 68))	// Mouse keys F7 - F10
				{


					buf[0] = 2;	 // mouse action

					if (ev[i].code == 65) 	// X axis
					{
						buf[1] = 0;
						buf[2] = -5;
					}
					else if (ev[i].code == 66) 	// Y axis
					{
						buf[1] = 1;
						buf[2] = 5;
					}
					else if (ev[i].code == 67) 	// Y axis
					{
						buf[1] = 1;
						buf[2] = -5;
					}
					else if (ev[i].code == 68) 	// Y axis
					{
						buf[1] = 0;
						buf[2] = 5;
					}

				printf ("Mouse action - %x %x %x\n", buf[0], buf[1], buf[2]);

				}
				else if (ev[i].code == 58)	// Special hanling for caps lock
				{
					//printf("Special handling for caps lock\n");

					buf[0] = 3;	 // mouse action
					buf[1] 	= 0x10;
					buf[2] = ev[i].value;

				}
				else
				{
					buf[0] = 0;	 // keyboard action
					buf[1] = eventCode2Ascii[ev[i].code];  
					buf[2] = ev[i].value;

				}
				/*if (write(fd_i2c, buf, 3) != 3) 
				{
					perror("Failed to write to the i2c bus.\n");
				}*/

			}
		}

		continue;
		//	printf ("Code[%d %d %d]\n", ev[1].type, ev[1].code, ev[1].value);
		//printf ("Code[%x %x %x]\n", ev[2].type, ev[2].code, ev[2].value);
		//printf ("Code[%x %x %x]\n", ev[3].type, ev[3].code, ev[3].value);
		//printf ("Code[%x %x %x]\n", ev[4].type, ev[4].code, ev[4].value);
		//printf ("Code[%x %x %x]\n", ev[5].type, ev[5].code, ev[5].value);
		//printf("\n");

		if (ev[0].type == 0x11)
		{
			//printf("Special handling for caps lock\n");

			buf[0] = 3;	 // mouse action
			buf[1] 	= 0x10;
			buf[2] = ev[0].value ^ 1;
			////printf ("%x %x %x\n", buf[0], buf[1], buf[2]);

			/*if (write(fd_i2c, buf, 3) != 3) 
			{
				perror("Failed to write to the i2c bus.\n");
			}*/

		}
		if (ev[0].type == 4)
		{
			//printf("Mouse Button or keyboard Action\n");	
			//printf ("Code[%d %d %d]\n", ev[0].type, ev[0].code, ev[0].value);
			//printf ("Code[%d %d %d]\n", ev[1].type, ev[1].code, ev[1].value);

			if (kbd_action)	 	// kbd action
			{
				//printf("kbd_action true \n");

				if (ev[1].code == 58)	// Special hanling for caps lock
				{
					//printf("Special handling for caps lock\n");

					buf[0] = 3;	 // mouse action
					buf[1] 	= 0x10;
					buf[2] = ev[1].value;
					//printf ("%x %x %x\n", buf[0], buf[1], buf[2]);

					/*if (write(fd_i2c, buf, 3) != 3) 
					{
						perror("Failed to write to the i2c bus.\n");
					}*/

				}
				else
				{

					buf[0] = 0;	 // keyboard action
					buf[1] = eventCode2Ascii[ev[1].code];  
					buf[2] = ev[1].value;
					//printf (" buf %x %x %x\n\n", buf[0], buf[1], buf[2]);

					/*if (write(fd_i2c, buf, 3) != 3) 
					{
						perror("Failed to write to the i2c bus.\n");
					}*/

				}


			}

			else			// mouse action
			{
				buf[0] = 3;	 // mouse action
				buf[1] = ev[1].code - 256;
				buf[2] = ev[1].value;
				//printf ("%x %x %x\n", buf[0], buf[1], buf[2]);

				/*if (write(fd_i2c, buf, 3) != 3) 
				{
					perror("Failed to write to the i2c bus.\n");
				}*/
			}
		}

		else if (ev[0].type == 2)
		{
			//printf("Mouse movement or special mouse  button action\n");	

			buf[0] = 2;	 // mouse movement
			buf[1] = ev[0].code;
			buf[2] = ev[0].value;
			//printf ("%x %x %x\n", buf[0], buf[1], buf[2]);

			/*if (write(fd_i2c, buf, 3) != 3) 
			{
				perror("Failed to write to the i2c bus.\n");
			}*/

			if (ev[1].type == 2)
			{
				buf[0] = 2;	
				buf[1] = ev[1].code;
				buf[2] = ev[1].value;
				//	printf ("%x %x %x\n", buf[0], buf[1], buf[2]);

				/*if (write(fd_i2c, buf, 3) != 3) 
				{
					perror("Failed to write to the i2c bus.\n");
				}*/

			}


		}

		else if (ev[0].type == 1)
		{
			//printf("Keyboard action\n");	
		}

		else 
		{
			//printf("Unknown action\n");	
			//printf ("Code[%x %x %x]\n", ev[0].type, ev[0].code, ev[0].value);
			//printf ("Code[%x %x %x]\n", ev[1].type, ev[1].code, ev[1].value);
			//printf ("Code[%x %x %x]\n", ev[2].type, ev[2].code, ev[2].value);
			//printf ("Code[%x %x %x]\n", ev[3].type, ev[3].code, ev[3].value);
			//printf ("Code[%x %x %x]\n", ev[4].type, ev[4].code, ev[4].value);
			//printf ("Code[%x %x %x]\n", ev[5].type, ev[5].code, ev[5].value);
			//printf("\n");

		}

		//buf[0] = ev[1].value;  
		//buf[1] = eventCode2Ascii[ev[1].code];  

		// mouse


		////printf("code  %02x - value %02x\n", buf[0], buf[1]);
		////printf("character %c %x\n", buf, buf);
		/*if (write(fd_i2c, buf, 2) != 2) 
		  {
		  perror("Failed to write to the i2c bus.\n");
		  }*/
	}

	return 0;
} 

