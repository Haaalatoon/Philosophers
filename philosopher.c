#include "philo.h"

// Request permission from manager to eat
static void request_eating_permission(t_philosopher *philo)
{
    t_data *data = philo->data;

    pthread_mutex_lock(&data->admission_mutex);

    // Only enqueue if not already waiting
    if (!queue_contains(&data->admission_queue, philo->id))
    {
        philo->request_time = get_time_ms();
        queue_enqueue(&data->admission_queue, philo->id);
    }

    pthread_mutex_unlock(&data->admission_mutex);
}

// Wait for manager to grant permission via gate_flag
static void wait_for_permission(t_philosopher *philo)
{
    // Poll gate_flag with nanosleep backoff
    while (!philo->gate_flag && !philo->data->simulation_over)
    {
        usleep(100); // Short polling interval
    }
}

// Eating with resource ordering (deadlock prevention)
static void eat(t_philosopher *philo)
{
    t_data *data = philo->data;
    int left_fork = philo->id;
    int right_fork = (philo->id + 1) % data->num_philosophers;
    int first_fork, second_fork;
    
    // RESOURCE ORDERING: Always acquire lower-numbered fork first
    if (left_fork < right_fork)
    {
        first_fork = left_fork;
        second_fork = right_fork;
    }
    else
    {
        first_fork = right_fork;
        second_fork = left_fork;
    }
    
    // Special case: single philosopher
    if (data->num_philosophers == 1)
    {
        pthread_mutex_lock(&data->forks[first_fork]);
        print_status(philo, "has taken a fork");
        ft_sleep(data->time_to_die + 1);
        pthread_mutex_unlock(&data->forks[first_fork]);
        return;
    }
    
    // Acquire forks in order
    pthread_mutex_lock(&data->forks[first_fork]);
    print_status(philo, "has taken a fork");
    
    pthread_mutex_lock(&data->forks[second_fork]);
    print_status(philo, "has taken a fork");
    
    // Start eating
    print_status(philo, "is eating");
    philo->last_meal_time = get_time_ms();
    
    // Eat for specified duration
    ft_sleep(data->time_to_eat);
    philo->meals_eaten++;
    
    // Release forks in reverse order
    pthread_mutex_unlock(&data->forks[second_fork]);
    pthread_mutex_unlock(&data->forks[first_fork]);
}

// Signal completion to manager
static void signal_completion(t_philosopher *philo)
{
    pthread_mutex_lock(&philo->data->admission_mutex);
    philo->done_flag = 1;    // Signal manager we're done
    philo->gate_flag = 0;    // Reset permission flag
    pthread_mutex_unlock(&philo->data->admission_mutex);
}

// Sleep phase
static void sleep_phase(t_philosopher *philo)
{
    if (philo->data->simulation_over)
        print_status(philo, "is sleeping");
    ft_sleep(philo->data->time_to_sleep);
}

// Think phase  
static void think(t_philosopher *philo)
{
    print_status(philo, "is thinking");
    // Brief thinking time to prevent excessive CPU usage
    usleep(100); // 1 ms
}

// Main philosopher thread routine
void *philosopher_routine(void *arg)
{
    t_philosopher *philo = (t_philosopher *)arg;
    t_data *data = philo->data;
    
    // Stagger start times to reduce initial contention
    if (philo->id % 2 == 1)
        ft_sleep(data->time_to_eat / 2);
    
    // Main philosopher loop
    while (!data->simulation_over)
    {
        // Check meal completion
        if (data->meals_required > 0 && philo->meals_eaten >= data->meals_required)
            break;
        
        // Think first
        think(philo);
        
        if (data->simulation_over)
            break;
        
        // Request permission to eat from manager
        request_eating_permission(philo);
        
        // Wait for manager to grant permission
        wait_for_permission(philo);
        
        if (data->simulation_over)
            break;
        
        // Eat (manager has granted permission and reserved forks)
        eat(philo);
        
        // Signal completion to manager
        signal_completion(philo);
        
        if (data->simulation_over)
            break;
        
        // Sleep
        sleep_phase(philo);
    }
    
    return (NULL);
}