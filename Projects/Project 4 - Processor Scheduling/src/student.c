/*
 * student.c
 * Multithreaded OS Simulation for CS 2200
 *
 * This file contains the CPU scheduler for the simulation.
 */

#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h> //ERIC ADDED THIS (but they said we can use the getopt function so that's why I did?)

#include "student.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/** Function prototypes **/
extern void idle(unsigned int cpu_id);
extern void preempt(unsigned int cpu_id);
extern void yield(unsigned int cpu_id);
extern void terminate(unsigned int cpu_id);
extern void wake_up(pcb_t *process);



/**
 * current is an array of pointers to the currently running processes, 
 * each pointer corresponding to each CPU in the simulation.
 * ]
 * NOTE: This is NOT a list of lists. This variable is simply an array of pointers--and those pointers
 * point to the currently executing process of that CPU (whose ID is the index we just accessed)
 *
 * e.g.:
 * current[0] = pointer to the process running on the CPU whose ID is 0
 * current[4] = pointer to the process running on the CPU whose ID is 4
 */
static pcb_t **current;
/* rq is a pointer to a struct you should use for your ready queue implementation.*/
static queue_t *rq;

/**
 * current and rq are accessed by multiple threads, so you will need to use 
 * a mutex to protect it (ready queue).
 *
 * The condition variable queue_not_empty has been provided for you
 * to use in conditional waits and signals.
 */
static pthread_mutex_t current_mutex;
static pthread_mutex_t queue_mutex;
static pthread_cond_t queue_not_empty;

/* keeps track of the scheduling algorithm and cpu count */
static sched_algorithm_t scheduler_algorithm;
static unsigned int cpu_count;
static int timeslice; //we want this to be signed because a timeslice of -1 means an infinite timeslice

/** ------------------------Problem 0 & 2-----------------------------------
 * Checkout PDF Section 2 and 4 for this problem
 * 
 * enqueue() is a helper function to add a process to the ready queue.
 *
 * @param queue pointer to the ready queue
 * @param process process that we need to put in the ready queue
 */
void enqueue(queue_t *queue, pcb_t *process) {
    if (is_empty(queue)) { //if the queue is empty, it doesn't matter which method of enqueueing we use
        queue->head = process;
        queue->tail = process;

        //If the queue was empty, we might be waiting for it to be refilled.
        //Signal that the queue has at least one element.
        pthread_cond_signal(&queue_not_empty);
        return;
    }

    pcb_t *traverse_node = queue->head;
    if (scheduler_algorithm == PR) { //Preemptive Priority ---- LOWER NUMBER IS HIGHER PRIORITY
        //initially check if the new process' priority is higher than the head of the ready queue.
        //We need to do this since in the while loop below we have to check the "next" nodes instead of the current
        //nodes because if we check the current nodes we can't insert the new process into the queue correctly.
        if (queue->head->priority > process->priority) {
            process->next = queue->head; //first set new process' next node to the head
            queue->head = process;       //then set the head to the new process (order of the last two lines matters)
            return;
        }

        //if not, then traverse the queue to see where the lowest priority is and insert ourselves there.
        while (traverse_node->next != NULL) {
            if (traverse_node->next->priority > process->priority) { //curr->next less important than new process
                process->next = traverse_node->next; //first, set new process next to next node
                traverse_node->next = process;       //then set current traversed node to new process(order matters)
                return;
            } //else we keep going
            traverse_node = traverse_node->next;
        }

        //The underlying assumption is at this point is that    traverse_node->next == NULL     (hence our while
        //loop stopped) we HAVE checked the priority on the tail and thus all nodes in our queue have a higher priority
        //than the new process. We will exit the if-statement and add to the tail just like Round-Robin and FCFS do.
        assert(traverse_node->next == NULL); //For testing purposes
    }
    //RR and FCFS enqueue are both at the back; PR enqueue may be the back if all of the nodes in the queue have a
    //higher priority than the new process. So the following lines add to the tail of the queue (like normal)
    queue->tail->next = process;     //first set the tail of the queue to the new process
    queue->tail = queue->tail->next; //then set the tail to the new process (order of last two lines matters)
}

/**
 * dequeue() is a helper function to remove a process to the ready queue.
 *
 * @param queue pointer to the ready queue
 */
pcb_t *dequeue(queue_t *queue) {
    if (is_empty(queue)) {
        return NULL;
    }

    pcb_t *node_to_remove = queue->head;
    queue->head = queue->head->next;
    return node_to_remove;
}

/** ------------------------Problem 0-----------------------------------
 * Checkout PDF Section 2 for this problem
 * 
 * is_empty() is a helper function that returns whether the ready queue
 * has any processes in it.
 * 
 * @param queue pointer to the ready queue
 * @return a boolean value that indicates whether the queue is empty or not
 */
bool is_empty(queue_t *queue) {
    return queue->head == NULL;
}

