/* PMF 2015
 *
 * udev api intro: http://www.signal11.us/oss/udev/
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <inttypes.h>
#include <poll.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <signal.h>
//#include <libudev.h>

//#include "udev.h"
#include "libcstuff.h"
#include "emukb.h"
#include "emumouse.h"
#include "keymap.h"


int isLWinDown = 0;

/* A real hid device plugged into this machine's host controller */
struct hid_in {
	/* The event device, typically /dev/input/eventX */
	const char *input_dev;

	const char *usbhid_sys_path;
};


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


//struct kbpt {
//	list_head phys_kb;
//
//	struct emu_kb emu_kb;
//};
//
//

int fd_keyboard = -1;
int fd_mouse = -1;

bool
register_physical_keyboard(struct hid_in *hin)
{
	/* Set up listening for events */

	int fd = open(hin->input_dev, O_RDONLY);
	if (fd == -1) {
		PERROR("open");
		return false;
	}

	int result = ioctl(fd, EVIOCGRAB, 1);
	if (result == -1) {
		PERROR("ioctl(EVIOCGRAB)");
		return false;
	}

	//list_add(kbpt.phys_kb, fd);

	fd_keyboard = fd;

	/* Ok, we have a physical keyboard all setup;
	 * now we can set up an emulated keyboard.
	 */

	//if (!register_emulated_keyboard()) {
	//	ERROR("failed to register emulated keyboard");
	//	return false;
	//}

	return true;
}

bool
register_physical_mouse(struct hid_in *hin)
{
	/* Set up listening for events */

	int fd = open(hin->input_dev, O_RDONLY);
	if (fd == -1) {
		PERROR("open");
		return false;
	}

	int result = ioctl(fd, EVIOCGRAB, 1);
	if (result == -1) {
		PERROR("ioctl(EVIOCGRAB)");
		return false;
	}

	fd_mouse = fd;

	return true;
}

/*bool
probe_device(struct udev_device *dev)
{
	DEBUG("probe: probing device at %s", udev_device_get_syspath(dev));

//	 1. Want an input device with an event character device 
	const char *event_dev = NULL;

	// 2. Want a usb device of driver usbhid 
	const char *usbhid_sys_path = NULL;

	// 3. capabilities/key 
	const char *capabilities_key = NULL;

	struct udev_device *orig_dev = dev;

	for (; dev != NULL; dev = udev_device_get_parent(dev)) {
		const char *subsys = udev_device_get_subsystem(dev);
		const char *driver = udev_device_get_driver(dev);

		if (!event_dev && subsys && !strcmp(subsys, "input")) {
			event_dev = udev_device_get_devnode(dev);
			// Could be NULL, in that case we'll try again
			 // at the next opportunity.
			 
		}

		if (!usbhid_sys_path && subsys && driver && !strcmp(subsys, "usb") && !strcmp(driver, "usbhid")) {
			usbhid_sys_path = udev_device_get_syspath(dev);
			// Could be NULL, in that case we'll try again
			 // at the next opportunity.
			 
		}

		if (!capabilities_key) {
			capabilities_key = udev_device_get_sysattr_value(dev, "capabilities/key");
		}

	}

	if (event_dev == NULL) {
		DEBUG("rejected device because we couldn't find a device for it");
		return false;
	}
	if (usbhid_sys_path == NULL) {
		DEBUG("rejected device because we couldn't find a hid parent for it");
		return false;
	}

	bool is_keyboard = false;
	bool is_mouse = false;
	// USB keyboards have a key bitmap that ends in ffffffff fffffffe 
	const char needle_keyboard_end[] = "ffffffff fffffffe";
	const char needle_mouse_begin[] = "70000 0 0 0 0 0 0 0 0";
	if (strlen(capabilities_key) >= sizeof(needle_keyboard_end) -1) {
		if (strcmp(capabilities_key + strlen(capabilities_key) - (sizeof(needle_keyboard_end) - 1), needle_keyboard_end) == 0) {
			is_keyboard = true;
			VERBOSE("Got a good keyboard (%s) [path=%s] [%s]", event_dev, udev_device_get_syspath(orig_dev), capabilities_key);

		} else {
			DEBUG("rejected device as keyboard because it doesn't have the right key capabilities (%s)", capabilities_key);
		}

	} else {
		DEBUG("rejected device as keyboard because it doesn't have the right key capabilities length");
	}

	if (strlen(capabilities_key) >= sizeof(needle_mouse_begin) -1) {
		if (strcmp(capabilities_key, needle_mouse_begin) == 0) {
			is_mouse = true;
			VERBOSE("Got a good mouse (%s) [path=%s] [%s]", event_dev, udev_device_get_syspath(orig_dev), capabilities_key);
		} else {
			DEBUG("rejected device as mouse because it doesn't have the right key capabilities (%s)", capabilities_key);
		}

	} else {
		DEBUG("rejected device as mouse because it doesn't have the right key capabilities length");
	}

	// Ok we have a usb device which resolved as hid and input.
	 // Still check if it's a type that interests us.
	 //

	// We have a device we're interested in 

	struct hid_in *hin = malloc(sizeof(struct hid_in));
	if (!hin) {
		OOM();
		return false;
	}

	hin->usbhid_sys_path = strdup(usbhid_sys_path);
	if (!hin->usbhid_sys_path) {
		OOM();
		return false;
	}

	hin->input_dev = strdup(event_dev);
	if (!hin->input_dev) {
		OOM();
		return false;
	}

	if (is_keyboard) {
		if (!register_physical_keyboard(hin)) {
			ERROR("failed to register keyboard");
			return false;
		}
	}

	// FIXME 
	if (is_mouse && fd_mouse == -1) {
		if (!register_physical_mouse(hin)) {
			ERROR("failed to register mouse");
			return false;
		}
	}

	return true;
}
*/

