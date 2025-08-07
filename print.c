#include "philo.h"

void print_status(t_philosopher *philo, char *status)
{
	int timestamp;

	pthread_mutex_lock(&philo->shared->print_mutex);
	if (is_simulation_running(philo->shared))
	{
		timestamp = get_elapsed_time(philo->shared->simulation_start);
		printf("%d %d %s\n", timestamp, philo->id, status);
	}
	pthread_mutex_unlock(&philo->shared->print_mutex);
}

bool is_simulation_running(t_shared_data *shared)
{
	bool running;

	pthread_mutex_lock(&shared->death_mutex);
	running = shared->simulation_running;
	pthread_mutex_unlock(&shared->death_mutex);
	return (running);
}

void stop_simulation(t_shared_data *shared)
{
	pthread_mutex_lock(&shared->death_mutex);
	shared->simulation_running = false;
	pthread_mutex_unlock(&shared->death_mutex);
}
