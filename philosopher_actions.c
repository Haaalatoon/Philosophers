/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrhilane <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 06:50:31 by hrhilane          #+#    #+#             */
/*   Updated: 2025/08/16 06:50:35 by hrhilane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	take_release(t_philosopher *philo, int first_fork, int second_fork)
{
	t_data	*data;

	data = philo->data;
	pthread_mutex_lock(&data->forks[first_fork]);
	print_status(philo, "has taken a fork");
	pthread_mutex_lock(&data->forks[second_fork]);
	print_status(philo, "has taken a fork");
	pthread_mutex_lock(&philo->state_mutex);
	philo->last_meal_time = get_time_ms();
	print_status(philo, "is eating");
	philo->meals_eaten++;
	pthread_mutex_unlock(&philo->state_mutex);
	ft_sleep(data->time_to_eat);
	pthread_mutex_unlock(&data->forks[second_fork]);
	pthread_mutex_unlock(&data->forks[first_fork]);
}

static void	single_philosopher(t_philosopher *philo, int fork)
{
	t_data	*data;

	data = philo->data;
	pthread_mutex_lock(&data->forks[fork]);
	print_status(philo, "has taken a fork");
	ft_sleep(data->time_to_die);
	pthread_mutex_unlock(&data->forks[fork]);
}

void	eating(t_philosopher *philo)
{
	t_data	*data;
	int		left_fork;
	int		right_fork;
	int		first_fork;
	int		second_fork;

	data = philo->data;
	left_fork = philo->id;
	right_fork = (philo->id + 1) % data->num_philosophers;
	if (left_fork < right_fork)
	{
		first_fork = left_fork;
		second_fork = right_fork;
	}
	else
	{
		first_fork = right_fork;
		second_fork = left_fork;
	}
	if (data->num_philosophers == 1)
	{
		single_philosopher(philo, first_fork);
		return ;
	}
	take_release(philo, first_fork, second_fork);
}

void	sleeping(t_philosopher *philo)
{
	if (philo->meals_eaten < philo->data->meals_required
		|| philo->data->meals_required <= 0)
		print_status(philo, "is sleeping");
	ft_sleep(philo->data->time_to_sleep);
}

void	thinking(t_philosopher *philo)
{
	print_status(philo, "is thinking");
	usleep(100);
}
