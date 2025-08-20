/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrhilane <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 06:48:13 by hrhilane          #+#    #+#             */
/*   Updated: 2025/08/16 06:48:16 by hrhilane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	request_eating_permission(t_philosopher *philo)
{
	t_data	*data;

	data = philo->data;
	pthread_mutex_lock(&data->admission_mutex);
	if (!queue_contains(&data->admission_queue, philo->id))
	{
		philo->request_time = get_time_ms();
		queue_enqueue(&data->admission_queue, philo->id);
	}
	pthread_mutex_unlock(&data->admission_mutex);
}

static void	wait_for_permission(t_philosopher *philo)
{
	int	should_continue;

	should_continue = 1;
	while (should_continue && !is_simulation_over(philo->data))
	{
		pthread_mutex_lock(&philo->state_mutex);
		should_continue = !philo->gate_flag;
		pthread_mutex_unlock(&philo->state_mutex);
		if (should_continue)
			usleep(100);
	}
}

static void	request_and_wait(t_philosopher *philo)
{
	request_eating_permission(philo);
	wait_for_permission(philo);
}

static void	signal_completion(t_philosopher *philo)
{
	pthread_mutex_lock(&philo->data->admission_mutex);
	pthread_mutex_lock(&philo->state_mutex);
	philo->done_flag = 1;
	philo->gate_flag = 0;
	pthread_mutex_unlock(&philo->state_mutex);
	pthread_mutex_unlock(&philo->data->admission_mutex);
}

void	*philosopher_routine(void *arg)
{
	t_philosopher	*philo;
	t_data			*data;

	philo = (t_philosopher *)arg;
	data = philo->data;
	if (philo->id % 2 == 1)
		ft_sleep(data->time_to_eat / 2);
	while (!is_simulation_over(data))
	{
		if (data->meals_required > 0
			&& philo->meals_eaten >= data->meals_required)
			break ;
		thinking(philo);
		if (is_simulation_over(data))
			break ;
		request_and_wait(philo);
		if (is_simulation_over(data))
			break ;
		eating(philo);
		signal_completion(philo);
		if (is_simulation_over(data))
			break ;
		sleeping(philo);
	}
	return (NULL);
}
