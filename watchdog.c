#include "philo.h"

// Find philosopher closest to starvation
static int find_most_starved(t_data *data)
{
    long long current_time = get_time_ms();
    long long max_wait_time = 0;
    long long wait_time;
    int most_starved = -1;
    int i;
    
    i = 0;
    while (i < data->num_philosophers)
    {
        wait_time = current_time - data->philosophers[i].last_meal_time;
        
        if (wait_time > max_wait_time)
        {
            max_wait_time = wait_time;
            most_starved = i;
        }
        i++;
    }
    
    return (most_starved);
}

// Promote starving philosopher to front of queue
static void promote_starving_philosopher(t_data *data, int philosopher_id)
{
    t_queue temp_queue;
    int current_id;
    // int promoted = 0;
    
    // Initialize temporary queue
    if (queue_init(&temp_queue, data->num_philosophers * 2) != 0)
        return;
    
    // Add the starving philosopher first
    queue_enqueue(&temp_queue, philosopher_id);
    // promoted = 1;
    
    // Re-add other philosophers (except the promoted one)
    while (!queue_is_empty(&data->admission_queue))
    {
        current_id = queue_dequeue(&data->admission_queue);
        if (current_id != philosopher_id)
            queue_enqueue(&temp_queue, current_id);
    }
    
    // Copy back to main queue
    while (!queue_is_empty(&temp_queue))
    {
        current_id = queue_dequeue(&temp_queue);
        queue_enqueue(&data->admission_queue, current_id);
    }
    
    queue_destroy(&temp_queue);
}

// Check for philosopher deaths
static int check_deaths(t_data *data)
{
    long long current_time = get_time_ms();
    long long time_since_meal;
    int i;
    
    i = 0;
    while (i < data->num_philosophers)
    {
        time_since_meal = current_time - data->philosophers[i].last_meal_time;
        
        if (time_since_meal > data->time_to_die)
        {
            print_status(&data->philosophers[i], "died");
            data->simulation_over = 1;
            return (1);
        }
        i++;
    }
    return (0);
}

// Check if all philosophers have eaten enough
static int all_fed(t_data *data)
{
    int i;
    
    if (data->meals_required <= 0)
        return (0);
    
    i = 0;
    while (i < data->num_philosophers)
    {
        if (data->philosophers[i].meals_eaten < data->meals_required)
            return (0);
        i++;
    }
    return (1);
}

// Main watchdog routine
void *watchdog_routine(void *arg)
{
    t_data *data = (t_data *)arg;
    long long current_time;
    long long time_since_meal;
    int most_starved;
    
    while (!data->simulation_over)
    {
        // Check for deaths (highest priority)
        if (check_deaths(data))
            break;
        
        // Check meal completion
        if (all_fed(data))
        {
            data->simulation_over = 1;
            break;
        }
        
        pthread_mutex_lock(&data->admission_mutex);
        
        // Check for potential starvation and promote if necessary
        current_time = get_time_ms();
        most_starved = find_most_starved(data);
        
        if (most_starved != -1)
        {
            time_since_meal = current_time - data->philosophers[most_starved].last_meal_time;
            
            // If philosopher is approaching starvation threshold
            if (time_since_meal > (data->time_to_die)) // 80% of time_to_die
            {
                // Promote to front of queue if not already waiting for permission
                if (!data->philosophers[most_starved].gate_flag)
                    promote_starving_philosopher(data, most_starved);
            }
        }
        
        pthread_mutex_unlock(&data->admission_mutex);
        
        // Watchdog check interval - must be frequent for death detection
        usleep(100); // 1ms
    }
    
    return (NULL);
}