/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrhilane <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 06:47:23 by hrhilane          #+#    #+#             */
/*   Updated: 2025/08/16 06:47:24 by hrhilane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	init_mutexes(t_data *data)
{
	if (pthread_mutex_init(&data->admission_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&data->print_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&data->simulation_mutex, NULL) != 0)
		return (1);
	return (0);
}

static int	init_forks_and_states(t_data *data)
{
	int	i;

	data->forks = malloc(sizeof(pthread_mutex_t) * data->num_philosophers);
	if (!data->forks)
		return (1);
	data->fork_states = malloc(sizeof(t_fork_state) * data->num_philosophers);
	if (!data->fork_states)
	{
		free(data->forks);
		return (1);
	}
	i = 0;
	while (i < data->num_philosophers)
	{
		if (pthread_mutex_init(&data->forks[i], NULL) != 0)
			return (1);
		data->fork_states[i] = FORK_FREE;
		i++;
	}
	return (0);
}

static int	init_philosophers(t_data *data)
{
	int	i;

	data->philosophers = malloc(sizeof(t_philosopher) * data->num_philosophers);
	if (!data->philosophers)
		return (1);
	i = 0;
	while (i < data->num_philosophers)
	{
		data->philosophers[i].id = i;
		data->philosophers[i].last_meal_time = data->start_time;
		data->philosophers[i].request_time = 0;
		data->philosophers[i].meals_eaten = 0;
		data->philosophers[i].gate_flag = 0;
		data->philosophers[i].done_flag = 0;
		data->philosophers[i].data = data;
		if (pthread_mutex_init(&data->philosophers[i].state_mutex, NULL) != 0)
			return (1);
		i++;
	}
	return (0);
}

int	init_data(t_data *data)
{
	data->simulation_over = 0;
	data->start_time = get_time_ms();
	if (init_philosophers(data) != 0)
		return (1);
	if (init_forks_and_states(data) != 0)
	{
		free(data->philosophers);
		return (1);
	}
	if (queue_init(&data->admission_queue, data->num_philosophers * 2) != 0)
	{
		free(data->philosophers);
		free(data->forks);
		free(data->fork_states);
		return (1);
	}
	if (init_mutexes(data) != 0)
		return (1);
	return (0);
}

void	cleanup_data(t_data *data)
{
	int	i;

	if (data->forks)
	{
		i = 0;
		while (i < data->num_philosophers)
		{
			pthread_mutex_destroy(&data->forks[i]);
			i++;
		}
		free(data->forks);
	}
	if (data->philosophers)
	{
		i = 0;
		while (i < data->num_philosophers)
		{
			pthread_mutex_destroy(&data->philosophers[i].state_mutex);
			i++;
		}
	}
	if (data->fork_states)
		free(data->fork_states);
	destroy_mutexes(data);
	if (data->philosophers)
		free(data->philosophers);
}
