name = Besh kan
pname = init
CC = gcc

all: compile geniso

compile:
	${CC} -static -o ${pname} main.c
	mv ${pname} initfs/
	cd initfs/ && find . | cpio --quiet -H newc -o | gzip -9 -n > ../initramfs
	mv initramfs iso/boot

geniso:
	genisoimage -R                             \
			-b boot/grub/i386-pc/eltorito.img  \
			-no-emul-boot                      \
			-V '${name}'                       \
			-quiet                             \
			-o '${name}.iso'                   \
			iso

run:
	# development
	qemu-system-x86_64 -m 4096 -smp 4 -cdrom '${name}.iso'

	# testing 
	#qemu-system-x86_64 -m 4096 -smp 4 -cdrom '${name}.iso' -hda ../../windows.img -boot d -nic none

clean:
	rm initfs/${pname} iso/boot/initramfs '${name}.iso'
