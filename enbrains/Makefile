#gcc -o enbrains -I/usr/lib/x86_64-linux-gnu/ libevent_example.c -L/usr/lib/x86_64-linux-gnu/ -levent 

target: main.o
	gcc -o enbrains main.c -L/usr/lib/x86_64-linux-gnu/ -levent 

clean:
	rm enbrains *.o 
