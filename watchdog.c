#include "philo.h"

static int	find_most_starved(t_data *data)
{
	long long	current_time;
	long long	max_wait_time;
	long long	wait_time;
	int			most_starved;
	int			i;

	current_time = get_time_ms();
	max_wait_time = 0;
	most_starved = -1;
	i = 0;
	while (i < data->num_philosophers)
	{
		wait_time = current_time - data->philosophers[i].last_meal_time;
		if (wait_time > max_wait_time)
		{
			max_wait_time = wait_time;
			most_starved = i;
		}
		i++;
	}
	return (most_starved);
}

static void	promote_starving_philosopher(t_data *data, int philosopher_id)
{
	t_queue	temp_queue;
	int		current_id;

	if (queue_init(&temp_queue, data->num_philosophers * 2) != 0)
		return ;
	queue_enqueue(&temp_queue, philosopher_id);
	while (!queue_is_empty(&data->admission_queue))
	{
		current_id = queue_dequeue(&data->admission_queue);
		if (current_id != philosopher_id)
			queue_enqueue(&temp_queue, current_id);
	}
	while (!queue_is_empty(&temp_queue))
	{
		current_id = queue_dequeue(&temp_queue);
		queue_enqueue(&data->admission_queue, current_id);
	}
	queue_destroy(&temp_queue);
}

static int	check_deaths(t_data *data)
{
	long long	current_time;
	long long	time_since_meal;
	int			i;

	current_time = get_time_ms();
	i = 0;
	while (i < data->num_philosophers)
	{
		time_since_meal = current_time - data->philosophers[i].last_meal_time;
		if (time_since_meal >= data->time_to_die)
		{
			print_status(&data->philosophers[i], "died");
			data->simulation_over = 1;
			return (1);
		}
		i++;
	}
	return (0);
}

static int	all_fed(t_data *data)
{
	int	i;

	if (data->meals_required <= 0)
		return (0);
	i = 0;
	while (i < data->num_philosophers)
	{
		if (data->philosophers[i].meals_eaten < data->meals_required)
			return (0);
		i++;
	}
	return (1);
}

void	*watchdog_routine(void *arg)
{
	t_data		*data;
	long long	current_time;
	long long	time_since_meal;
	int			most_starved;

	data = (t_data *)arg;
	while (!data->simulation_over)
	{
		if (check_deaths(data))
			break ;
		if (all_fed(data))
		{
			data->simulation_over = 1;
			break ;
		}
		pthread_mutex_lock(&data->admission_mutex);
		current_time = get_time_ms();
		most_starved = find_most_starved(data);
		if (most_starved != -1)
		{
			time_since_meal = current_time
				- data->philosophers[most_starved].last_meal_time;
			if (time_since_meal > (data->time_to_die))
			{
				if (!data->philosophers[most_starved].gate_flag)
					promote_starving_philosopher(data, most_starved);
			}
		}
		pthread_mutex_unlock(&data->admission_mutex);
		usleep(100);
	}
	return (NULL);
}
