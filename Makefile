obj-m := get_proc_info.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	rm -f get_proc_info test
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean


test: build_test
	./test

build_test: test.c
	gcc test.c -o test
