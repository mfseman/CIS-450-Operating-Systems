// Marco Seman and Jason Lu
// Project 3 - Bridge Problem

#include <pthread.h>
#include <time.h> 
#include <stdbool.h> 
#include <stdio.h> 
#include <stdlib.h>

# define Threads 50
# define CapSafe 3;

// number of vehicles cannot exceed 3 in the bridge
int NumOfVehicles, presentDirection = 0; // For Direction (0 = north, 1 = south, -1 = no cars)

pthread_mutex_t NumOfVehicles_mutex;
pthread_cond_t safe;

// For isSafe function, we must make sure that the vehicles have completed their
// route from one direction in order for the other values to pass by successfully
bool isSafe(int direc) {
    bool safe;

    // If the bridge is empty, then it is safe to cross.
    // and is room (less than 3) in the bridge going towards the same direction, then it is safe to cross.
    if (NumOfVehicles == 0 || (NumOfVehicles < 3) && (presentDirection == direc)) {
        safe = true;;
    }
    // User cannot cross the bridge if it
    else {
	// make them wait
        safe = false;
    }
    return safe;
}

// This function will begin once a vehicle has arrived the bridge
// direc is the direction the vehicle is going (north or south)
// each vehicle is assigned an id, so we dont have the same vehicle cross twice
void ArriveBridge(int direc, int id) {
    // Begin with locking the mutex
    pthread_mutex_lock( &NumOfVehicles_mutex);

    // while loop that calls the isSafe function and continues until it is safe to drive
    while (!isSafe(direc)) {
        // make sure to wait until it is safe to cross
        pthread_cond_wait( &safe, &NumOfVehicles_mutex);
    }

    // Vehicle is crossing the bridge
    if(direc == 0) {
	printf("Car %d dir North arrives at bridge. \n", id);
    }
    if (direc == 1){
        printf("Car %d dir South arrives at bridge. \n", id);
}

    // Adding the number of vehicles, then making the present direction to direc
    NumOfVehicles++;
    presentDirection = direc;

    //Unlocking the mutex (Discarding it wouldn't work)
    pthread_mutex_unlock( & NumOfVehicles_mutex);

    // Check the functions of other threads to see if they are ready to run
    sched_yield();
    return;
}

// This function will assist a vehicle with crossing the bridge
void CrossBridge(int direc, int id) {
   // Begin with locking the mutex
    pthread_mutex_lock( & NumOfVehicles_mutex);

    // In the process of crossing the bridge
    // C doesn't use strings, so i had to improvise
    if(direc == 0) {
	if(presentDirection == 0){
	    printf("Car %d dir North crossing the bridge. The current dir is North. #Cars: %d. \n", id, NumOfVehicles);
	}
	if(presentDirection == 1){
	    printf("Car %d dir North crossing the bridge. The current dir is South. #Cars: %d. \n", id, NumOfVehicles);
	}
    }
    if (direc == 1){
    	if(presentDirection == 0){
	    printf("Car %d dir South crossing the bridge. The current dir North. #Cars: %d. \n", id, NumOfVehicles);
	}
	if(presentDirection == 1){
	    printf("Car %d dir South crossing the bridge. The current dir: South. #Cars: %d. \n", id, NumOfVehicles);
	}
}

    // Unlocking the mutex
    pthread_mutex_unlock( & NumOfVehicles_mutex);
    // Check the functions of other threads to see if they are ready to run
    sched_yield();
    return;
}

// This function will assist the vehicle with exiting the bridge
void ExitBridge(int direc, int id) {
    // Always start with locking the mutex
    pthread_mutex_lock( & NumOfVehicles_mutex);

    //Decrement the number of vehicles crossing the bridge.
    NumOfVehicles--;

    if(direc == 0) {
         printf("Car %d dir North exits from bridge. \n", id);
    }
    if (direc == 1){
	 printf("Car %d dir South exits from bridge. \n", id);
}

    //Use the safe condition so that another vehicle can cross the bridge
    pthread_cond_broadcast( & safe);

    //Unlocking the mutex
    pthread_mutex_unlock( & NumOfVehicles_mutex);

    sched_yield();
    return;
}

// void function used for vehicles to cross the bridge
void * OneVehicle(void * vehicleID) {
    int id = * ((int * )( & vehicleID));
    // random direction generator
    int direc = rand() % 2;

    ArriveBridge(direc, id);
    CrossBridge(direc, id);
    ExitBridge(direc, id);
    // exit the pthread
    pthread_exit(NULL);
}

// This is executed first
int main(int argc, char * argv[]) {
    long i = 0;
    pthread_t threads[Threads];
    pthread_attr_t attr;

    // Initialize mutex objects
    pthread_mutex_init( &NumOfVehicles_mutex, NULL);

    // Initialize conditions objects
    pthread_cond_init( &safe, NULL);

    // Threads are created in a joinable state
    pthread_attr_init( & attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // Random number generator with srand
    srand(time(NULL));

    // A for loop is used to create a thread
    while (i < Threads) {
        pthread_create(&threads[i], & attr, OneVehicle, (void*) i);
	i++; 
}
	i=0;
    // for loop used to have threads join in groups
    while(i < Threads) {
        pthread_join(threads[i], NULL);
	i++;
    }

    printf("%d threads have been generated.\n", Threads);
}
