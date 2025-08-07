#include "philo.h"

void *philosopher_routine(void *arg)
{
	t_philosopher *philo = (t_philosopher *)arg;

	if (philo->shared->num_philosophers == 1)
	{
		print_status(philo, "has taken a fork");
		precise_sleep(philo->shared->time_to_die);
		return (NULL);
	}

	if (philo->id % 2 == 0)
		precise_sleep(philo->shared->time_to_eat / 2);

	while (is_simulation_running(philo->shared))
	{
		print_status(philo, "is thinking");

		pthread_mutex_lock(&philo->shared->forks[philo->first_fork]);
		if (!is_simulation_running(philo->shared))
			return (pthread_mutex_unlock(&philo->shared->forks[philo->first_fork]), NULL);
		print_status(philo, "has taken a fork");

		pthread_mutex_lock(&philo->shared->forks[philo->second_fork]);
		if (!is_simulation_running(philo->shared))
		{
			pthread_mutex_unlock(&philo->shared->forks[philo->second_fork]);
			pthread_mutex_unlock(&philo->shared->forks[philo->first_fork]);
			return (NULL);
		}
		print_status(philo, "has taken a fork");

		pthread_mutex_lock(&philo->shared->death_mutex);
		philo->last_meal_time = get_current_time_ms();
		pthread_mutex_unlock(&philo->shared->death_mutex);

		print_status(philo, "is eating");
		precise_sleep(philo->shared->time_to_eat);
		philo->meals_eaten++;

		if (philo->shared->must_eat_count != -1 &&
			philo->meals_eaten >= philo->shared->must_eat_count)
		{
			pthread_mutex_lock(&philo->shared->death_mutex);
			philo->shared->philosophers_finished++;
			if (philo->shared->philosophers_finished >= philo->shared->num_philosophers)
				philo->shared->simulation_running = false;
			pthread_mutex_unlock(&philo->shared->death_mutex);
		}

		pthread_mutex_unlock(&philo->shared->forks[philo->second_fork]);
		pthread_mutex_unlock(&philo->shared->forks[philo->first_fork]);

		if (!is_simulation_running(philo->shared))
			break ;

		print_status(philo, "is sleeping");
		precise_sleep(philo->shared->time_to_sleep);
	}
	return (NULL);
}
