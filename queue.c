/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrhilane <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 06:48:23 by hrhilane          #+#    #+#             */
/*   Updated: 2025/08/16 06:48:26 by hrhilane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	queue_init(t_queue *queue, int capacity)
{
	queue->buffer = malloc(sizeof(int) * capacity);
	if (!queue->buffer)
		return (1);
	queue->head = 0;
	queue->tail = 0;
	queue->size = 0;
	queue->capacity = capacity;
	return (0);
}

void	queue_destroy(t_queue *queue)
{
	if (queue->buffer)
	{
		free(queue->buffer);
		queue->buffer = NULL;
	}
}

int	queue_enqueue(t_queue *queue, int philosopher_id)
{
	if (queue->size >= queue->capacity)
		return (1);
	queue->buffer[queue->tail] = philosopher_id;
	queue->tail = (queue->tail + 1) % queue->capacity;
	queue->size++;
	return (0);
}

int	queue_dequeue(t_queue *queue)
{
	int	philosopher_id;

	if (queue->size <= 0)
		return (-1);
	philosopher_id = queue->buffer[queue->head];
	queue->head = (queue->head + 1) % queue->capacity;
	queue->size--;
	return (philosopher_id);
}
