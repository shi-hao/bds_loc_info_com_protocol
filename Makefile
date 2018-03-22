SRC:=main_test.c bds_data_proc.c
prog:=main

all: $(SRC)
	gcc -o $(prog) $(SRC) -g

clean:
	rm $(prog)

