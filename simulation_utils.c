#include "philo.h"

int	is_simulation_over(t_data *data)
{
	int	result;

	pthread_mutex_lock(&data->simulation_mutex);
	result = data->simulation_over;
	pthread_mutex_unlock(&data->simulation_mutex);
	return (result);
}

void	set_simulation_over(t_data *data)
{
	pthread_mutex_lock(&data->simulation_mutex);
	data->simulation_over = 1;
	pthread_mutex_unlock(&data->simulation_mutex);
}

void	destroy_mutexes(t_data *data)
{
	pthread_mutex_destroy(&data->admission_mutex);
	pthread_mutex_destroy(&data->print_mutex);
	pthread_mutex_destroy(&data->simulation_mutex);
	queue_destroy(&data->admission_queue);
}
