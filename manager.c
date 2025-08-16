#include "philo.h"

static void	grant_permission(t_data *data, int philosopher_id)
{
	data->philosophers[philosopher_id].gate_flag = 1;
	reserve_forks(data, philosopher_id);
}

static void	handle_completions(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_philosophers)
	{
		if (data->philosophers[i].done_flag)
		{
			free_forks(data, i);
			data->philosophers[i].done_flag = 0;
		}
		i++;
	}
}

static void	remove_from_queue_and_grant(t_data *data, int idx, int candidate_id)
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
	grant_permission(data, candidate_id);
}

static void	process_queue(t_data *data)
{
	int	qsize;
	int	idx;
	int	candidate_id;

	qsize = queue_size(&data->admission_queue);
	idx = 0;
	while (idx < qsize)
	{
		candidate_id = data->admission_queue.buffer[(data->admission_queue.head
				+ idx) % data->admission_queue.capacity];
		if (forks_available(data, candidate_id))
		{
			remove_from_queue_and_grant(data, idx, candidate_id);
			qsize--;
			continue ;
		}
		idx++;
	}
}

void	*manager_routine(void *arg)
{
	t_data	*data;

	data = (t_data *)arg;
	while (!data->simulation_over)
	{
		pthread_mutex_lock(&data->admission_mutex);
		handle_completions(data);
		process_queue(data);
		pthread_mutex_unlock(&data->admission_mutex);
		usleep(500);
	}
	return (NULL);
}
