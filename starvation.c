/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   starvation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrhilane <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 06:48:46 by hrhilane          #+#    #+#             */
/*   Updated: 2025/08/16 06:48:48 by hrhilane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	find_most_starved(t_data *data)
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
		pthread_mutex_lock(&data->philosophers[i].state_mutex);
		wait_time = current_time - data->philosophers[i].last_meal_time;
		pthread_mutex_unlock(&data->philosophers[i].state_mutex);
		if (wait_time > max_wait_time)
		{
			max_wait_time = wait_time;
			most_starved = i;
		}
		i++;
	}
	return (most_starved);
}

void	promote_starving_philosopher(t_data *data, int philosopher_id)
{
	t_queue	temp_queue;
	int		current_id;

	if (queue_init(&temp_queue, data->num_philosophers * 2) != 0)
		return ;
	queue_enqueue(&temp_queue, philosopher_id);
	while (!data->admission_queue.size)
	{
		current_id = queue_dequeue(&data->admission_queue);
		if (current_id != philosopher_id)
			queue_enqueue(&temp_queue, current_id);
	}
	while (!temp_queue.size)
	{
		current_id = queue_dequeue(&temp_queue);
		queue_enqueue(&data->admission_queue, current_id);
	}
	queue_destroy(&temp_queue);
}

void	handle_starvation(t_data *data)
{
	long long	current_time;
	long long	time_since_meal;
	int			most_starved;

	pthread_mutex_lock(&data->admission_mutex);
	current_time = get_time_ms();
	most_starved = find_most_starved(data);
	if (most_starved != -1)
	{
		pthread_mutex_lock(&data->philosophers[most_starved].state_mutex);
		time_since_meal = current_time
			- data->philosophers[most_starved].last_meal_time;
		pthread_mutex_unlock(&data->philosophers[most_starved].state_mutex);
		if (time_since_meal > (data->time_to_die))
		{
			if (!data->philosophers[most_starved].gate_flag)
				promote_starving_philosopher(data, most_starved);
		}
	}
	pthread_mutex_unlock(&data->admission_mutex);
}
