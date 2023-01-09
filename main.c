//
// Created by Gökberk Köksoy on 4.01.2023.
//

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

// a linked list node called typedef Car with int id, int chassis, int tires, int seats, int engines, int tops, int painting, mutex
typedef struct Car {
    int id;
    int chassis;
    int tires;
    int seats;
    int engines;
    int tops;
    int painting;
    pthread_mutex_t mutex;
    struct Car *next;
} Car;

#define NUM_TECHNICIANS 6
int findNumOfCarsCanProducedPerDay(const int limits[], int size);
void initializeThreads(pthread_t threads[], void *startRoutine, void *arg, int size);
void initializeCar(Car *car, int id);
void printCars(Car *car);
void aTypeAction(Car *car);
void bTypeAction(Car *car);
void cTypeAction(Car *car);
void dTypeAction(Car *car);
void addNewCarWithChassis(Car *car);
void setChassis(Car *car);
sem_t sem;
int chassisLimit, paintingLimit, tireLimit, seatLimit, engineLimit, topLimit;
pthread_mutex_t carMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t barrier;


Car *head = NULL;

int main(int argc, char *argv[]) {
    // Open the input file
    FILE* input_file = fopen(argv[1], "r");

    // Read the first line of the input file
    int aTypeNum, bTypeNum, cTypeNum, dTypeNum, numberOfDays = 0;

    fscanf(input_file, "%d %d %d %d %d", &aTypeNum, &bTypeNum, &cTypeNum, &dTypeNum, &numberOfDays);

    printf("%d %d %d %d %d\n", aTypeNum, bTypeNum, cTypeNum, dTypeNum, numberOfDays);

    // Read the second line of the input file
    int work_limits[NUM_TECHNICIANS];
    fscanf(input_file, "%d %d %d %d %d %d", &work_limits[0], &work_limits[1], &work_limits[2], &work_limits[3], &work_limits[4], &work_limits[5]);
    chassisLimit = work_limits[0];
    paintingLimit = work_limits[1];
    tireLimit = work_limits[2];
    seatLimit = work_limits[3];
    engineLimit = work_limits[4];
    topLimit = work_limits[5];

    printf("Work limit for chassis operations: %d\n", chassisLimit);
    printf("Work limit for painting operations: %d\n", paintingLimit);
    printf("Work limit for tire operations: %d\n", tireLimit);
    printf("Work limit for seat operations: %d\n", seatLimit);
    printf("Work limit for engine operations: %d\n", engineLimit);
    printf("Work limit for top operations: %d\n", topLimit);

    // allocate memory for the head node
    head = (Car *) malloc(sizeof (Car));
    initializeCar(head, 0);
    int maxNumOfCarsCanProduced = findNumOfCarsCanProducedPerDay(work_limits, NUM_TECHNICIANS);
    sem_init(&sem, 0, maxNumOfCarsCanProduced);

    // create thread a called aType, bType, cType, dType
    pthread_t aType[aTypeNum];
    pthread_t bType[bTypeNum];
    pthread_t cType[cTypeNum];
    pthread_t dType[dTypeNum];

    // initialize mutexes
    initializeThreads(aType, (void *) aTypeAction, (void *) head, aTypeNum);
    initializeThreads(bType, (void *) bTypeAction, (void *) head, bTypeNum);
    initializeThreads(cType, (void *) cTypeAction, (void *) head, cTypeNum);
    initializeThreads(dType, (void *) dTypeAction, (void *) head, dTypeNum);

    // wait for other 3 types

    for(int j = 0; j < bTypeNum; j++) {
        pthread_join(bType[j], NULL);
    }

    sleep(1);
    printCars(head);

    // Close the input file
    fclose(input_file);
    return 0;
}

// a function that takes the integer array as an input and returns the minimum value in the array
int findNumOfCarsCanProducedPerDay(const int limits[], int size) {
    int min = limits[0];
    for (int i = 1; i < size; i++) {
        if (limits[i] < min) {
            min = limits[i];
        }
    }
    return min;
}

void initializeCar(Car *car, int id) {
    car->id = id;
    car->chassis = 0;
    car->tires = 0;
    car->seats = 0;
    car->engines = 0;
    car->tops = 0;
    car->painting = 0;
    pthread_mutex_init(&car->mutex, NULL);
    car->next = NULL;
}

// traverse the linked list and print all nodes
void printCars(Car *car) {
    pthread_mutex_lock(&carMutex);
    Car *current = car;
    while (current != NULL) {
        printf("Car id: %d, car chassis: %d, tire: %d, seat: %d, engine: %d, topCover: %d, paint: %d\n", current->id, current->chassis, current->tires, current->seats, current->engines, current->tops, current->painting);
        current = current->next;
    }
    pthread_mutex_unlock(&carMutex);
}

// add and initialize new node to the end of car linked list
void addNewCarWithChassis(Car *car) {
    while (car->next != NULL) {
        car = car->next;
    }
    car->next = (Car *) malloc(sizeof (Car));
    initializeCar(car->next, car->id + 1);
    fprintf(stderr, "car added with id: %d, created by thread: %lu\n", car->next->id, pthread_self());
    setChassis(car->next);
}

// set chassis to 1 of a car
void setChassis(Car *car) {
    pthread_mutex_lock(&car->mutex);
    car->chassis = 1;
    pthread_mutex_unlock(&car->mutex);
}

void initializeThreads(pthread_t threads[], void *startRoutine, void *arg, int size) {
    for(int i = 0; i < size; i++) {
        pthread_create(&threads[i], NULL, startRoutine, arg);
    }
}

void aTypeAction(Car *car) {
    pthread_exit(NULL);
}

void bTypeAction(Car *car) {
    while(sem_trywait(&sem) == 0) {
        pthread_mutex_lock(&carMutex);
        addNewCarWithChassis(car);
        pthread_mutex_unlock(&carMutex);
    }
    pthread_exit(NULL);
}

void cTypeAction(Car *car) {
    pthread_exit(NULL);
}
void dTypeAction(Car *car) {
    pthread_exit(NULL);
}


}