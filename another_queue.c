#include "philo.h"

int	queue_peek(t_queue *queue)
{
	if (queue->size <= 0)
		return (-1);
	return (queue->buffer[queue->head]);
}

int	queue_is_empty(t_queue *queue)
{
	return (queue->size == 0);
}

int	queue_size(t_queue *queue)
{
	return (queue->size);
}

int	queue_contains(t_queue *queue, int philosopher_id)
{
	int	i;
	int	idx;

	i = 0;
	while (i < queue->size)
	{
		idx = (queue->head + i) % queue->capacity;
		if (queue->buffer[idx] == philosopher_id)
			return (1);
		i++;
	}
	return (0);
}
