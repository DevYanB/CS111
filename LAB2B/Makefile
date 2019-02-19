#NAME: Devyan Biswas
#EMAIL: devyanbiswas@outlook.com
#ID: #UID

build:
	gcc -pthread -Wall -Wextra -g -o lab2_add lab2_add.c
	gcc -pthread -lprofiler -Wall -Wextra -g -o lab2_list lab2_list.c SortedList.c SortedList.h
tests: build
	chmod +x testb.sh
	@-./testb.sh
clean:
	rm -f lab2_add lab2_list lab2b-#UID.tar.gz
profile: lab2_list
	LD_PRELOAD=/usr/local/cs/gperftools-2.7/lib/libprofiler.so
	CPUPROFILE=./raw.gperf
	./lab2_list --threads=12 --iterations=1000 --sync=s
	pprof --text ./lab2_list ./raw.gperf > profile.out
	pprof --list=pass_to_thread ./lab2_list ./raw.gperf >> profile.out
graphs: tests lab2b.gp
	chmod + lab2b.gp
	./lab2b.gp
dist: graphs 
	tar -czf lab2b-#UID.tar.gz README testb.sh lab2_add.c Makefile lab2b.gp lab2_add.csv SortedList.c SortedList.h lab2_list.c lab2b_list.csv *.png profile.out
