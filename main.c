#include "philo.h"

t_philosopher *g_philosophers = NULL;

int main(int argc, char **argv)
{
	t_shared_data shared;
	pthread_t death_monitor_thread;
	int i;

	if (argc != 5 && argc != 6)
	{
		printf("Usage: %s number_of_philosophers time_to_die time_to_eat ", argv[0]);
		printf("time_to_sleep [number_of_times_each_philosopher_must_eat]\n");
		return (1);
	}

	if (init_shared_data(&shared, argc, argv) != 0)
		return (1);

	g_philosophers = malloc(sizeof(t_philosopher) * shared.num_philosophers);
	if (!g_philosophers)
		error_exit("Failed to allocate memory for philosophers");

	if (init_philosophers(g_philosophers, &shared) != 0)
		return (1);

	if (pthread_create(&death_monitor_thread, NULL, death_monitor, &shared) != 0)
		error_exit("Failed to create death monitor thread");

	i = -1;
	while (++i < shared.num_philosophers)
		if (pthread_create(&g_philosophers[i].thread, NULL, philosopher_routine, &g_philosophers[i]) != 0)
			error_exit("Failed to create philosopher thread");

	i = -1;
	while (++i < shared.num_philosophers)
		pthread_join(g_philosophers[i].thread, NULL);

	pthread_join(death_monitor_thread, NULL);
	cleanup_resources(&shared);
	free(g_philosophers);
	return (0);
}
