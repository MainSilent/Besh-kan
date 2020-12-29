name = Besh kan
CC = gcc

all: compile geniso run

compile:
	${CC} -static -o init main.c
	mv init initfs/
	cd initfs/ && find . | cpio -o --format=newc > ../initramfs.bin
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
			-o '${name}.iso'               \
			iso

run:
	# development
	#qemu-system-x86_64 -m 4096 -smp 4 -cdrom '${name}.iso'

	# testing 
	qemu-system-x86_64 -m 4096 -smp 4 -cdrom '${name}.iso' -hda ../../windows.img -boot d -nic none

clean:
	rm init iso/boot/initramfs.bin '${name}.iso'