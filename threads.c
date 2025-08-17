/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrhilane <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 06:48:53 by hrhilane          #+#    #+#             */
/*   Updated: 2025/08/16 06:48:56 by hrhilane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	start_threads(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_philosophers)
	{
		if (pthread_create(&data->philosophers[i].thread, NULL,
				philosopher_routine, &data->philosophers[i]) != 0)
		{
			printf("Error: Failed to create philosopher thread %d\n", i + 1);
			data->simulation_over = 1;
			return (1);
		}
		i++;
	}
	return (0);
}

void	join_threads(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_philosophers)
	{
		pthread_join(data->philosophers[i].thread, NULL);
		i++;
	}
	pthread_join(data->manager_thread, NULL);
	pthread_join(data->watchdog_thread, NULL);
}

void	join_and_cleanup(t_data *data)
{
	join_threads(data);
	cleanup_data(data);
}

int	create_manager_thread(t_data *data)
{
	if (pthread_create(&data->manager_thread, NULL, manager_routine, data) != 0)
	{
		printf("Error: Failed to create manager thread\n");
		cleanup_data(data);
		return (1);
	}
	return (0);
}

int	create_watchdog_thread(t_data *data)
{
	if (pthread_create(&data->watchdog_thread, NULL, watchdog_routine,
			data) != 0)
	{
		printf("Error: Failed to create watchdog thread\n");
		data->simulation_over = 1;
		pthread_join(data->manager_thread, NULL);
		cleanup_data(data);
		return (1);
	}
	return (0);
}