uint16_t modifiers[] = {
	KEY_LEFTCTRL,
	KEY_LEFTSHIFT,
	KEY_LEFTALT,
	KEY_LEFTMETA,
	KEY_RIGHTCTRL,
	KEY_RIGHTSHIFT,
	KEY_RIGHTALT,
	KEY_RIGHTMETA,
};

int8_t key_to_usb_modifier(uint16_t key)
{
	int i;
	for (i = 0; i < 8; i++) {
		if (key == modifiers[i]) {
			return i;
		}
	}

	return -1;
}

enum state {
	STATE_PASSTHROUGH = 0,
	STATE_COMMAND_INPUT = 1,
	STATE_KEYMAP_GET_FROM = 2,
	STATE_KEYMAP_GET_TO = 3,
} state = STATE_PASSTHROUGH;

bool process_command(const char *cmd)
{
	ERROR("Got command %s", cmd);

	if (!strcmp(cmd, "remap")) {
		if (!emukb_erase_chars(strlen(cmd))) {
			ERROR("failed to erase chars");
			return false;
		}

		if (!emukb_inject("Ok enter key to remap: ")) {
			ERROR("failed to inject to keyboard");
			return false;
		}
		state = STATE_KEYMAP_GET_FROM;
	} else if (!strcmp(cmd, "swap")) {
		emukb_use_aux = !emukb_use_aux;
		emumouse_use_aux = !emumouse_use_aux;
	} else {
		if (!emukb_erase_chars(strlen(cmd))) {
			ERROR("failed to erase chars");
			return false;
		}
	}

	return true;
}

char current_command[100];
size_t current_command_len = 0;

uint8_t keymap_from;

bool send_pressed_report_from_scancode(uint8_t modifier_state, int64_t scancode, bool is_modifier)
{
	char report[8];
	memset(report, 0, sizeof(report));

	report[0] = modifier_state;

	DEBUG("Have modifiers %" PRIu8, modifier_state);

	if (!is_modifier) {
		report[2] = scancode;
	}

	if (modifier_state == 2 + 4 + 8 + 32 + 64 + 128) {
		if (!emukb_inject(">")) {
			ERROR("failed to inject to keyboard");
			return false;
		}
		state = STATE_COMMAND_INPUT;
	}

	if (emukb_send_report(report, 8) == -1) {
		ERROR("failed to send usb report");
		return false;
	}

	return true;
}

