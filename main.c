#include "philo.h"

int	main(int argc, char **argv)
{
	t_data	data;

	memset(&data, 0, sizeof(t_data));
	if (parse_arguments(argc, argv, &data) != 0)
		return (1);
	if (init_data(&data) != 0)
	{
		printf("Error: Failed to initialize data\n");
		return (1);
	}
	if (create_manager_thread(&data) != 0)
		return (1);
	if (create_watchdog_thread(&data) != 0)
		return (1);
	if (start_threads(&data) != 0)
	{
		join_and_cleanup(&data);
		return (1);
	}
	join_and_cleanup(&data);
	return (0);
}
