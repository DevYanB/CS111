#Old add test
#mutex sync adds
./lab2_add --threads=1 --iterations=10000 --sync=m >> lab2_add.csv
./lab2_add --threads=2 --iterations=10000 --sync=m >> lab2_add.csv
./lab2_add --threads=4 --iterations=10000 --sync=m >> lab2_add.csv
./lab2_add --threads=8 --iterations=10000 --sync=m >> lab2_add.csv
./lab2_add --threads=12--iterations=10000 --sync=m >> lab2_add.csv
#spin lock sync adds
./lab2_add --threads=1 --iterations=10000 --sync=s >> lab2_add.csv
./lab2_add --threads=2 --iterations=10000 --sync=s >> lab2_add.csv
./lab2_add --threads=4 --iterations=10000 --sync=s >> lab2_add.csv
./lab2_add --threads=8 --iterations=10000 --sync=s >> lab2_add.csv
./lab2_add --threads=12 --iterations=10000 --sync=s >> lab2_add.csv

# Mutex synchronized list operations
./lab2_list --threads=1 --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --threads=2 --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --threads=4 --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --threads=8 --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --threads=12 --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --threads=16 --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --threads=24 --iterations=1000 --sync=m >> lab2b_list.csv

# Spin-lock synchronized list operations
./lab2_list --threads=1 --iterations=1000 --sync=s >> lab2b_list.csv
./lab2_list --threads=2 --iterations=1000 --sync=s >> lab2b_list.csv
./lab2_list --threads=4 --iterations=1000 --sync=s >> lab2b_list.csv
./lab2_list --threads=8 --iterations=1000 --sync=s >> lab2b_list.csv
./lab2_list --threads=12 --iterations=1000 --sync=s >> lab2b_list.csv
./lab2_list --threads=16 --iterations=1000 --sync=s >> lab2b_list.csv
./lab2_list --threads=24 --iterations=1000 --sync=s >> lab2b_list.csv

#lab2b_2.png
./lab2_list --threads=1 --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --threads=2 --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --threads=4 --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --threads=8 --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --threads=16 --iterations=1000 --sync=m >> lab2b_list.csv
./lab2_list --threads=24 --iterations=1000 --sync=m >> lab2b_list.csv

#lab2b_3.png
#No synchronization initially
./lab2_list --threads=1 --iterations=1 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=1 --iterations=2 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=1 --iterations=4 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=1 --iterations=8 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=1 --iterations=16 --lists=4 --yield=id >> lab2b_list.csv

./lab2_list --threads=4 --iterations=1 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=4 --iterations=2 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=4 --iterations=4 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=4 --iterations=8 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=4 --iterations=16 --lists=4 --yield=id >> lab2b_list.csv

./lab2_list --threads=8 --iterations=1 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=8 --iterations=2 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=8 --iterations=4 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=8 --iterations=8 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=8 --iterations=16 --lists=4 --yield=id >> lab2b_list.csv

./lab2_list --threads=12 --iterations=1 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=12 --iterations=2 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=12 --iterations=4 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=12 --iterations=8 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=12 --iterations=16 --lists=4 --yield=id >> lab2b_list.csv

./lab2_list --threads=16 --iterations=1 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=16 --iterations=2 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=16 --iterations=4 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=16 --iterations=8 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=16 --iterations=16 --lists=4 --yield=id >> lab2b_list.csv

#With mutex synchronization
./lab2_list --threads=1 --iterations=10 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=1 --iterations=20 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=1 --iterations=40 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=1 --iterations=80 --lists=4 --yield=id --sync=m >> lab2b_list.csv

./lab2_list --threads=4 --iterations=10 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=4 --iterations=20 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=4 --iterations=40 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=4 --iterations=80 --lists=4 --yield=id --sync=m >> lab2b_list.csv

./lab2_list --threads=8 --iterations=10 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=8 --iterations=20 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=8 --iterations=40 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=8 --iterations=80 --lists=4 --yield=id --sync=m >> lab2b_list.csv

./lab2_list --threads=12 --iterations=10 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=12 --iterations=20 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=12 --iterations=40 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=12 --iterations=80 --lists=4 --yield=id --sync=m >> lab2b_list.csv

./lab2_list --threads=16 --iterations=10 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=16 --iterations=20 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=16 --iterations=40 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=16 --iterations=80 --lists=4 --yield=id --sync=m >> lab2b_list.csv

#With spin lock synchronization
./lab2_list --threads=1 --iterations=10 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=1 --iterations=20 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=1 --iterations=40 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=1 --iterations=80 --lists=4 --yield=id --sync=s >> lab2b_list.csv

./lab2_list --threads=4 --iterations=10 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=4 --iterations=20 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=4 --iterations=40 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=4 --iterations=80 --lists=4 --yield=id --sync=s >> lab2b_list.csv

