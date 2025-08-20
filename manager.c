/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   manager.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrhilane <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 06:47:50 by hrhilane          #+#    #+#             */
/*   Updated: 2025/08/16 06:47:52 by hrhilane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	allow_to_eat(t_data *data, int philosopher_id)
{
	pthread_mutex_lock(&data->philosophers[philosopher_id].state_mutex);
	data->philosophers[philosopher_id].gate_flag = 1;
	pthread_mutex_unlock(&data->philosophers[philosopher_id].state_mutex);
	reserve_forks(data, philosopher_id);
}

static void	collect_finished_forks(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_philosophers)
	{
		pthread_mutex_lock(&data->philosophers[i].state_mutex);
		if (data->philosophers[i].done_flag)
		{
			free_forks(data, i);
			data->philosophers[i].done_flag = 0;
		}
		pthread_mutex_unlock(&data->philosophers[i].state_mutex);
		i++;
	}
}

static void	remove_and_feed(t_data *data, int idx, int candidate_id)
{
	int	remove_idx;
	int	i;
	int	prev_idx;

	remove_idx = (data->admission_queue.head + idx)
		% data->admission_queue.capacity;
	i = idx;
	while (i > 0)
	{
		prev_idx = (remove_idx - 1 + data->admission_queue.capacity)
			% data->admission_queue.capacity;
		data->admission_queue.buffer[remove_idx]
			= data->admission_queue.buffer[prev_idx];
		remove_idx = prev_idx;
		i--;
	}
	data->admission_queue.head = (data->admission_queue.head + 1)
		% data->admission_queue.capacity;
	data->admission_queue.size--;
	allow_to_eat(data, candidate_id);
}

static void	feed_waiting_philosophers(t_data *data)
{
	int	q_size;
	int	idx;
	int	candidate_id;

	q_size = data->admission_queue.size;
	idx = 0;
	while (idx < q_size)
	{
		candidate_id = data->admission_queue.buffer[(data->admission_queue.head
				+ idx) % data->admission_queue.capacity];
		if (forks_available(data, candidate_id))
		{
			remove_and_feed(data, idx, candidate_id);
			q_size--;
			continue ;
		}
		idx++;
	}
}

void	*manager_routine(void *arg)
{
	t_data	*data;

	data = (t_data *)arg;
	while (!is_simulation_over(data))
	{
		pthread_mutex_lock(&data->admission_mutex);
		collect_finished_forks(data);
		feed_waiting_philosophers(data);
		pthread_mutex_unlock(&data->admission_mutex);
		usleep(100);
	}
	return (NULL);
}
