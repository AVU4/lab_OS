#include <stdio.h>
#include <sys/mman.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>

#define A 197
#define B 0xA35E1090
#define D 78
#define E 72
#define G 10


const int filenumbers = A/ E + (A % E == 0 ? 0: 1);
sem_t sem1;
sem_t sem2;
sem_t sem3;

int loop = 1;

typedef struct {
	char* address;
	size_t size;
	FILE* file;
}thread_info;


typedef struct {
	char* address;
	int filenumber;
//	sem_t* sems;
}writer_thread_info;


char* allocate(){
	return mmap(B, A * 1024 * 1024, 
			PROT_EXEC | PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS,
			-1,
			0);
}

void* write(void* data){

	thread_info* cur_data = (thread_info*) data;
	char* block = malloc(cur_data->size * sizeof(char));
	fread(block, 1, cur_data->size, cur_data->file);
	for (size_t i = 0; i < cur_data->size; i ++)
		cur_data->address[i] = block[i];
	free(block);
	pthread_exit(0);
}

void fill_space(char* address){

	char* address_offset = address;
	
	thread_info informations[D]; 
	pthread_t threads[D];

	size_t size_for_thread = A * 1024 * 1024 / D;
	FILE* filerandom = fopen("/dev/urandom", "rb");

	for (int i = 0; i < D; i ++){
		informations[i].address = address_offset;
	        informations[i].size = size_for_thread;
	        informations[i].file = filerandom;
		address_offset += size_for_thread;
	}

	informations[D - 1].size += A * 1024 * 1024 % D;


	for (int i = 0; i < D; i ++) 
		pthread_create(&threads[i], NULL, write, &informations[i]);

	for (int i = 0; i < D; i ++) 
		pthread_join(threads[i], NULL);

	fclose(filerandom);

}

void* generate_info(void* data){
	char* cur_data = (char*) data;
	while(loop == 1){
		fill_space(cur_data);
	}
	
	pthread_exit(0);
}

sem_t* get_sem(int id){
	if (id == 0) return &sem1;
	if (id == 1) return &sem2;
	if (id == 2) return &sem3;
}


void write_file(writer_thread_info* data, int idFile){
	
	sem_t* cur_sem = get_sem(idFile);
	sem_wait(cur_sem);
	char* defaultname = malloc(sizeof(char) * 6);
	snprintf(defaultname, sizeof(defaultname) + 1, "labOS%d",++idFile);
	FILE* file = fopen(defaultname, "w+b");
	unsigned int file_size = E * 1024 * 1024;
	size_t rest = 0;
	while (rest < file_size) {
		long size = file_size - rest >= D ? D : file_size - rest;
	        rest += fwrite(data->address + rest, 1,size, file);	
	}

	sem_post(cur_sem);
}

void* write_files(void* data){
	writer_thread_info* cur_data = (writer_thread_info*) data;
        while (loop == 1){	
		for (int i = 0; i < cur_data->filenumber; i ++){
			write_file(cur_data, i);
			
		}
	}
}



int main(void){
	printf("Before allocation");
	getchar();

	char* address = allocate();

	printf("After allocation");
	getchar();

	fill_space(address);
	
	printf("After filling");
	getchar();

	munmap(address, A * 1024 * 1024);

	printf("After deallocation");
	getchar();

	pthread_t generate_thread;
	
	address = allocate();

	pthread_create(&generate_thread, NULL, generate_info, address);

	sem_init(&sem1, 0, 1);
	sem_init(&sem2, 0, 1);
	sem_init(&sem3, 0, 1);
	
	pthread_t writer_thread;
	writer_thread_info writer_information;

	writer_information.filenumber = filenumbers;
	writer_information.address = address;


	pthread_create(&writer_thread, NULL, write_files, &writer_information);

	printf("Pause");
	getchar();

	loop = 0;

	pthread_join(writer_thread, NULL);
	pthread_join(generate_thread, NULL);

	sem_destroy(&sem1);
	sem_destroy(&sem2);
	sem_destroy(&sem3);

	munmap(B, A * 1024 * 1024);	
	return 0;
}
