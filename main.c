//
// Created by Gökberk Köksoy on 4.01.2023.
//

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Car {
    int id;
    int chassis;
    int tires;
    int seats;
    int engines;
    int tops;
    int painting;
} Car;

#define NUM_TECHNICIANS 6
int findNumOfCarsCanProducedPerDay(const int limits[], int size);
void initializeCars(Car cars[], int size);
void printCars(Car cars[], int size);
void* aTypeWorker();
void* bTypeWorker();
void* cTypeWorker();
void* dTypeWorker();

int main(int argc, char* argv[]) {
    // Open the input file
    FILE* input_file = fopen(argv[1], "r");

    // Read the first line of the input file
    int aTypeNum, bTypeNum, cTypeNum, dTypeNum, numberOfDays = 0;
    int chassisLimit, paintingLimit, tireLimit, seatLimit, engineLimit, topLimit;
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

    int maxNumOfCarsCanProduced = findNumOfCarsCanProducedPerDay(work_limits, NUM_TECHNICIANS);
    Car cars[maxNumOfCarsCanProduced];
    initializeCars(cars, maxNumOfCarsCanProduced);

    printCars(cars, maxNumOfCarsCanProduced);

    // Close the input file
    fclose(input_file);

    for (int i = 0; i < aTypeNum; i++) {
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, &aTypeWorker, &i);
    }

    for (int i = 0; i < bTypeNum; i++)
    {
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, &bTypeWorker, &i);
    }

    for (int i = 0; i < cTypeNum; i++)
    {
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, &cTypeWorker, &i);
    }

    for (int i = 0; i < dTypeNum; i++)
    {
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, &dTypeWorker, &i);
    }

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

void initializeCars(Car cars[], int size) {
    for (int i = 0; i < size; i++) {
        cars[i].id = i;
        cars[i].chassis = 0;
        cars[i].tires = 0;
        cars[i].seats = 0;
        cars[i].engines = 0;
        cars[i].tops = 0;
        cars[i].painting = 0;
    }
}

// a function that lists all attributes of the car struct inside the array
void printCars(Car cars[], int size) {
    for (int i = 0; i < size; i++) {
        printf("Car %d: chassis: %d, tires: %d, seats: %d, engines: %d, tops: %d, painting: %d\n", cars[i].id, cars[i].chassis, cars[i].tires, cars[i].seats, cars[i].engines, cars[i].tops, cars[i].painting);
    }
}

void* aTypeWorker(void* arg) {
    printf("I am technician A-%d!\n", *(int*)arg);
}

void* bTypeWorker(void* arg) {
    printf("I am technician B-%d!\n", *(int*)arg);
}
void* cTypeWorker(void* arg) {
    printf("I am technician C-%d!\n", *(int*)arg);
}

void* dTypeWorker(void* arg) {
    printf("I am technician D-%d!\n", *(int*)arg);
}
