build:
	qemu-img create hd.img 10m
	nasm -f bin mbr_32bits.asm -o mbr_32bits.bin
	nasm -f bin 32bits.asm -o 32bits.bin
	dd if=mbr_32bits.bin of=hd.img bs=512 count=1 seek=0 conv=notrunc
	dd if=32bits.bin of=hd.img bs=512 count=5 seek=1 conv=notrunc
run:
	qemu-system-i386 -drive format=raw,file=hd.img -serial null -parallel stdio
clean:
	rm -fr *.bin
