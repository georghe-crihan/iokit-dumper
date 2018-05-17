all:
	make clean;
	mkdir build;
	gcc -D_x86_64_=1 -std=c99 -I. dumper/*.c kernel/*.c kernel/analysis/*.c kernel/runtime_parsing/*.c *.c *.m -o build/iokit-dumper -framework IOKit -framework Foundation

clean:
	rm -rf build;
