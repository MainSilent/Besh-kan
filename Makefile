name = Besh kan
CC = gcc

all: compile geniso run

compile:
	${CC} -static -o init main.c
	echo ./init | cpio -o --format=newc > initramfs.bin
	mv initramfs.bin iso/boot

geniso:
	genisoimage -R                          \
			-b boot/grub/grub.bin		    \
			-no-emul-boot                   \
			-boot-load-size 4               \
			-A os                           \
			-input-charset utf8             \
			-quiet                          \
			-boot-info-table                \
			-o 'Besh kan.iso'               \
			iso

run:
	qemu-system-x86_64 -cdrom 'Besh kan.iso'

clean:
	rm init iso/boot/initramfs.bin 'Besh kan.iso'