./lab2_list --threads=8 --iterations=10 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=8 --iterations=20 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=8 --iterations=40 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=8 --iterations=80 --lists=4 --yield=id --sync=s >> lab2b_list.csv

./lab2_list --threads=12 --iterations=10 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=12 --iterations=20 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=12 --iterations=40 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=12 --iterations=80 --lists=4 --yield=id --sync=s >> lab2b_list.csv

./lab2_list --threads=16 --iterations=10 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=16 --iterations=20 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=16 --iterations=40 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=16 --iterations=80 --lists=4 --yield=id --sync=s >> lab2b_list.csv

#lab2b_4.png and lab2b_5.png
#Mutex synchronization w/o yields
./lab2_list --threads=1 --iterations=1000 --lists=1 --sync=m >> lab2b_list.csv
./lab2_list --threads=1 --iterations=1000 --lists=4 --sync=m >> lab2b_list.csv
./lab2_list --threads=1 --iterations=1000 --lists=8 --sync=m >> lab2b_list.csv
./lab2_list --threads=1 --iterations=1000 --lists=16 --sync=m >> lab2b_list.csv

./lab2_list --threads=2 --iterations=1000 --lists=1 --sync=m >> lab2b_list.csv
./lab2_list --threads=2 --iterations=1000 --lists=4 --sync=m >> lab2b_list.csv
./lab2_list --threads=2 --iterations=1000 --lists=8 --sync=m >> lab2b_list.csv
./lab2_list --threads=2 --iterations=1000 --lists=16 --sync=m >> lab2b_list.csv

./lab2_list --threads=4 --iterations=1000 --lists=1 --sync=m >> lab2b_list.csv
./lab2_list --threads=4 --iterations=1000 --lists=4 --sync=m >> lab2b_list.csv
./lab2_list --threads=4 --iterations=1000 --lists=8 --sync=m >> lab2b_list.csv
./lab2_list --threads=4 --iterations=1000 --lists=16 --sync=m >> lab2b_list.csv

./lab2_list --threads=8 --iterations=1000 --lists=1 --sync=m >> lab2b_list.csv
./lab2_list --threads=8 --iterations=1000 --lists=4 --sync=m >> lab2b_list.csv
./lab2_list --threads=8 --iterations=1000 --lists=8 --sync=m >> lab2b_list.csv
./lab2_list --threads=8 --iterations=1000 --lists=16 --sync=m >> lab2b_list.csv

./lab2_list --threads=12 --iterations=1000 --lists=1 --sync=m >> lab2b_list.csv
./lab2_list --threads=12 --iterations=1000 --lists=4 --sync=m >> lab2b_list.csv
./lab2_list --threads=12 --iterations=1000 --lists=8 --sync=m >> lab2b_list.csv
./lab2_list --threads=12 --iterations=1000 --lists=16 --sync=m >> lab2b_list.csv

#Spin lock synchronization w/o yields
./lab2_list --threads=1 --iterations=1000 --lists=1 --sync=s >> lab2b_list.csv
./lab2_list --threads=1 --iterations=1000 --lists=4 --sync=s >> lab2b_list.csv
./lab2_list --threads=1 --iterations=1000 --lists=8 --sync=s >> lab2b_list.csv
./lab2_list --threads=1 --iterations=1000 --lists=16 --sync=s >> lab2b_list.csv

./lab2_list --threads=4 --iterations=1000 --lists=1 --sync=s >> lab2b_list.csv
./lab2_list --threads=4 --iterations=1000 --lists=4 --sync=s >> lab2b_list.csv
./lab2_list --threads=4 --iterations=1000 --lists=8 --sync=s >> lab2b_list.csv
./lab2_list --threads=4 --iterations=1000 --lists=16 --sync=s >> lab2b_list.csv

./lab2_list --threads=8 --iterations=1000 --lists=1 --sync=s >> lab2b_list.csv
./lab2_list --threads=8 --iterations=1000 --lists=4 --sync=s >> lab2b_list.csv
./lab2_list --threads=8 --iterations=1000 --lists=8 --sync=s >> lab2b_list.csv
./lab2_list --threads=8 --iterations=1000 --lists=16 --sync=s >> lab2b_list.csv

./lab2_list --threads=12 --iterations=1000 --lists=1 --sync=s >> lab2b_list.csv
./lab2_list --threads=12 --iterations=1000 --lists=4 --sync=s >> lab2b_list.csv
./lab2_list --threads=12 --iterations=1000 --lists=8 --sync=s >> lab2b_list.csv
./lab2_list --threads=12 --iterations=1000 --lists=16 --sync=s >> lab2b_list.csv
