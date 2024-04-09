CFILES = $(wildcard *.c) $(wildcard */*.c)
LIBC =  $(wildcard ../wt-libc/*.o)
OFILES = $(CFILES:.c=.o)

CC=x86_64-elf-gcc
CFLAGS=-ffreestanding -Wall -Wextra -Werror -I. -I../wt-libc
LDFLAGS=-T linker.ld -ffreestanding -O2 -nostdlib -lgcc

all: $(OFILES) link

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

link: $(OFILES)
	$(CC) -o Xansh.elf $(OFILES) $(LIBC) $(LDFLAGS)
	cp Xansh.elf ../../Xana3/ramdisk/bin/Xansh.elf

clean:
	rm -f $(OFILES) Xansh.elf