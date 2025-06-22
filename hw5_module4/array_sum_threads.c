#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static int global_sum = 0;
static pthread_mutex_t sum_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int* array;
    int start_index;
    int end_index;
    int thread_id;
} thread_data_t;

static void* worker_thread(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    int local_sum = 0;
    
    printf("Thread %d: processing elements from index %d to %d\n", 
           data->thread_id, data->start_index, data->end_index - 1);
    
    for (int i = data->start_index; i < data->end_index; i++) {
        local_sum += data->array[i];
    }
    
    pthread_mutex_lock(&sum_mutex);
    global_sum += local_sum;
    printf("Thread %d: added %d to global sum, new global_sum = %d\n", 
           data->thread_id, local_sum, global_sum);
    pthread_mutex_unlock(&sum_mutex);
    
    return NULL;
}

int main(int argc, char* argv[]) {
    int N;
    
    if (argc != 2) {
        printf("Usage: %s <N>\n", argv[0]);
        printf("N must be 16 or greater\n");
        return 1;
    }
    
    N = atoi(argv[1]);
    if (N < 16) {
        printf("Error: N must be 16 or greater\n");
        return 1;
    }
    
    printf("Creating array of %d random integers...\n", N);
    
    int* array = (int*)malloc(N * sizeof(int));
    if (array == NULL) {
        perror("malloc failed");
        return 1;
    }
    
    srand(time(NULL));
    
    for (int i = 0; i < N; i++) {
        array[i] = rand() % 100;
    }
    
    printf("\nArray contents:\n");
    for (int i = 0; i < N; i++) {
        printf("%d ", array[i]);
        if ((i + 1) % 10 == 0) printf("\n");
    }
    printf("\n\n");
    
    int part_size = N / 4;
    int remainder = N % 4;
    
    thread_data_t thread_data[4];
    pthread_t threads[4];
    
    int current_index = 0;
    for (int i = 0; i < 4; i++) {
        thread_data[i].array = array;
        thread_data[i].start_index = current_index;
        
        int extra = (i < remainder) ? 1 : 0;
        thread_data[i].end_index = current_index + part_size + extra;
        thread_data[i].thread_id = i + 1;
        
        current_index = thread_data[i].end_index;
        
        printf("Creating thread %d: indices %d to %d (%d elements)\n", 
               i + 1, thread_data[i].start_index, 
               thread_data[i].end_index - 1, 
               thread_data[i].end_index - thread_data[i].start_index);
        
        int s = pthread_create(&threads[i], NULL, worker_thread, &thread_data[i]);
        if (s != 0) {
            perror("pthread_create");
            free(array);
            return 1;
        }
    }
    
    printf("\nMain thread: waiting for all worker threads to complete...\n");
    
    for (int i = 0; i < 4; i++) {
        int s = pthread_join(threads[i], NULL);
        if (s != 0) {
            perror("pthread_join");
            free(array);
            return 1;
        }
        printf("Thread %d completed\n", i + 1);
    }
    
    printf("\nFINAL RESULT\n");
    printf("Sum of all elements: %d\n", global_sum);
    
    int verification_sum = 0;
    for (int i = 0; i < N; i++) {
        verification_sum += array[i];
    }
    printf("Verification sum: %d\n", verification_sum);
    printf("Results match: %s\n", (global_sum == verification_sum) ? "YES" : "NO");
    
    free(array);
    return 0;
} 