/** ------------------------Problem 1B & 3-----------------------------------
 * Checkout PDF Section 3 and 5 for this problem
 * 
 * schedule() is your CPU scheduler.
 * 
 * Remember to specify the timeslice if the scheduling algorithm is Round-Robin
 * NOTE TO SELF: the timeslice is the UPPER BOUND (maximum time) for a process to run
 *               before switching. If a process ends before its timeslice ends, then the
 *               next scheduled process gets a new FULL timeslice, not the remainder of the
 *               timeslice of the process that ended before it.
 * 
 * @param cpu_id the target cpu we decide to put our process in
 */
static void schedule(unsigned int cpu_id) {
    //call context_switch to simulate putting a new process on a CPU
    pthread_mutex_lock(&queue_mutex);
    pcb_t *next_process = dequeue(rq);
    pthread_mutex_unlock(&queue_mutex);

    //Check if the ready queue is empty (and wait until it is not to continue)
//    while (NULL == next_process) { //dequeue() will return NULL if the queue is empty; We check if the queue is empty
//        pthread_cond_wait(&queue_not_empty, &queue_mutex);
//        next_process = dequeue(rq); //re-update the next_process pointer with the head of rq
//    }

    //Put a new process on this CPU
    context_switch(cpu_id, next_process, timeslice);

    //Update the current array. First lock "current"
    pthread_mutex_lock(&current_mutex);

    //NOTE: It is the job of functions that call schedule() to update the state of the process running on current
    //before schedule is called. For example, terminate() will set the process' state to PROCESS_TERMINATED and
    //other functions like preempt and yield will set it to PROCESS_WAITING
    current[cpu_id] = next_process; //set the "current" array's index of the cpi_id to the new process
    current[cpu_id]->state = PROCESS_RUNNING; //Update the running process' state

    pthread_mutex_unlock(&current_mutex);
//    pthread_mutex_unlock(&queue_mutex);
}

/**  ------------------------Problem 1A-----------------------------------
 * Checkout PDF Section 3 for this problem
 * 
 * idle() is your idle process.  It is called by the simulator when the idle
 * process is scheduled.
 *
 * @param cpu_id the cpu that is waiting for process to come in
 */
extern void idle(unsigned int cpu_id) {
    /* FIX ME */
//    schedule(0);


    pthread_mutex_lock(&queue_mutex);
    pcb_t *head = rq->head;
    while (NULL == head) { //dequeue() will return NULL if the queue is empty; We check if the queue is empty
        pthread_cond_wait(&queue_not_empty, &queue_mutex);
        head = dequeue(rq); //re-update the next_process pointer with the head of rq
    }
    pthread_mutex_unlock(&queue_mutex);
    schedule(cpu_id);

    /*
     * REMOVE THE LINE BELOW AFTER IMPLEMENTING IDLE()
     *
     * idle() must block when the ready queue is empty, or else the CPU threads
     * will spin in a loop.  Until a ready queue is implemented, we'll put the
     * thread to sleep to keep it from consuming 100% of the CPU time.  Once
     * you implement a proper idle() function using a condition variable,
     * remove the call to mt_safe_usleep() below.
     */
//    mt_safe_usleep(1000000);
}

/** ------------------------Problem 2 & 3-----------------------------------
 * Checkout Section 4 and 5 for this problem
 * 
 * preempt() is the handler used in Round-robin and Preemptive Priority 
 * Scheduling
 *
 * This function should place the currently running process back in the
 * ready queue, and call schedule() to select a new runnable process.
 * 
 * @param cpu_id the cpu in which we want to preempt process
 */
extern void preempt(unsigned int cpu_id) {
    pthread_mutex_lock(&current_mutex);
    current[cpu_id]->state = PROCESS_WAITING;
    pthread_mutex_unlock(&current_mutex);

    pthread_mutex_lock(&queue_mutex);
    enqueue(rq, current[cpu_id]);
    pthread_mutex_unlock(&queue_mutex);

    schedule(cpu_id);
}

/**  ------------------------Problem 1-----------------------------------
 * Checkout PDF Section 3 for this problem
 * 
 * yield() is the handler called by the simulator when a process yields the
 * CPU to perform an I/O request.
 *
 * @param cpu_id the cpu that is yielded by the process
 */
extern void yield(unsigned int cpu_id) {
    //lock the current array
    pthread_mutex_lock(&current_mutex);

    //The process now must go into the I/O ready queue to be serviced; set its current state to WAITING
    current[cpu_id]->state = PROCESS_WAITING;

    //Unlock the current array
    pthread_mutex_unlock(&current_mutex);

    //This CPU no longer has anything running on it; let's now schedule a new process on it.
    schedule(cpu_id);
}

/**  ------------------------Problem 1-----------------------------------
 * Checkout PDF Section 3
 * 
 * terminate() is the handler called by the simulator when a process completes.
 * 
 * @param cpu_id the cpu we want to terminate
 */