bool key_pressed(uint8_t modifier_state, int64_t scancode, bool is_modifier)
{
	if (state == STATE_PASSTHROUGH) {
		if (scancode == 71) {
			emukb_use_aux = !emukb_use_aux;
			emumouse_use_aux = !emumouse_use_aux;
			return true;
		}

		uint8_t mapped;
		keymap_map(scancode, &mapped);
		send_pressed_report_from_scancode(modifier_state, mapped, is_modifier);
	} else if (state == STATE_COMMAND_INPUT) {
		char ascii;
		usb2ascii(scancode, &ascii);

		if (ascii == '\n') {
			/* Command done */
			current_command[current_command_len] = 0;
			state = STATE_PASSTHROUGH;
			process_command(current_command);
			current_command_len = 0;

			return true;
		}

		current_command[current_command_len++] = ascii;
		send_pressed_report_from_scancode(modifier_state, scancode, is_modifier);
		if (current_command_len == sizeof(current_command) - 1) {
			ERROR("command too long; resetting");
			current_command_len = 0;
		}
	} else if (state == STATE_KEYMAP_GET_FROM) {
		keymap_from = scancode;
		state = STATE_KEYMAP_GET_TO;
		if (!emukb_inject(" Ok; enter destination: ")) {
			ERROR("failed to inject to keyboard");
			return false;
		}
	} else if (state == STATE_KEYMAP_GET_TO) {
		keymap_remap(keymap_from, scancode);
		if (!emukb_inject(" Roger. ")) {
			ERROR("failed to inject to keyboard");
			return false;
		}
		state = STATE_PASSTHROUGH;
		if (!emukb_erase_injected()) {
			ERROR("failed to erase injected keys");
			return false;
		}
	}

	return true;
}

bool key_released(uint8_t modifier_state, int64_t scancode, bool is_modifier)
{
	if (state == STATE_PASSTHROUGH || state == STATE_COMMAND_INPUT) {
		char report[8];
		memset(report, 0, sizeof(report));

		report[0] = modifier_state;

		if (emukb_send_report(report, 8) == -1) {
			ERROR("failed to send usb report");
			return false;
		}
	}

	return true;
}

bool handle_keyboard_event(void)
{
	int result;
	struct input_event ev;
	static uint8_t modifier_state = 0;

	static int64_t current_key = 0;
	static int64_t current_scancode = 0;
	static int64_t current_act = -1;


	result = read(fd_keyboard, &ev, sizeof(struct input_event));
	if (result == -1) {
		PERROR("read");
		return false;
	}
	if (result == 0) {
		ERROR("Got end of file on evdev");
		return false;
	}

	if (ev.type == EV_SYN) {
		/* Done, check if we have a valid event */
		if (!current_key || !current_scancode || current_act == -1) {
			current_key = 0;
			current_scancode = 0;
			current_act = -1;
			return true;
		}
		DEBUG("%s linux key %" PRId64 " (usb %" PRId64 ")", current_act?"Pressed":"Released", current_key, current_scancode);

		int8_t mod = key_to_usb_modifier(current_key);

		if (current_act == 1) {
			if (mod != -1) {
				modifier_state |= (1 << mod);
			}
			key_pressed(modifier_state, current_scancode, (mod != -1));
		} else {
			if (mod != -1) {
				modifier_state &= ~(1 << mod);
			}
			key_released(modifier_state, current_scancode, (mod != -1));
		}

		current_key = 0;
		current_scancode = 0;
		current_act = -1;
	}

	if (ev.type == EV_KEY && (ev.value == 0 || ev.value == 1)) {
		current_key = ev.code;
		current_act = ev.value;
	}

	if (ev.type == EV_MSC && ev.code == MSC_SCAN) {
		current_scancode = ev.value & 0xffff;
	}

	return true;
}

bool handle_mouse_event(void)
{
	static uint8_t button[3] = { 0, 0, 0};
	int8_t wheel = 0;

	int result;
	struct input_event ev;

	result = read(fd_mouse, &ev, sizeof(struct input_event));
	if (result == -1) {
		PERROR("read");
		return false;
	}
	if (result == 0) {
		ERROR("Got end of file on evdev");
		return false;
	}

	DEBUG("Got a mouse event! %" PRIu16 " %" PRIu16 " %" PRId32, ev.type, ev.code, ev.value);
	int16_t x_movement = 0;
	int16_t y_movement = 0;

	if (ev.type == 2 && ev.code == 0) {
		x_movement = ev.value;
	}

	if (ev.type == 2 && ev.code == 1) {
		y_movement = ev.value;
	}
	if (ev.type == 2 && ev.code == 8) {
		wheel = ev.value;
	}
	if (ev.type == 1 && ev.code == BTN_LEFT) {
		button[0] = ev.value;
	}
	if (ev.type == 1 && ev.code == BTN_RIGHT) {
		button[1] = ev.value;
	}
	if (ev.type == 1 && ev.code == BTN_MIDDLE) {
		button[2] = ev.value;
	}


	uint8_t but = (button[0] << 0) | (button[1] << 1) | (button[2] << 2);

	if (emumouse_send_report(but, x_movement, y_movement, wheel) == -1) {
		ERROR("failed to send usb report");
		return false;
	}

	//if (emumouse_send_report(0, 0, 0) == -1) {
	//	ERROR("failed to send usb report");
	//	return false;
	//}

	return true;
}

