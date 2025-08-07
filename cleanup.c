#include "philo.h"

void cleanup_resources(t_shared_data *shared)
{
	int i = -1;

	while (++i < shared->num_philosophers)
		pthread_mutex_destroy(&shared->forks[i]);
	free(shared->forks);
	pthread_mutex_destroy(&shared->print_mutex);
	pthread_mutex_destroy(&shared->death_mutex);
}
