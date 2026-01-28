debug:
	qemu-system-i386 -s -S -drive format=raw,file=hd.img -serial null -parallel stdio &
	sleep 1
	gnome-terminal -- gdb -q -x gdbinit
build:
	nasm -g -f elf32 mbr_protect.asm -o mbr_protect.o
	ld -o mbr_protect.symbol -melf_i386 -N mbr_protect.o -Ttext 0x7c00
	ld -o mbr_protect.bin -melf_i386 -N mbr_protect.o -Ttext 0x7c00 --oformat binary
	nasm -g -f elf32 bootloader_protect.asm -o bootloader_protect.o
	ld -o bootloader_protect.symbol -melf_i386 -N bootloader_protect.o -Ttext 0x7e00
	ld -o bootloader_protect.bin -melf_i386 -N bootloader_protect.o -Ttext 0x7e00 --oformat binary
	dd if=mbr_protect.bin of=hd.img bs=512 count=1 seek=0 conv=notrunc
	dd if=bootloader_protect.bin of=hd.img bs=512 count=5 seek=1 conv=notrunc
clean:
	rm -fr *.bin *.o

build1:
	nasm -f bin mbr.asm -o mbr.bin
	nasm -f bin bootloader.asm -o bootloader.bin
	dd if=mbr.bin of=hd.img bs=512 count=1 seek=0 conv=notrunc
	dd if=bootloader.bin of=hd.img bs=512 count=5 seek=1 conv=notrunc
build2:
	nasm -f bin mbr_CHS.asm -o mbr_CHS.bin
	nasm -f bin bootloader.asm -o bootloader.bin
	dd if=mbr_CHS.bin of=hd.img bs=512 count=1 seek=0 conv=notrunc
	dd if=bootloader.bin of=hd.img bs=512 count=5 seek=1 conv=notrunc
build3:
	nasm -f bin mbr_protect.asm -o mbr_protect.bin
	nasm -f bin bootloader_protect.asm -o bootloader_protect.bin
	dd if=mbr_protect.bin of=hd.img bs=512 count=1 seek=0 conv=notrunc
	dd if=bootloader_protect.bin of=hd.img bs=512 count=5 seek=1 conv=notrunc
run:
	qemu-system-i386 -hda hd.img -serial null -parallel stdio  

