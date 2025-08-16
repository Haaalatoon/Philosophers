#include "philo.h"

static void	request_eating_permission(t_philosopher *philo)
{
	t_data	*data;

	data = philo->data;
	pthread_mutex_lock(&data->admission_mutex);
	if (!queue_contains(&data->admission_queue, philo->id))
	{
		philo->request_time = get_time_ms();
		queue_enqueue(&data->admission_queue, philo->id);
	}
	pthread_mutex_unlock(&data->admission_mutex);
}

static void	wait_for_permission(t_philosopher *philo)
{
	while (!philo->gate_flag && !philo->data->simulation_over)
		usleep(100);
}

static void	request_and_wait(t_philosopher *philo)
{
	request_eating_permission(philo);
	wait_for_permission(philo);
}

static void	signal_completion(t_philosopher *philo)
{
	pthread_mutex_lock(&philo->data->admission_mutex);
	philo->done_flag = 1;
	philo->gate_flag = 0;
	pthread_mutex_unlock(&philo->data->admission_mutex);
}

void	*philosopher_routine(void *arg)
{
	t_philosopher	*philo;
	t_data			*data;

	philo = (t_philosopher *)arg;
	data = philo->data;
	if (philo->id % 2 == 1)
		ft_sleep(data->time_to_eat / 2);
	while (!data->simulation_over)
	{
		if (data->meals_required > 0
			&& philo->meals_eaten >= data->meals_required)
			break ;
		think(philo);
		if (data->simulation_over)
			break ;
		request_and_wait(philo);
		if (data->simulation_over)
			break ;
		eat(philo);
		signal_completion(philo);
		if (data->simulation_over)
			break ;
		sleep_phase(philo);
	}
	return (NULL);
}
