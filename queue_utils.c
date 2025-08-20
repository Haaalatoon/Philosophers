/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   another_queue.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrhilane <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 06:47:07 by hrhilane          #+#    #+#             */
/*   Updated: 2025/08/16 06:47:10 by hrhilane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	queue_peek(t_queue *queue)
{
	if (queue->size <= 0)
		return (-1);
	return (queue->buffer[queue->head]);
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
