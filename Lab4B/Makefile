#NAME: DEVYAN BISWAS
#EMAIL: devyanbiswas@outlook.com
#ID: #UID

default:
	gcc -Wall -Wextra -lmraa -o lab4b lab4b.c -lm
clean:
	rm -f lab4b lab4b-#UID.tar.gz *~
dist:
	tar -czvf lab4b-#UID.tar.gz lab4b.c Makefile README tester.sh
check: default
	chmod +x tester.sh
	@-./tester.sh
