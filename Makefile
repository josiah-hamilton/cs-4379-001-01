CC=mpicc

%.o:%.c
	$(CC) $(CFLAGS) $< -o $@
hwk1make: main
	#module load gnu openmpi # Module not found in PATH
	$(CC) -o main main.c
clean:
	rm main

	