bool input_loop(void)
{

	
	size_t n_poll_fds = 0;
	struct pollfd pfd[2];
	if (fd_keyboard != -1) {
		pfd[n_poll_fds].fd = fd_keyboard;
		pfd[n_poll_fds].events = POLLIN;
		n_poll_fds++;
	} if (fd_mouse != -1) {
		pfd[n_poll_fds].fd = fd_mouse;
		pfd[n_poll_fds].events = POLLIN;
		n_poll_fds++;
	} else {
		/* No keyboard nor mouse */
		//RD ERROR("no keyboard nor mouse; not continuing with event loop");
		//RD return false;
		ERROR("no keyboard nor mouse; continuing with event loop");
		sleep(5);
		return true;
	}

	for (;;) {
		if (poll(pfd, n_poll_fds, -1) == -1) {
			PERROR("poll failed");
			return false;
		}

		for (int i = 0; i < n_poll_fds; i++) {
			if (!pfd[i].revents) {
				continue;
			}

			if (pfd[i].fd == fd_keyboard) {
				if (!handle_keyboard_event()) {
					ERROR("failed to handle keyboard event");
					return false;
				}
			} else {
				if (!handle_mouse_event()) {
					ERROR("failed to handle mouse event");
					return false;
				}
			}
		}
	}

	return true;
}

bool run(void)
{
	keymap_init();

	emukb_unregister();
	emumouse_unregister();
	system("rmdir /sys/kernel/config/usb_gadget/kb/configs/c.1");
	system("rmdir /sys/kernel/config/usb_gadget/kb");

	if (!emukb_register_pre_enable()) {
		ERROR("emukb_register_pre_enable() failed");
		return false;
	}

	if (!emukb_register_auxiliary("/dev/ttyO1", 115200)) {
		ERROR("failed to register auxiliary gadget hardware");
		return false;
	}

	if (!emumouse_register_pre_enable()) {
		ERROR("emumouse_register_pre_enable() failed");
		return false;
	}

	// ENABLE
	system("echo musb-hdrc.0.auto >/sys/kernel/config/usb_gadget/kb/UDC");

	if (!emukb_register_post_enable()) {
		ERROR("emukb_register_post_enable() failed");
		return false;
	}

	if (!emumouse_register_post_enable()) {
		ERROR("emumouse_register_post_enable() failed");
		return false;
	}

	/* Look at installed hardware */
	//udev_initial_probe(probe_device);

	/*if (fd_keyboard == -1) {
		ERROR("failed to probe keyboard");
		return false;
	}
*/


//	while(	input_loop());
	
	return true;
}

bool
parse_opt(int argc, char **argv)
{
	struct option opts[] = {
		{ "verbose", 0, NULL, 1 },
		{ "debug", 0, NULL, 2 },
		{ "help", 0, NULL, 'h' },
		{ NULL, 0, NULL, 0 },
	};
	
	for (;;) {
		int opt;
		opt = getopt_long(argc, argv, "h", opts, NULL);
		if (opt == -1) {
			/* Finished */
			break;
		}
		
		switch (opt) {
		case 1: /* verbose */
			__loglevel = LOGLEVEL_VERBOSE;
			break;
		case 2: /* debug */
			__loglevel = LOGLEVEL_DEBUG;
			break;
		case 'h':
			//usage(argv[0]);
			exit(0);
		default:
			return false;
		};
	}

	return true;
}

