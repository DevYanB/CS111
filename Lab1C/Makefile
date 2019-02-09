#NAME: Devyan Biswas
#EMAIL: devyanbiswas@outlook.com
#ID: 804988161

.SILENT:

default: simpsh.c
	gcc -g -o simpsh -Wall -Wextra simpsh.c

clean:
	rm -f simpsh *.o *.tar.gz *~
	rm -r 804988161

check:  default inputCheck verboseCheck outputCheck commCheck

dist:
	tar -czvf lab1-804988161.tar.gz simpsh.c Makefile README report.pdf

inputCheck: 
	touch readable.txt;
	chmod -r readable.txt;
	./simpsh --rdonly readable.txt 2> /dev/null; \
	if [ $$? -ne 1 ]; then \
		echo "Failed inputCheck test "; \
	fi; 
	rm -f readable.txt; \

outputCheck:	
	touch writeable.txt;
	chmod -w writeable.txt;
	./simpsh --wronly writeable.txt 2> /dev/null; \
	if [ $$? -ne 1 ]; then \
		echo "Failed outputCheck test "; \
	fi;
	rm -f writeable.txt; \

verboseCheck:
	touch items.txt;
	touch write.txt;
	touch read.txt;
	./simpsh --wronly write.txt --verbose --rdonly read.txt &> items.txt 2> /dev/null; \
	if [ $$? -ne 0 ] || grep -q -- "--wronly write.txt" items.txt; then \
		echo "Failed verboseCheck test";\
	fi;
	rm -f items.txt write.txt read.txt;\

commCheck:
	touch rd.txt;
	touch wr.txt;
	touch wr1.txt
	./simpsh --wronly wr.txt --rdonly rd.txt --wronly wr1.txt --command 0 1 2 cat 2> /dev/null; \
	if [ $$? -ne 0 ]; then \
		echo "Failed commCheck test "; \
	fi;
	rm -r rd.txt wr.txt wr1.txt; \
