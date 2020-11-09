#include <stdio.h>
#include <sys/mman.h>
#include <pthread.h>
#include <stdlib.h>

#define D 78

typedef struct{
	long start_address;
	FILE* file;
} thread_info;

const long size_for_thread = 197 * 1024 * 1024 / D;

void* write(void* data){
	thread_info* current_data = (thread_info*) data;
	fgets(0xA35E1090 + size_for_thread * current_data->start_address , 4, current_data->file);
	pthread_exit(0);
}

void fill_space(){
	
	printf("Before allocation");
	getchar();

	int* address = mmap(0xA35E1090, 197 * 1024 * 1024,
			PROT_READ | PROT_WRITE | PROT_EXEC,
			MAP_ANONYMOUS | MAP_PRIVATE,
			0,
			0);
	
	printf("After allocation");
	getchar();

	thread_info informations[D];
	pthread_t threads[D];
	size_t i;
	
	const char* filename = "/dev/urandom";
	FILE* file_random = fopen(filename, "r");

	if (file_random == NULL){
		fprintf(stderr, "Couldn't open %s\n", "/dev/urandom");
		exit(1);
	}

	for (i = 0; i < D; i ++){
		informations[i].start_address = i;
		informations[i].file = file_random;
	}

	for (i = 0; i < D; i ++){
		pthread_create(&threads[i], NULL, write, &informations[i]);
	}
	
	printf("After fill data");
	getchar();

	munmap(address, 197 * 1024 * 1024);
	fclose(file_random);
	printf("After deallocation");
	getchar();

}


int main(void){
	fill_space();


}
