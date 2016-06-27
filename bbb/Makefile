LDFLAGS= -g
#CFLAGS=-g -Ilibcstuff -Wall -Werror --std=c99 -D_GNU_SOURCE
CFLAGS=-g -Ilibcstuff  --std=c99 -D_GNU_SOURCE

all: gadgetkbd

%.o:%.c
	@echo [CC] $<
	@gcc $(CFLAGS) -c $<

report_descriptor_kb.o: report_descriptor_kb.bin
	@echo [BIN] $<
	@ld -r -b binary report_descriptor_kb.bin -o report_descriptor_kb.o

report_descriptor_mouse.o: report_descriptor_mouse.bin
	@echo [BIN] $<
	@ld -r -b binary report_descriptor_mouse.bin -o report_descriptor_mouse.o

libcstuff.o: libcstuff/libcstuff.c
	@echo [CC] $<
	@gcc $(CFLAGS) -o $@ -c $<

gadgetkbd: libcstuff.o main.o emukb.o emumouse.o report_descriptor_kb.o report_descriptor_mouse.o keymap.o
	@echo [LD] $@
	@gcc $(CFLAGS) $(LDFLAGS) -o gadgetkbd $^

clean:
	rm -f gadgetkbd
	rm -f *.o

.PHONY: clean
