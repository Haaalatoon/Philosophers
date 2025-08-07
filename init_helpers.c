#include "philo.h"

int init_philosophers(t_philosopher *philos, t_shared_data *shared)
{
	int i = -1;
	int left, right;

	while (++i < shared->num_philosophers)
	{
		philos[i].id = i + 1;
		philos[i].shared = shared;
		philos[i].meals_eaten = 0;
		philos[i].last_meal_time = shared->simulation_start;

		left = i;
		right = (i + 1) % shared->num_philosophers;

		if (left < right)
		{
			philos[i].first_fork = left;
			philos[i].second_fork = right;
		}
		else
		{
			philos[i].first_fork = right;
			philos[i].second_fork = left;
		}
	}
	return (0);
}

int parse_arguments(int argc, char **argv, t_shared_data *shared)
{
	int i = 1;

	while (i < argc)
		if (!is_valid_number(argv[i++]))
			return (printf("Error: All arguments must be positive integers\n"), 1);

	shared->num_philosophers = atoi(argv[1]);
	shared->time_to_die = atoi(argv[2]);
	shared->time_to_eat = atoi(argv[3]);
	shared->time_to_sleep = atoi(argv[4]);
	shared->must_eat_count = (argc == 6) ? atoi(argv[5]) : -1;

	if (shared->num_philosophers <= 0 || shared->num_philosophers > 200)
		return (printf("Error: Number of philosophers must be 1–200\n"), 1);
	if (shared->time_to_die <= 0 || shared->time_to_eat <= 0 || shared->time_to_sleep <= 0)
		return (printf("Error: All time values must be positive\n"), 1);
	if (argc == 6 && shared->must_eat_count <= 0)
		return (printf("Error: Required meals must be positive\n"), 1);
	return (0);
}
