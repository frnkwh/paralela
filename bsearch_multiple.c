#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_THREADS 64

typedef struct {
        long long target;
        int *result;
        int index;
} SearchTask;

pthread_t threads[MAX_THREADS];
long long *inputArray;
int nElements;


int compare(const void *a, const void *b) {
        return (*(long long *)a - *(long long *)b);
}

void *bsearch_lower_bound_task(void *arg) {
        SearchTask *task = (SearchTask *)arg;
        long long target = task->target;
        int left = 0, right = nElements;

        printf("Thread %lu started for target %lld\n", pthread_self(), target); // Log thread ID and target

        while (left < right) {
                int mid = left + (right - left) / 2;
                if (inputArray[mid] < target) {
                        left = mid + 1;
                } else {
                        right = mid;
                }
        }

        task->result[task->index] = left; // Store the result
        long long foundIndex = left;
        printf("Thread %lu finished for target %lld, found index %d\n", pthread_self(), target, foundIndex); // Log result
        return NULL;
}

void generate_sorted_array(long long *array, int size) {
        for (int i = 0; i < size; i++) {
                array[i] = rand() % (size * 10); // Random values
        }
        // Sort the array
        qsort(array, size, sizeof(long long), compare);

       // printf("\n");
       // for (int i = 0; i < size; i++)
       //         printf("%lld ", array[i]);

       // printf("\n");
}


int main(int argc, char *argv[]) {

    chronometer_t parallelReductionTime;

        if (argc < 3) {
                printf("Usage: %s <nElements> <target1> <target2> ...\n", argv[0]);
                return 1;
        }


        nElements = atoi(argv[1]);
        int nTargets = argc - 2;
        long long targets[nTargets];
        int results[nTargets];

        // Allocate and generate the sorted input array
        inputArray = malloc(nElements * sizeof(long long));
        srand(time(NULL)); // Seed for randomness
        generate_sorted_array(inputArray, nElements);

        // Fill targets from command line arguments
        for (int i = 0; i < nTargets; i++) {
                targets[i] = atoll(argv[i + 2]);
        }

    chrono_reset( &parallelReductionTime );
    chrono_start( &parallelReductionTime );
        // Create threads for each target
        for (int i = 0; i < nTargets; i++) {
                SearchTask *task = malloc(sizeof(SearchTask));
                task->target = targets[i];
                task->result = results;
                task->index = i;
                pthread_create(&threads[i], NULL, bsearch_lower_bound_task, task);
        }

        // Wait for all threads to finish
        for (int i = 0; i < nTargets; i++) {
                pthread_join(threads[i], NULL);
        }

    chrono_stop( &parallelReductionTime );

        // Output the results
        for (int i = 0; i < nTargets; i++) {
                printf("Insertion point for %lld is %d\n", targets[i], results[i]);
        }

    chrono_reportTime( &parallelReductionTime, "parallelReductionTime" );

            double total_time_in_seconds = (double) chrono_gettotal( &parallelReductionTime ) /
                                      ((double)1000*1000*1000);


        free(inputArray);
        return 0;
}
