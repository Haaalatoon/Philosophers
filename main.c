#include "philo.h"

int	main(int argc, char **argv)
{
	int		i;
	t_data	data;

	memset(&data, 0, sizeof(t_data));
	if (parse_arguments(argc, argv, &data) != 0)
		return (1);
	if (init_data(&data) != 0)
	{
		printf("Error: Failed to initialize data\n");
		return (1);
	}
	if (pthread_create(&data.manager_thread, NULL, manager_routine, &data) != 0)
	{
		printf("Error: Failed to create manager thread\n");
		cleanup_data(&data);
		return (1);
	}
	if (pthread_create(&data.watchdog_thread, NULL, watchdog_routine,
			&data) != 0)
	{
		printf("Error: Failed to create watchdog thread\n");
		data.simulation_over = 1;
		pthread_join(data.manager_thread, NULL);
		cleanup_data(&data);
		return (1);
	}
	i = 0;
	while (i < data.num_philosophers)
	{
		if (pthread_create(&data.philosophers[i].thread, NULL,
				philosopher_routine, &data.philosophers[i]) != 0)
		{
			printf("Error: Failed to create philosopher thread %d\n", i + 1);
			data.simulation_over = 1;
			break ;
		}
		i++;
	}
	i = 0;
	while (i < data.num_philosophers)
	{
		pthread_join(data.philosophers[i].thread, NULL);
		i++;
	}
	data.simulation_over = 1;
	pthread_join(data.manager_thread, NULL);
	pthread_join(data.watchdog_thread, NULL);
	cleanup_data(&data);
	return (0);
}
