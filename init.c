#include "philo.h"

int init_shared_data(t_shared_data *shared, int argc, char **argv)
{
	int i;

	if (parse_arguments(argc, argv, shared) != 0)
		return (1);

	shared->simulation_running = true;
	shared->philosophers_finished = 0;
	shared->simulation_start = get_current_time_ms();

	if (pthread_mutex_init(&shared->print_mutex, NULL) != 0)
		return (printf("Error: Failed to init print mutex\n"), 1);
	if (pthread_mutex_init(&shared->death_mutex, NULL) != 0)
		return (pthread_mutex_destroy(&shared->print_mutex), printf("Error: Failed to init death mutex\n"), 1);

	shared->forks = malloc(sizeof(pthread_mutex_t) * shared->num_philosophers);
	if (!shared->forks)
		return (pthread_mutex_destroy(&shared->print_mutex), pthread_mutex_destroy(&shared->death_mutex),
			printf("Error: Failed to allocate forks\n"), 1);

	i = -1;
	while (++i < shared->num_philosophers)
		if (pthread_mutex_init(&shared->forks[i], NULL) != 0)
		{
			while (--i >= 0)
				pthread_mutex_destroy(&shared->forks[i]);
			free(shared->forks);
			pthread_mutex_destroy(&shared->print_mutex);
			pthread_mutex_destroy(&shared->death_mutex);
			return (printf("Error: Failed to init fork %d\n", i), 1);
		}
	return (0);
}
