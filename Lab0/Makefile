#NAME: Devyan Biswas
#EMAIL: devyanbiswas@outlook.com
#ID: #UID

.SILENT:

default: lab0.c
	gcc -o lab0 -g -Wall -Wextra lab0.c

check: default inAndOutTest catchSegFault notWriteTest badInput badArg dumpCoreTest

clean:
	rm -f *.tar.gz *.txt *.o lab0 check

dist: default
	tar -czvf lab0-#UID.tar.gz Makefile README lab0.c breakpoint.png backtrace.png \

inAndOutTest:
	@echo "Input Testing Info Dood" > input.txt;
	./lab0 --input input.txt --output output.txt; \
	if [ $$? -ne 0 ]; then \
		echo "Smoke Test failed on inAndOutTest (input AND output file test)\n"; \
	fi; 
	rm -f input.txt output.txt; \

catchSegFault:
	./lab0 --catch --segfault &> /dev/null; \
	if [ $$? -ne 4 ]; then \
		echo "Smoke Test did not catch seg fault when meant to catch\n"; \
	fi; 

notWriteTest:
	touch badWrite.txt; \
	chmod -w badWrite.txt; \
	./lab0 --output badWrite.txt &> /dev/null; \
	if [ $$? -ne 3 ]; then \
		echo "Smoke Test failed handling unwritable file\n"; \
	fi;
	rm -f badWrite.txt; \

badInput:
	echo "Hello World!" > input.txt;\
	chmod u-r input.txt;\
	./lab0 --input input.txt &> /dev/null ; \
	if [ $$? -ne 2 ] ;\
	then\
		echo "Smoke test failed error return for unreadable file";\
	fi;\
	rm *.txt;

badArg:
	./lab0 --applesauce &> /dev/null; \
	if [ $$? -ne 1 ]; \
	then\
		echo "Smoke test failed handling bad arg";\
	fi;

dumpCoreTest:
	./lab0 --dump-core --catch --segfault &> /dev/null; \
	if [ $$? -ne 4 ]; then \
		echo "Smoke Test did not catch segfault with dumpcore before catch\n"; \
	fi;

