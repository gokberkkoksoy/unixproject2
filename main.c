//
// Created by Gökberk Köksoy on 4.01.2023.
//

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
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
void initializeCar(Car *car, int id);
void aTypeAction(Car *car);
void bTypeAction(Car *car);
void cTypeAction(Car *car);
void dTypeAction(Car *car);
void addNewCarWithChassis(Car *car, int currentday);
void setChassis(Car *car);
int findCarsProduced(Car *car);
int day = 1;
int numberOfDays = 0;
sem_t sem, chassisSem, topCoverSem, paintSem;
pthread_mutex_t logMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t carMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t dayMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t barrier, daybarrier;
pthread_cond_t daycond = PTHREAD_COND_INITIALIZER;

Car *head = NULL;
FILE *input_file;
FILE *output_file;

int main(int argc, char *argv[]) {
    // Open the input file
    input_file = fopen(argv[1], "r");
    output_file = fopen("output.txt", "w");

    // Read the first line of the input file
    int aTypeNum, bTypeNum, cTypeNum, dTypeNum = 0;

    fscanf(input_file, "%d %d %d %d %d", &aTypeNum, &bTypeNum, &cTypeNum, &dTypeNum, &numberOfDays);
    int carsProducedEachDay[numberOfDays];

    // Read the second line of the input file
    int work_limits[NUM_TECHNICIANS];
    fscanf(input_file, "%d %d %d %d %d %d", &work_limits[0], &work_limits[1], &work_limits[2], &work_limits[3], &work_limits[4], &work_limits[5]);

    // allocate memory for the head node
    head = (Car *)malloc(sizeof(Car));
    initializeCar(head, 0);
    int maxNumOfCarsCanProduced = findNumOfCarsCanProducedPerDay(work_limits, NUM_TECHNICIANS);
    sem_init(&sem, 0, maxNumOfCarsCanProduced);
    sem_init(&chassisSem, 0, 0);
    sem_init(&topCoverSem, 0, 0);
    sem_init(&paintSem, 0, 0);

    // create thread a called aType, bType, cType, dType
    pthread_t aType[aTypeNum];
    pthread_t bType[bTypeNum];
    pthread_t cType[cTypeNum];
    pthread_t dType[dTypeNum];

    pthread_barrier_init(&barrier, NULL, aTypeNum + bTypeNum + cTypeNum + dTypeNum);
    pthread_barrier_init(&daybarrier, NULL, aTypeNum + bTypeNum + cTypeNum + dTypeNum + 1);

    for (int i = 0; i < aTypeNum; i++) {
        pthread_create(&aType[i], NULL, (void *)aTypeAction, (void *)head);
    }

    for (int i = 0; i < bTypeNum; i++) {
        pthread_create(&bType[i], NULL, (void *)bTypeAction, (void *)head);
    }

    for (int i = 0; i < cTypeNum; i++) {
        pthread_create(&cType[i], NULL, (void *)cTypeAction, (void *)head);
    }

    for (int i = 0; i < dTypeNum; i++) {
        pthread_create(&dType[i], NULL, (void *)dTypeAction, (void *)head);
    }

    while (day < numberOfDays) {
        pthread_barrier_wait(&daybarrier);
        pthread_mutex_lock(&dayMutex);
        sem_init(&sem, 0, maxNumOfCarsCanProduced);
        carsProducedEachDay[day - 1] = findCarsProduced(head);
        day++;
        pthread_cond_broadcast(&daycond);
        pthread_mutex_unlock(&dayMutex);
    }

    for (int i = 0; i < aTypeNum; i++) {
        pthread_join(aType[i], NULL);
    }

    for (int j = 0; j < bTypeNum; j++) {
        pthread_join(bType[j], NULL);
    }

    for (int i = 0; i < cTypeNum; i++) {
        pthread_join(cType[i], NULL);
    }

    for (int i = 0; i < dTypeNum; i++) {
        pthread_join(dType[i], NULL);
    }

    
    pthread_mutex_lock(&logMutex);

    for(int x=0; x<numberOfDays; x++) {
        fprintf(output_file, "%d cars produced at day: %d\n", carsProducedEachDay[x], x+1);
    }
    
    pthread_mutex_unlock(&logMutex);
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

// add and initialize new node to the end of car linked list
void addNewCarWithChassis(Car *car, int currentday) {
    while (car->next != NULL) {
        car = car->next;
    }
    car->next = (Car *)malloc(sizeof(Car));
    initializeCar(car->next, car->id + 1);
    setChassis(car->next);
    pthread_mutex_lock(&logMutex);
    fprintf(output_file, "Type B - %lu\t%d\tchassis\t%d\n", pthread_self(), car->next->id, currentday);
    pthread_mutex_unlock(&logMutex);
}

// set chassis to 1 of a car
void setChassis(Car *car) {
    pthread_mutex_lock(&car->mutex);
    car->chassis = 1;
    pthread_mutex_unlock(&car->mutex);
}

// adds tires and painting
void aTypeAction(Car *car) {
    do {
        int currentDay = day;
        pthread_barrier_wait(&barrier);
        int semSemvalue, chassisSemvalue;
        sem_getvalue(&sem, &semSemvalue);
        sem_getvalue(&chassisSem, &chassisSemvalue);
        while (semSemvalue != 0 || chassisSemvalue != 0) {
            sem_wait(&chassisSem);
            while (car->next != NULL) {
                if (pthread_mutex_trylock(&car->mutex) == 0) {
                    if (car->tires == 0 && car->chassis == 1) {
                        car->tires = 1;
                        pthread_mutex_lock(&logMutex);
                        fprintf(output_file, "Type A - %lu\t%d\ttires\t%d\n", pthread_self(), car->id, currentDay);
                        pthread_mutex_unlock(&logMutex);
                        if (car->engines == 1 && car->seats == 1) {
                            sem_post(&topCoverSem);
                        } else {
                            sem_post(&chassisSem);
                        }
                        pthread_mutex_unlock(&car->mutex);
                        break;
                    }
                    pthread_mutex_unlock(&car->mutex);
                }
                car = car->next;
            }
            car = head->next;

            if (sem_trywait(&paintSem) == 0) {
                while (car->next != NULL) {
                    if (pthread_mutex_trylock(&car->mutex) == 0) {
                        if (car->chassis == 1 && car->engines == 1 & car->tires == 1 && car->seats == 1 && car->tops == 1 &&
                            car->painting == 0) {
                            car->painting = 1;
                            pthread_mutex_lock(&logMutex);
                            fprintf(output_file, "Type A - %lu\t%d\tpainting\t%d\n", pthread_self(), car->id, currentDay);
                            pthread_mutex_unlock(&logMutex);
                        }
                        pthread_mutex_unlock(&car->mutex);
                    }
                    car = car->next;
                }
                car = head->next;
            }
            sem_getvalue(&sem, &semSemvalue);
            sem_getvalue(&chassisSem, &chassisSemvalue);
        }

        if (day == numberOfDays) {
            break;
        }
        pthread_barrier_wait(&daybarrier);
        pthread_mutex_lock(&dayMutex);

        while (currentDay == day) {
            pthread_cond_wait(&daycond, &dayMutex);
        }

        pthread_mutex_unlock(&dayMutex);
    } while (day <= numberOfDays);

    pthread_exit(NULL);
}

// adds chassis
void bTypeAction(Car *car) {
    do {
        int currentDay = day;
        pthread_barrier_wait(&barrier);
        while (sem_trywait(&sem) == 0) {
            pthread_mutex_lock(&carMutex);
            addNewCarWithChassis(car, currentDay);
            sem_post(&chassisSem);
            pthread_mutex_unlock(&carMutex);
        }

        if (day == numberOfDays) {
            break;
        }
        pthread_barrier_wait(&daybarrier);
        pthread_mutex_lock(&dayMutex);

        while (currentDay == day) {
            pthread_cond_wait(&daycond, &dayMutex);
        }

        pthread_mutex_unlock(&dayMutex);
    } while (day <= numberOfDays);
    pthread_exit(NULL);
}

// adds seats
void cTypeAction(Car *car) {
    do {
        int currentDay = day;
        pthread_barrier_wait(&barrier);
        int semSemvalue, chassisSemvalue;
        sem_getvalue(&sem, &semSemvalue);
        sem_getvalue(&chassisSem, &chassisSemvalue);
        while (semSemvalue != 0 || chassisSemvalue != 0) {
            sem_wait(&chassisSem);
            while (car->next != NULL) {
                if (pthread_mutex_trylock(&car->mutex) == 0) {
                    if (car->seats == 0 && car->chassis == 1) {
                        car->seats = 1;
                        if (car->engines == 1 && car->tires == 1) {
                            sem_post(&topCoverSem);
                        } else {
                            sem_post(&chassisSem);
                        }
                        pthread_mutex_lock(&logMutex);
                        fprintf(output_file, "Type C - %lu\t%d\tseats\t%d\n", pthread_self(), car->id, currentDay);
                        pthread_mutex_unlock(&logMutex);
                        pthread_mutex_unlock(&car->mutex);
                        break;
                    }
                    pthread_mutex_unlock(&car->mutex);
                }
                car = car->next;
            }
            car = head->next;
            sem_getvalue(&sem, &semSemvalue);
            sem_getvalue(&chassisSem, &chassisSemvalue);
        }

        if (day == numberOfDays) {
            break;
        }
        pthread_barrier_wait(&daybarrier);
        pthread_mutex_lock(&dayMutex);

        while (currentDay == day) {
            pthread_cond_wait(&daycond, &dayMutex);
        }

        pthread_mutex_unlock(&dayMutex);
    } while (day <= numberOfDays);
    pthread_exit(NULL);
}
// adds engine
void dTypeAction(Car *car) {
    do {
        int currentDay = day;
        pthread_barrier_wait(&barrier);
        int semSemvalue, chassisSemvalue;
        sem_getvalue(&sem, &semSemvalue);
        sem_getvalue(&chassisSem, &chassisSemvalue);
        while (semSemvalue != 0 || chassisSemvalue != 0) {
            sem_wait(&chassisSem);
            while (car->next != NULL) {
                if (pthread_mutex_trylock(&car->mutex) == 0) {
                    if (car->engines == 0 && car->chassis == 1) {
                        car->engines = 1;
                        pthread_mutex_lock(&logMutex);
                        fprintf(output_file, "Type D - %lu\t%d\tengines\t%d\n", pthread_self(), car->id, currentDay);
                        pthread_mutex_unlock(&logMutex);
                        if (car->tires == 1 && car->seats == 1) {
                            sem_post(&topCoverSem);
                        } else {
                            sem_post(&chassisSem);
                        }
                        pthread_mutex_unlock(&car->mutex);
                        break;
                    }
                    pthread_mutex_unlock(&car->mutex);
                }
                car = car->next;
            }
            car = head->next;

            if (sem_trywait(&topCoverSem) == 0) {
                while (car->next != NULL) {
                    if (pthread_mutex_trylock(&car->mutex) == 0) {
                        if (car->chassis == 1 && car->seats == 1 && car->tires == 1 && car->engines == 1 && car->tops == 0) {
                            car->tops = 1;
                            pthread_mutex_lock(&logMutex);
                            fprintf(output_file, "Type D - %lu\t%d\ttops\t%d\n", pthread_self(), car->id, currentDay);
                            pthread_mutex_unlock(&logMutex);
                        }
                        pthread_mutex_unlock(&car->mutex);
                    }
                    car = car->next;
                }
                car = head->next;
                sem_post(&paintSem);
            }
            sem_getvalue(&sem, &semSemvalue);
            sem_getvalue(&chassisSem, &chassisSemvalue);
        }

        if (day == numberOfDays) {
            break;
        }
        pthread_barrier_wait(&daybarrier);
        pthread_mutex_lock(&dayMutex);

        while (currentDay == day) {
            pthread_cond_wait(&daycond, &dayMutex);
        }

        pthread_mutex_unlock(&dayMutex);
    } while (day <= numberOfDays);
    pthread_exit(NULL);
}

// traverse the linked list, if chassis == 1, tires == 1, engine == 1, seat == 1, tops == 1, painting == 1 increment the counter
int findCarsProduced(Car *car) {
    int result = 0;
    while(car != NULL) {
        if (car->chassis == 1 && car->tires == 1 && car->engines == 1 && car->seats == 1 && car->tops == 1 && car->painting == 1) {
            result++;
        }
        car = car->next;
    }
    return result;
}