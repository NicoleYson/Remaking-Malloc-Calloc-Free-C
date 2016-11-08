COMPILER = gcc
CFLAGS = -Wall -g
CFLAGS2 = -g

all: mymalloc test0 test1 test2 test3 test4 test5 test6 test7 test8 test9 test10
.PHONY: all
.PHONY: clean

mymalloc.o: mymalloc.c mymalloc.h
	$(COMPILER) $(CFLAGS) -c mymalloc.c

main.o: main.c mymalloc.h
	$(COMPILER) $(CFLAGS2) -c main.c
	
mymalloc: mymalloc.o main.o
	$(COMPILER) $(CFLAGS) -o mymalloc main.o mymalloc.o


test0.o: test0.c
	$(COMPILER) $(CFLAGS2) -c test0.c

test0: mymalloc.o test0.o
	$(COMPILER) $(CFLAGS) -o test0 test0.o mymalloc.o


test1.o: test1.c
	$(COMPILER) $(CFLAGS2) -c test1.c

test1: mymalloc.o test1.o
	$(COMPILER) $(CFLAGS) -o test1 test1.o mymalloc.o


test2.o: test2.c
	$(COMPILER) $(CFLAGS2) -c test2.c

test2: mymalloc.o test2.o
	$(COMPILER) $(CFLAGS) -o test2 test2.o mymalloc.o


test3.o: test3.c
	$(COMPILER) $(CFLAGS2) -c test3.c

test3: mymalloc.o test3.o
	$(COMPILER) $(CFLAGS) -o test3 test3.o mymalloc.o


test4.o: test4.c
	$(COMPILER) $(CFLAGS2) -c test4.c

test4: mymalloc.o test4.o
	$(COMPILER) $(CFLAGS) -o test4 test4.o mymalloc.o


test5.o: test5.c
	$(COMPILER) $(CFLAGS2) -c test5.c

test5: mymalloc.o test5.o
	$(COMPILER) $(CFLAGS) -o test5 test5.o mymalloc.o


test6.o: test6.c
	$(COMPILER) $(CFLAGS2) -c test6.c

test6: mymalloc.o test6.o
	$(COMPILER) $(CFLAGS) -o test6 test6.o mymalloc.o


test7.o: test7.c
	$(COMPILER) $(CFLAGS2) -c test7.c

test7: mymalloc.o test7.o
	$(COMPILER) $(CFLAGS) -o test7 test7.o mymalloc.o

test8.o: test8.c
	$(COMPILER) $(CFLAGS2) -c test8.c

test8: mymalloc.o test8.o
	$(COMPILER) $(CFLAGS) -o test8 test8.o mymalloc.o


test9.o: test9.c
	$(COMPILER) $(CFLAGS2) -c test9.c

test9: mymalloc.o test9.o
	$(COMPILER) $(CFLAGS) -o test9 test9.o mymalloc.o
	
	
test10.o: test10.c
	$(COMPILER) $(CFLAGS2) -c test10.c

test10: mymalloc.o test10.o
	$(COMPILER) $(CFLAGS) -o test10 test10.o mymalloc.o




	
clean:
	rm -f *.o mymalloc test1 test2 test3 test4 test5 test6 test7 test8 test9 test10
