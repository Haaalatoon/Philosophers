#include "philo.h"

// Get current time in milliseconds
long long get_time_ms(void)
{
    struct timeval tv;
    
    gettimeofday(&tv, NULL);
    return ((long long)tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

// Precise sleep function with nanosleep backoff
void ft_sleep(long ms)
{
    long long start_time;
    long long target_time;
    
    start_time = get_time_ms();
    target_time = start_time + ms;
    
    // For very small sleep times, busy wait is more accurate
    if (ms <= 2)
    {
        while (get_time_ms() < target_time)
            usleep(50);
    }
    else
    {
        // Sleep most of the time, then busy wait for precision
        usleep((ms - 1) * 1000);
        while (get_time_ms() < target_time)
            usleep(50);
    }
}

// Thread-safe printing with proper formatting
void print_status(t_philosopher *philo, char *status)
{
    long long timestamp;
    
    pthread_mutex_lock(&philo->data->print_mutex);
    
    // Only print if simulation is still running
    if (!philo->data->simulation_over)
    {
        timestamp = get_time_ms() - philo->data->start_time;
        printf("%lld %d %s\n", timestamp, philo->id + 1, status);
    }
    
    pthread_mutex_unlock(&philo->data->print_mutex);
}