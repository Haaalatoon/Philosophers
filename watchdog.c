#include "philo.h"

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

static void	check_and_handle_deaths(t_data *data, int *should_break)
{
	if (check_deaths(data))
		*should_break = 1;
}

static void	check_and_handle_all_fed(t_data *data, int *should_break)
{
	if (all_fed(data))
	{
		data->simulation_over = 1;
		*should_break = 1;
	}
}

void	*watchdog_routine(void *arg)
{
	t_data	*data;
	int		should_break;

	data = (t_data *)arg;
	while (!data->simulation_over)
	{
		should_break = 0;
		check_and_handle_deaths(data, &should_break);
		if (should_break)
			break ;
		check_and_handle_all_fed(data, &should_break);
		if (should_break)
			break ;
		handle_starvation(data);
		usleep(100);
	}
	return (NULL);
}