int main (int argc, char *argv[])
{
	struct input_event ev[64];
	int fdk, rd, value, size = sizeof (struct input_event);
	char name[256] = "Unknown";
	char *kbdDevice = NULL;
	char *mouseDevice = NULL;
	char buf[2];

	char eventCode2Usb[256];
	char eventCode2UsbFn[256];

	char report[8];

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

	memset(report, 0, sizeof(report));

	eventCode2Usb[1] 	= 0x29;	//Esc 
	eventCode2Usb[2] 	= 0x1e;	//1  
	eventCode2Usb[3] 	= 0x1f;	//2  
	eventCode2Usb[4] 	= 0x20;	//3  
	eventCode2Usb[5] 	= 0x21;	//4  
	eventCode2Usb[6] 	= 0x22;	//5  
	eventCode2Usb[7] 	= 0x23;	//6  
	eventCode2Usb[8] 	= 0x24;	//7  
	eventCode2Usb[9] 	= 0x25;	//8  
	eventCode2Usb[0xa] 	= 0x26;	//9  
	eventCode2Usb[0xb] 	= 0x27;	//0  
	eventCode2Usb[0xc] 	= 0x2d;	//-  
	eventCode2Usb[0xd] 	= 0x2e;	//=  
	eventCode2Usb[0xe] 	= 0x2a;	//BS  
	eventCode2Usb[0xf] 	= 0x2b;	//Tab  

	eventCode2Usb[0x10] 	= 0x14;	//q  
	eventCode2Usb[0x11] 	= 0x1a;	//w  
	eventCode2Usb[0x12] 	= 0x08;	//e  
	eventCode2Usb[0x13] 	= 0x15;	//r  
	eventCode2Usb[0x14] 	= 0x17;	//t  
	eventCode2Usb[0x15] 	= 0x1c;	//y  
	eventCode2Usb[0x16] 	= 0x18;	//u  
	eventCode2Usb[0x17] 	= 0x0c;	//i  
	eventCode2Usb[0x18] 	= 0x12;	//o  
	eventCode2Usb[0x19] 	= 0x13;	//p  
	eventCode2Usb[0x1a] 	= 0x2f;	//[  
	eventCode2Usb[0x1b] 	= 0x30;	//]  
	eventCode2Usb[0x1c] 	= 0x28;	//Enter  


	eventCode2Usb[0x1d] 	= 0x80;	//LCTRL
	eventCode2Usb[0x1e] 	= 0x04;	//a  
	eventCode2Usb[0x1f] 	= 0x16;	//s  
	eventCode2Usb[0x20] 	= 0x07;	//d  
	eventCode2Usb[0x21] 	= 0x09;	//f  
	eventCode2Usb[0x22] 	= 0x0a;	//g  
	eventCode2Usb[0x23] 	= 0x0b;	//h  
	eventCode2Usb[0x24] 	= 0x0d;	//j  
	eventCode2Usb[0x25] 	= 0x0e;	//k  
	eventCode2Usb[0x26] 	= 0x0f;	//l  
	eventCode2Usb[0x27] 	= 0x33;	//;  
	eventCode2Usb[0x28] 	= 0x34;	//'  


	eventCode2Usb[0x29] 	= 0x35;	//` back tick
	eventCode2Usb[0x2a] 	= 0xe1;	//LShift
	eventCode2Usb[0x2b] 	= 0x31;	//Back slash 
	eventCode2Usb[0x2c] 	= 0x1d;	//z  
	eventCode2Usb[0x2d] 	= 0x1b;	//x  
	eventCode2Usb[0x2e] 	= 0x06;	//c  
	eventCode2Usb[0x2f] 	= 0x19;	//v  
	eventCode2Usb[0x30] 	= 0x05;	//b  
	eventCode2Usb[0x31] 	= 0x11;	//n  
	eventCode2Usb[0x32] 	= 0x10;	//m  
	eventCode2Usb[0x33] 	= 0x36;	//,  
	eventCode2Usb[0x34] 	= 0x37;	//.  
	eventCode2Usb[0x35] 	= 0x38;	//slash 
	eventCode2Usb[0x36] 	= 0xe5;	//RShift  

	eventCode2Usb[0x38] 	= 0xe2;	//LAlt
	eventCode2Usb[0x39] 	= 0x2c;	//Space  
	eventCode2Usb[0x3a] 	= 0xc1;	//Caps Lock  
	eventCode2Usb[0x3b] 	= 0x3a;	//F1 
	eventCode2Usb[0x3c] 	= 0x3b;	//F2 
	eventCode2Usb[0x3d] 	= 0x3c;	//F3 
	eventCode2Usb[0x3e] 	= 0x3d;	//F4 
	eventCode2Usb[0x3f] 	= 0x3e;	//F5 
	eventCode2Usb[0x40] 	= 0x3f;	//F6 
	eventCode2Usb[0x41] 	= 0x40;	//F7 
	eventCode2Usb[0x42] 	= 0x41;	//F8 
	eventCode2Usb[0x43] 	= 0x42;	//F9 
	eventCode2Usb[0x44] 	= 0x43;	//F10  
	eventCode2Usb[0x57] 	= 0x46;	//F11 - Print scr
	eventCode2Usb[0x58] 	= 0x4c;	//F12 - Delete 


	eventCode2Usb[0x69] 	= 0x50;	//Left  
	eventCode2Usb[0x6a] 	= 0x4f;	//Right shift
	eventCode2Usb[0x6c] 	= 0x51;	//Down  
	eventCode2Usb[0x67] 	= 0x52;	//Up  

	eventCode2Usb[0x66] 	= 0xd2;	//Home  
	eventCode2Usb[0x6b] 	= 0xd5;	//End  
	eventCode2Usb[0x6d] 	= 0xd6;	//Pg Down  
	eventCode2Usb[0x68] 	= 0xd3;	//Pg Up  

	eventCode2Usb[97] 	= 0x84;	//RCtrl  
	eventCode2Usb[100] 	= 0x86;	//RAlt  
	eventCode2Usb[110] 	= 0xd1;	//Insert  
	eventCode2Usb[111] 	= 0xd4;	//Delete  

	//eventCode2Usb[0x7d] 	= 0x65;	//LWin
	//eventCode2Usb[0x7e] 	= 0x76;	//RWin 
	eventCode2Usb[0x7f] 	= 0x65;	//App - Gui

	//eventCode2Usb[119] 	= 0x85;	//Pause  
	//eventCode2Usb[69] 	= 0x85;	//Num Lock  
	//eventCode2Usb[70] 	= 0x85;	//Scroll Lock  
	//eventCode2Usb[99] 	= 0x85;	//PrintScr  

	eventCode2Usb[0xff] 	= 0xb0;	//Enter  

	// Prepare special look up with LWin down
	for (int i=0; i<256; i++) 
	{
		eventCode2UsbFn[i] = eventCode2Usb[i]; 
	}

	eventCode2UsbFn[0x3b] 	= 0x44;	//F1 - converted to F11
	eventCode2UsbFn[0x3c] 	= 0x45;	//F2 - converted to F12

	eventCode2UsbFn[0x69] 	= 0x4a;	//Left  - Home
	eventCode2UsbFn[0x6a] 	= 0x4d;	//Right - End 
	eventCode2UsbFn[0x6c] 	= 0x4e;	//Down - Page Down 
	eventCode2UsbFn[0x67] 	= 0x4b;	//Up - Page Up 


	if (!run()) {
		return 1;
	}


	printf("After configfs init\n");


	//Setup check
	if (argv[1] == NULL){
		printf("main: Please specify (on the command line) the path to the dev event interface devicen");
		printf("See /proc/bus/input/devices\n");
		exit (0);
	}

	if ((getuid ()) != 0)
		printf ("You are not root! This may not work...n");




	//char *bus = "/dev/i2c-1"; // Pins P9_19 and P9_20 
	//int addr = 0x9;          // The I2C address of TMP102 
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
	/*else
	{
		printf("Needs 1 device\n");
		exit;
	}*/

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
//			printf("kbd has something to say\n");	
			if ((rd = read (fdk, ev, size * 64)) < size)
				perror_exit ("read()");     
			kbd_action = 1;
		}

		int i;
		for(i=0; i<rd/size; i++)
		{
		//	printf ("%d of %d - Code[%x %x %x]\n", i, rd/size, ev[i].type, ev[i].code, ev[i].value);
		}

		for(i=0; i<rd/size; i++)
		{
			if (ev[i].type == 1)
			{
				printf ("%d of %d - Code[%x %x %x]\n", i, rd/size, ev[i].type, ev[i].code, ev[i].value);

				if (ev[i].code == 0x3a)	// Special hanling for caps lock
				{
					printf("Mouse Buttons handling\n");
					if (ev[i].value > 0)
					{	
						if (emumouse_send_report(0x1, 0, 0, 0) == -1) 
						{
							ERROR("failed to send usb report");
							return false;
						}
					}
					else
					{
						if (emumouse_send_report(0, 0, 0, 0) == -1) 
						{
							ERROR("failed to send usb report");
							return false;
						}
					}
					continue;

				}


				else if (ev[i].code == 0x56)	// Special hanling for right click
				{
					printf("Mouse Buttons handling\n");
					if (ev[i].value > 0)
					{	
						if (emumouse_send_report(0x2, 0, 0, 0) == -1) 
						{
							ERROR("failed to send usb report");
							return false;
						}
					}
					else
					{
						if (emumouse_send_report(0, 0, 0, 0) == -1) 
						{
							ERROR("failed to send usb report");
							return false;
						}
					}
					//continue;

				}

				else if ((ev[i].code == 0x2a) || (ev[i].code == 0x36))	// left shift
				{
					printf("Shift key handling\n");
					if (ev[i].value > 0)
					{	
						printf("Setting Shift \n");
						report[0] |= 0x2;
					}
					else
					{
						printf("Clearing Shift \n");
						report[0] &= 0xfd;
					}
					//continue;

				}


				else if ((ev[i].code == 0x1d) || (ev[i].code == 0x61))	// Control keys
				{
					printf("Control key handling\n");
					if (ev[i].value > 0)
					{	
						printf("Setting Control \n");
						report[0] |= 0x1;
					}
					else
					{
						printf("Clearing Control \n");
						report[0] &= 0xfe;
					}
					//continue;

				}


				else if ((ev[i].code == 0x38) || (ev[i].code == 0x64))	// Alt keys
				{
					printf("Alt key handling\n");
					if (ev[i].value > 0)
					{	
						printf("Setting Alt \n");
						report[0] |= 0x4;
					}
					else
					{
						printf("Clearing Alt \n");
						report[0] &= 0xfb;
					}
					//continue;

				}


				else if (ev[i].code == 0x7d) 	// Left Win keys
				{
					printf("Left Win key handling\n");
					if (ev[i].value > 0)
					{	
						printf("Setting Left Win \n");
						isLWinDown = 1;

					}
					else
					{
						printf("Clearing Left Win \n");
						isLWinDown = 0;
					}
					//continue;

				}


				else if (ev[i].code == 0x7e) 	// Right Win keys
				{
					printf("Right Win key handling\n");
					if (ev[i].value > 0)
					{	
						printf("Setting Right Win \n");
						report[0] |= 0x80;
					}
					else
					{
						printf("Clearing Right Win \n");
						report[0] &= 0x7f;
					}
					//continue;

				}

				int code = ev[i].code;  

				if (!isLWinDown)
				{
					printf("Report[0] is %d \nLWin is up", report[0]);
					report[0] &= 0xf7;
					if (ev[i].value > 0)
					{					
						report[2] = eventCode2Usb[ev[i].code];  
					}
					else
					{
						report[2] = 0;  
					}
				}	

				else if ((code == 0x3b) ||
				 	(code == 0x3c) ||
				 	(code == 0x69) ||
				 	(code == 0x6a) ||
				 	(code == 0x6c) ||
				 	(code == 0x67) )
				{
					report[0] &= 0xf7;
					printf("Report[0] is %d \nLWin is Down - Special key", report[0]);
					if (ev[i].value > 0)
					{					
						report[2] = eventCode2UsbFn[ev[i].code];  
					}
					else
					{
						report[2] = 0;  
					}
				} 


				else
				{
					report[0] |= 0x8;
					printf("Report[0] is %d \nLWin is Down - Normal key", report[0]);
					if (ev[i].value > 0)
					{					
						report[2] = eventCode2UsbFn[ev[i].code];  
					}
					else
					{
						report[2] = 0;  
					}

				}
				
				for (int j=0; j<8; j++)
				{
					printf("%02x ", report[j]);
				}
				printf("\n");
				if (emukb_send_report(report, 8) == -1) 
				{
					ERROR("failed to send usb report");
					return false;
				}



			}
		}

	}

	return 0;
} 