extern void terminate(unsigned int cpu_id) {
    //set the status of the newly terminated process to TERMINATED
    pthread_mutex_lock(&current_mutex);
    current[cpu_id]->state = PROCESS_TERMINATED;
    pthread_mutex_unlock(&current_mutex);

    //This CPU no longer has anything running on it; let's now schedule a new process on it.
    schedule(cpu_id);
}

/**  ------------------------Problem 1A & 3---------------------------------
 * Checkout PDF Section 3 and 4 for this problem
 * 
 * wake_up() is the handler called by the simulator when a process's I/O
 * request completes. This method will also need to handle priority, 
 * Look in section 5 of the PDF for more info.
 * 
 * @param process the process that finishes I/O and is ready to run on CPU
 */
extern void wake_up(pcb_t *process) {
    process->state = PROCESS_WAITING;

    pthread_mutex_lock(&queue_mutex);
    enqueue(rq, process);
    pthread_mutex_unlock(&queue_mutex);

    if (scheduler_algorithm == PR) { //If the scheduling algo is Preemptive Priority
        pthread_mutex_lock(&current_mutex);
        for (unsigned int i = 0; i < cpu_count; i++) {
            if (current[i]->priority > process->priority) {
                preempt(i);
                break;
            }
        }
        pthread_mutex_unlock(&current_mutex); //we should still execute this after breaking
    } //else for RR and FCFS we don't need to do any pre-emptiveness swapping
}

/**
 * main() simply parses command line arguments, then calls start_simulator().
 * Add support for -r and -p parameters. If no argument has been supplied, 
 * you should default to FCFS.
 * 
 * HINT:
 * Use the scheduler_algorithm variable (see student.h) in your scheduler to 
 * keep track of the scheduling algorithm you're using.
 */
int main(int argc, char *argv[]) {

    /*
     * FIX ME
     */
    //The below commented out code was what was given. Not sure whether i need a part of this or not tho
//    scheduler_algorithm = FCFS;
//
//    if (argc != 2) {
//        fprintf(stderr, "CS 2200 Project 4 -- Multithreaded OS Simulator\n"
//                        "Usage: ./os-sim <# CPUs> [ -r <time slice> | -p ]\n"
//                        "    Default : FCFS Scheduler\n"
//                        "         -r : Round-Robin Scheduler\n1\n"
//                        "         -p : Priority Scheduler\n");
//        return -1;
//    }
    scheduler_algorithm = FCFS; //initially set the algorithm to FCFS (if we don't want that it'll be updated below)
    timeslice = -1; //initially set timeslice to -1 (-1 signifies infinite timeslice for PR and FCFS). else, updated
    int option_char;

    //":r:p:" optstring denotes that we can take either the -r or -p flags (the -r flag requires an argument). The
    //initial colon in the optstring is to allow getopt to return ':' when an argument was expected but not given
    while ((option_char = getopt(argc, argv, ":pr:")) != -1) {
        switch (option_char) {
            case 'r': //Round-Robin scheduler (with timeslice specified)
                scheduler_algorithm = RR;
                timeslice = atoi(optarg); //use atoi() to convert char * to int
                break;
            case 'p': //Preemptive Priority scheduler
                scheduler_algorithm = PR;
                break;
            case ':':
                printf("ERROR: getopt() returned ':' meaning the %s option was expecting an argument and nothing was given\n",
                       argv[optind - 2]);
                fprintf(stderr, "CS 2200 Project 4 -- Multithreaded OS Simulator\n"
                        "Usage: ./os-sim <# CPUs> [ -r <time slice> | -p ]\n"
                        "    Default : FCFS Scheduler\n"
                        "         -r : Round-Robin Scheduler\n1\n"
                        "         -p : Priority Scheduler\n");
                return -1; //return since incorrect command was given
            default: //option_char == '?'
                fprintf(stderr, "CS 2200 Project 4 -- Multithreaded OS Simulator\n"
                        "Usage: ./os-sim <# CPUs> [ -r <time slice> | -p ]\n"
                        "    Default : FCFS Scheduler\n"
                        "         -r : Round-Robin Scheduler\n1\n"
                        "         -p : Priority Scheduler\n");
                printf("ERROR: getopt() returned '?' ...  \"%s\" is not a valid option \n", argv[optind - 1]);
                return -1; //return since incorrect command was given
        }
    }


    /* Parse the command line arguments */
    cpu_count = strtoul(argv[1], NULL, 0);

    /* Allocate the current[] array and its mutex */
    current = malloc(sizeof(pcb_t *) * cpu_count);
    assert(current != NULL);
    pthread_mutex_init(&current_mutex, NULL);
    pthread_mutex_init(&queue_mutex, NULL);
    pthread_cond_init(&queue_not_empty, NULL);
    rq = (queue_t *)malloc(sizeof(queue_t));
    assert(rq != NULL);

    /* Start the simulator in the library */
    start_simulator(cpu_count);

    return 0;
}

#pragma GCC diagnostic pop
