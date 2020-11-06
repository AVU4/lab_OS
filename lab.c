#include <stdio.h>
#include <sys/mman.h>
#include <pthread.h>
#include <stdlib.h>

#define D 78

struct pair{
	int file;
	FILE* address;
};

void* write(void* data){
	struct pair* current_data = (struct pair*) data;
	fgets(current_data->address, 1, current_data->file);
	pthread_exit(0);
}

void fill_space(int* address){
		

	struct pair* data;
	pthread_t threads[D];
	size_t i;
	

	data->file = fopen("/dev/urandom", "r");

	data->address = address;

	for (i = 0; i < D; i ++){
		pthread_create(&threads[i], NULL, write, data);
	}

}


int main(void){
	printf("Before allocation");
	int* address = 0xA35E1090;
	getchar();
	int* status =  mmap (address, 197 * 1024 * 1024,
	   PROT_READ | PROT_WRITE | PROT_EXEC,
	   MAP_ANONYMOUS | MAP_PRIVATE,
	   0,
	   0);

	if (status == 0) {
		printf("Error");
	}
	printf("After allocation");
	getchar();

	fill_space(status);

	printf("After fill space");
	getchar();

	free(status);
	printf("After deallocation");

	getchar();


}
