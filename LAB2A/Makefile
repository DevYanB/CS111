#NAME: Devyan Biswas
#EMAIL: devyanbiswas@outlook.com
#ID: #UID

build:
	gcc -pthread -Wall -Wextra -g -o lab2_add lab2_add.c
	gcc -pthread -Wall -Wextra -g -o lab2_list lab2_list.c SortedList.c SortedList.h
tests: build
	chmod +x test.sh
	@-./test.sh
clean:
	rm -f lab2_add lab2_list lab2a-#UID.tar.gz
graphs: tests lab2_add.gp lab2_list.gp
	chmod +x lab2_add.gp
	./lab2_add.gp
	chmod +x lab2_list.gp
	./lab2_list.gp
dist: graphs 
	tar -czf lab2a-#UID.tar.gz README test.sh lab2_add.c Makefile lab2_add.gp lab2_list.gp lab2_add.csv SortedList.c SortedList.h lab2_list.c lab2_list.csv *.png
