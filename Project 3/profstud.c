// Marco Seman and Jason Lu
// Project 3 - Professor and Student Problem

#include <stdio.h> 
#include <semaphore.h> 
#include <pthread.h> 
#include <stdlib.h> 
#define Allocate 0
// Setting up global Semaphores operations for synchronization
// Sem_t is used for signal and wait called since they are needed to call into the OS kernel
sem_t QStart;
sem_t AStart;
sem_t ADone;
sem_t QReady;

// Question start is the first thing that begins in this program. 
// When the student is ready to ask a question, this iteration will begin
void QuestionStart(int Student) {
int id = Student;
// When the student is ready to ask a question, this will display which student is ready to ask
  printf("Student %d: The student is ready to ask a question.\n", id);
// we will then use a semaphore wait function that locks the semaphore reference until the question is ready to be asked.   
  sem_wait(&QReady);
// The semaphore wait will then lock the semaphore question start amd wont be answered until the chance to ask a question begins.
  sem_wait(&QStart);
// we then end every void function with the sched command to check if the other threads are in the ready state
  sched_yield();
}//end of questionstart

// Question Done will begin when a student finishes asking a question when it is his/her turn
void QuestionDone(int Student) {
// When the question is done being asked, we will need to unlock the answer start which will essentially tell the professor to answer the question
  sem_post(&AStart);
// we then need to lock the answer done until the professor answers the question
  sem_wait(&ADone);
// Output the proper results of when the student finishes with his/her question
  printf("Student %d : The student receives the answer and leaves.\n", Student);
// We then will unlock the question function to allow another student to ask a question
  sem_post(&QStart);
// Make sure the other function are ready
  sched_yield();
} // end of questiondone

// This function represents a student asking the question
void * OneStudent(void * ID) {
// Each student is assigned an numeric value
  int Student = * ((int * )(&ID));
// the student id reference will then be passed to the question start function
  QuestionStart(Student);
// The function will return the student id when the question is being asked
  printf("Student %d : The student is asking a question.\n", Student);
// the student id will then be passed to question done being asked 
  QuestionDone(Student);
// the function will then return the students id that was asking the question
  printf("Student %d : The student is done asking a question.\n", Student);
// Make sure the other function are ready
  sched_yield();
  return 0;
} // end of student

// Function will begin when the professor starts answering the students question
void AnswerStart() {
// When the answer starts
  sem_wait(&AStart); //wait until student has finished asking the question
// every function must end with sched to make sure the other functions are ready
  sched_yield();
} // end of answer start

// Function will begin when the professor successfully answers the students question
void AnswerDone() {
// Display when the professor successfully answers the question
  printf("The professor is finished with the answer.\n");
// We will then unclock the semaphore allowing the next question to be asked
  sem_post(&ADone);
// every function must end with sched to make sure the other functions are ready
  sched_yield();
} // end of answer done

// This function represents a professor asked the question
void * Professor(void * Students) {
// The number of students asking a question will be assined to i
  long i = * ((int * )(&Students));
//while loop for the amount of student with questions being ready to be answered
  while (i > 0) {
// The professor is ready to ask te question, hense unlocking the question ready semaphore
    sem_post(&QReady);
    printf("The professor wants to be asked a question.\n");
// The professor can then start answering the question
    AnswerStart();
// the professor can then being answering the question
    printf("The Professor is answering the question.\n");
    AnswerDone();
    i--;
  }
  return 0;
} // end of professor

// main always begins with argc and argv
int main(int argc, char* argv[]) {
// The number of students entered by the user. we first have to convert char *argv to an int (Used long because i kept getting an error)
  long StudentNo = atoi(argv[1]); 
//pthread attr allows us to set various attributes of a thread so that a scheduling stack can be used
  pthread_attr_t attr;

// We need to uniquely identify a thread for the professor and student number
  pthread_t prof;
  pthread_t stud[StudentNo];

  //Initialize semaphoressemaphores and allocate them appropriately
  sem_init(&QStart, Allocate, 1);
  sem_init(&AStart, Allocate, 0);
  sem_init(&ADone, Allocate, 0);
  sem_init(&QReady, Allocate, 0);

// this function then sets the values that are going to be used when the thread has been created
  pthread_attr_init(&attr);
// This function allows us to reuse threads as soon as they are terminated
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

// We create a thread for the professor
  pthread_create(&prof, &attr, Professor, (void * ) StudentNo);

// Create multiple threads for the student
long i;
while (i < StudentNo){
pthread_create(&stud[i], &attr, OneStudent, (void*)(i + 1));
 i++; 
}

// We will then suspend the professor's thread and have it join main
  pthread_join(prof, NULL);

i=0;
// each of the students threads will be suspended and then join main
  while( i < StudentNo) {
    pthread_join(stud[i], NULL);
i++;
  }
// end of program
  pthread_exit(0);
  return 0;
}// end main
