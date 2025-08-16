#include "philo.h"

void	eat(t_philosopher *philo)
{
	t_data	*data;
	int		left_fork;
	int		right_fork;
	int		first_fork;
	int		second_fork;

	data = philo->data;
	left_fork = philo->id;
	right_fork = (philo->id + 1) % data->num_philosophers;
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
		pthread_mutex_lock(&data->forks[first_fork]);
		print_status(philo, "has taken a fork");
		ft_sleep(data->time_to_die + 1);
		pthread_mutex_unlock(&data->forks[first_fork]);
		return ;
	}
	pthread_mutex_lock(&data->forks[first_fork]);
	print_status(philo, "has taken a fork");
	pthread_mutex_lock(&data->forks[second_fork]);
	print_status(philo, "has taken a fork");
	print_status(philo, "is eating");
	philo->last_meal_time = get_time_ms();
	ft_sleep(data->time_to_eat);
	philo->meals_eaten++;
	pthread_mutex_unlock(&data->forks[second_fork]);
	pthread_mutex_unlock(&data->forks[first_fork]);
}

void	sleep_phase(t_philosopher *philo)
{
	if (philo->data->simulation_over)
		print_status(philo, "is sleeping");
	ft_sleep(philo->data->time_to_sleep);
}

void	think(t_philosopher *philo)
{
	print_status(philo, "is thinking");
	usleep(100);
}
