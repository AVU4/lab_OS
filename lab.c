#include <stdio.h>
#include <sys/mman.h>
#include <pthread.h>
#include <stdlib.h>



#define A 197
#define D 78
#define E 72
#define G 10

typedef struct{
	long start_address;
	FILE* file;
} thread_info;


const long size_for_thread = 197 * 1024 * 1024 / D;
const char* filename = "/dev/urandom";



void* write(void* data){
	thread_info* current_data = (thread_info*) data;
	fread(0xA35E1090 + size_for_thread * current_data->start_address, sizeof(char) , 4, current_data->file);
	pthread_exit(0);
}

void* write_file(void* data){
}

int* allocate(int* address, int size){
	return mmap(address, size  * 1024 * 1024,
			PROT_READ | PROT_WRITE | PROT_EXEC,
			MAP_ANONYMOUS | MAP_PRIVATE,
			0,
			0);
}


void fill_files(int* address){
	int number_files = A/E + (A%E == 0 ? 0: 1);
	address  = allocate(address, 197);
	int index = 1;
	
	
	int rest = E; //Rest of memory in File
	char name_file[11];
	snprintf(name_file,sizeof(name_file) , "labOS%d.bin", index);

	FILE* file = fopen(name_file, "wb");

	while (rest > 0) {
		int f = 1;
		fwrite(&f,sizeof(int),1, file);	
		rest = 0;
	}	
}

void fill_space(int* address){
	
	printf("Before allocation");
	getchar();

	address = allocate(address, 197);
	
	printf("After allocation");
	getchar();

	thread_info informations[D];
	pthread_t threads[D];
	size_t i;
	
	FILE* file_random = fopen(filename, "r");


	for (i = 0; i < D; i ++){
		informations[i].start_address = i;
		informations[i].file = file_random;
	}

	for (i = 0; i < D; i ++)
		pthread_create(&threads[i], NULL, write, &informations[i]);
	
	printf("After fill data");
	getchar();

	munmap(address, 197 * 1024 * 1024);
	fclose(file_random);
	for (i = 0; i < D; i ++)
		pthread_join(threads[i], NULL);
	printf("After deallocation");
	getchar();


}


int main(void){
	int* address = 0xA35E1091;
	fill_space(address);
	printf("Second part");
	getchar();
	fill_files(address);


}
