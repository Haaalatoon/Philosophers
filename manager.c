#include "philo.h"

// Check if both forks needed by philosopher are available
static int forks_available(t_data *data, int philosopher_id)
{
    int left_fork = philosopher_id;
    int right_fork = (philosopher_id + 1) % data->num_philosophers;
    
    return (data->fork_states[left_fork] == FORK_FREE && 
            data->fork_states[right_fork] == FORK_FREE);
}

// Reserve forks for a philosopher
static void reserve_forks(t_data *data, int philosopher_id)
{
    int left_fork = philosopher_id;
    int right_fork = (philosopher_id + 1) % data->num_philosophers;
    
    data->fork_states[left_fork] = FORK_RESERVED;
    data->fork_states[right_fork] = FORK_RESERVED;
}

// Free forks when philosopher is done eating
static void free_forks(t_data *data, int philosopher_id)
{
    int left_fork = philosopher_id;
    int right_fork = (philosopher_id + 1) % data->num_philosophers;
    
    data->fork_states[left_fork] = FORK_FREE;
    data->fork_states[right_fork] = FORK_FREE;
}

// Grant permission to philosopher by setting gate_flag
static void grant_permission(t_data *data, int philosopher_id)
{
    data->philosophers[philosopher_id].gate_flag = 1;
    reserve_forks(data, philosopher_id);
}

// Process completion signals from philosophers
static void handle_completions(t_data *data)
{
    int i;
    
    i = 0;
    while (i < data->num_philosophers)
    {
        if (data->philosophers[i].done_flag)
        {
            // Philosopher finished eating, free their forks
            free_forks(data, i);
            data->philosophers[i].done_flag = 0; // Reset flag
        }
        i++;
    }
}

// Main manager routine - processes FIFO queue
void *manager_routine(void *arg)
{
    t_data *data = (t_data *)arg;
    int philosopher_id;
    
    while (!data->simulation_over)
    {
        pthread_mutex_lock(&data->admission_mutex);
        
        // Handle any completion signals first
        handle_completions(data);
        
        // Process admission queue (FIFO)
        if (!queue_is_empty(&data->admission_queue))
        {
            philosopher_id = queue_peek(&data->admission_queue);
            
            // Check if this philosopher can be granted access
            if (forks_available(data, philosopher_id))
            {
                // Remove from queue and grant permission
                queue_dequeue(&data->admission_queue);
                grant_permission(data, philosopher_id);
            }
        }
        
        pthread_mutex_unlock(&data->admission_mutex);
        
        // Manager backoff - prevent busy waiting
        usleep(500); // 0.5ms
    }
    
    return (NULL);
}