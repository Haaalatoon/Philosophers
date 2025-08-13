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
    int qsize, idx, candidate_id;

    while (!data->simulation_over)
    {
        pthread_mutex_lock(&data->admission_mutex);

        handle_completions(data);

        qsize = queue_size(&data->admission_queue);
        for (idx = 0; idx < qsize; idx++)
        {
            candidate_id = data->admission_queue.buffer[(data->admission_queue.head + idx) % data->admission_queue.capacity];
            if (forks_available(data, candidate_id))
            {
                // Remove candidate from queue and grant permission
                int remove_idx = (data->admission_queue.head + idx) % data->admission_queue.capacity;
                int i = idx;
                while (i > 0)
                {
                    int prev_idx = (remove_idx - 1 + data->admission_queue.capacity) % data->admission_queue.capacity;
                    data->admission_queue.buffer[remove_idx] = data->admission_queue.buffer[prev_idx];
                    remove_idx = prev_idx;
                    i--;
                }
                data->admission_queue.head = (data->admission_queue.head + 1) % data->admission_queue.capacity;
                data->admission_queue.size--;
                grant_permission(data, candidate_id);

                // After granting, update qsize and idx to continue scanning
                qsize--;
                idx--;
            }
        }

        pthread_mutex_unlock(&data->admission_mutex);

        usleep(500);
    }

    return (NULL);
}