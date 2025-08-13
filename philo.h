#ifndef PHILO_H
# define PHILO_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <pthread.h>
# include <sys/time.h>
# include <string.h>

// Forward declaration
struct s_data;

// FIFO Queue for admission requests
typedef struct s_queue
{
    int     *buffer;        // Circular buffer of philosopher IDs
    int     head;           // Next position to dequeue
    int     tail;           // Next position to enqueue
    int     size;           // Current number of elements
    int     capacity;       // Maximum capacity
}   t_queue;

// Individual philosopher state
typedef struct s_philosopher
{
    int                 id;                 // 0-based internally, 1-based for display
    long long           last_meal_time;     // Timestamp of last meal start
    long long           request_time;       // When philosopher requested access
    int                 meals_eaten;        // Count of completed meals
    volatile int        gate_flag;          // Permission from manager (0=wait, 1=proceed)
    volatile int        done_flag;          // Completion signal to manager
    pthread_t           thread;             // Thread handle
    struct s_data       *data;             // Pointer to shared data
}   t_philosopher;

// Fork state tracking for manager
typedef enum e_fork_state
{
    FORK_FREE,          // Available
    FORK_RESERVED,      // Reserved by manager for a philosopher
    FORK_TAKEN          // Currently being used
}   t_fork_state;

// Shared context - NO GLOBAL VARIABLES
typedef struct s_data
{
    // Configuration (read-only after init)
    int                 num_philosophers;
    long                time_to_die;        // ms
    long                time_to_eat;        // ms  
    long                time_to_sleep;      // ms
    int                 meals_required;     // -1 if optional arg not provided
    
    // Runtime state
    long long           start_time;         // Simulation start timestamp
    volatile int        simulation_over;    // Flag to stop all threads
    
    // Shared resources
    t_philosopher       *philosophers;      // Array of philosopher states
    pthread_mutex_t     *forks;            // Array of fork mutexes
    t_fork_state        *fork_states;      // Fork state tracking for manager
    
    // Admission control system
    t_queue             admission_queue;    // FIFO queue for requests
    pthread_mutex_t     admission_mutex;    // Protects queue and fork_states
    
    // Thread synchronization
    pthread_mutex_t     print_mutex;       // For thread-safe logging
    pthread_t           manager_thread;     // Manager thread handle
    pthread_t           watchdog_thread;    // Watchdog thread handle
}   t_data;

// Function declarations
// init.c
int         parse_arguments(int argc, char **argv, t_data *data);
int         init_data(t_data *data);
void        cleanup_data(t_data *data);

// utils.c  
long long   get_time_ms(void);
void        ft_sleep(long ms);
void        print_status(t_philosopher *philo, char *status);

// queue.c
int         queue_init(t_queue *queue, int capacity);
void        queue_destroy(t_queue *queue);
int         queue_enqueue(t_queue *queue, int philosopher_id);
int         queue_dequeue(t_queue *queue);
int         queue_peek(t_queue *queue);
int         queue_is_empty(t_queue *queue);
int         queue_size(t_queue *queue);
int         queue_contains(t_queue *queue, int philosopher_id);

// philosopher.c
void        *philosopher_routine(void *arg);

// manager.c
void        *manager_routine(void *arg);

// watchdog.c
void        *watchdog_routine(void *arg);

#endif