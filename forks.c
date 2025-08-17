/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   forks.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrhilane <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 06:47:17 by hrhilane          #+#    #+#             */
/*   Updated: 2025/08/16 06:47:18 by hrhilane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	forks_available(t_data *data, int philosopher_id)
{
	int	left_fork;
	int	right_fork;

	left_fork = philosopher_id;
	right_fork = (philosopher_id + 1) % data->num_philosophers;
	return (data->fork_states[left_fork] == FORK_FREE
		&& data->fork_states[right_fork] == FORK_FREE);
}

void	reserve_forks(t_data *data, int philosopher_id)
{
	int	left_fork;
	int	right_fork;

	left_fork = philosopher_id;
	right_fork = (philosopher_id + 1) % data->num_philosophers;
	data->fork_states[left_fork] = FORK_RESERVED;
	data->fork_states[right_fork] = FORK_RESERVED;
}

void	free_forks(t_data *data, int philosopher_id)
{
	int	left_fork;
	int	right_fork;

	left_fork = philosopher_id;
	right_fork = (philosopher_id + 1) % data->num_philosophers;
	data->fork_states[left_fork] = FORK_FREE;
	data->fork_states[right_fork] = FORK_FREE;
}
