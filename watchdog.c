/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   watchdog.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrhilane <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 06:49:13 by hrhilane          #+#    #+#             */
/*   Updated: 2025/08/16 06:49:15 by hrhilane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
		pthread_mutex_lock(&data->philosophers[i].state_mutex);
		time_since_meal = current_time - data->philosophers[i].last_meal_time;
		pthread_mutex_unlock(&data->philosophers[i].state_mutex);
		if (time_since_meal >= data->time_to_die)
		{
			print_status(&data->philosophers[i], "died");
			set_simulation_over(data);
			return (1);
		}
		i++;
	}
	return (0);
}

static int	all_fed(t_data *data)
{
	int	i;
	int	meals_eaten;

	if (data->meals_required <= 0)
		return (0);
	i = 0;
	while (i < data->num_philosophers)
	{
		pthread_mutex_lock(&data->philosophers[i].state_mutex);
		meals_eaten = data->philosophers[i].meals_eaten;
		pthread_mutex_unlock(&data->philosophers[i].state_mutex);
		if (meals_eaten < data->meals_required)
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
		set_simulation_over(data);
		*should_break = 1;
	}
}

void	*watchdog_routine(void *arg)
{
	int		should_break;
	t_data	*data;

	data = (t_data *)arg;
	while (!is_simulation_over(data))
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
