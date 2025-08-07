#include "philo.h"

void *death_monitor(void *arg)
{
	t_shared_data *shared = (t_shared_data *)arg;
	long current_time;
	long since_last;
	int i;

	while (is_simulation_running(shared))
	{
		current_time = get_current_time_ms();
		i = -1;
		while (++i < shared->num_philosophers)
		{
			pthread_mutex_lock(&shared->death_mutex);
			since_last = current_time - g_philosophers[i].last_meal_time;
			if (since_last > shared->time_to_die)
			{
				shared->simulation_running = false;
				pthread_mutex_unlock(&shared->death_mutex);
				pthread_mutex_lock(&shared->print_mutex);
				printf("%d %d died\n", get_elapsed_time(shared->simulation_start), g_philosophers[i].id);
				pthread_mutex_unlock(&shared->print_mutex);
				return (NULL);
			}
			pthread_mutex_unlock(&shared->death_mutex);
		}
		usleep(1000);
	}
	return (NULL);
}
