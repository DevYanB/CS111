#NAME: DEVYAN BISWAS
#EMAIL: devyanbiswas@outlook.com
#ID: #UID

default:
	gcc -o lab4c_tcp -Wall -Wextra -lm -lmraa -g lab4c_tcp.c
	gcc -o lab4c_tls -Wall -Wextra -lm -lmraa -g -lssl -lcrypto lab4c_tls.c
clean:
	rm -f lab4c_tcp lab4c_tls lab4c-#UID.tar.gz *~
dist:
	tar -czvf lab4c-#UID.tar.gz lab4c_tcp.c lab4c_tls.c Makefile README

