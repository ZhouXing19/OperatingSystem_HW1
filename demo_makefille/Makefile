hw: hw.o helper.o
	gcc -o hw hw.o helper.o -lm

hw.o: hw.c
	gcc -O -Wall -c hw.c

helper.o: helper.c
	gcc -O -Wall -c helper.c

clean:
	rm -f hw.o helper.o hw