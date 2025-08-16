#include "philo.h"

long long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (((long long)tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

void	ft_sleep(long ms)
{
	long long	start_time;
	long long	target_time;

	start_time = get_time_ms();
	target_time = start_time + ms;
	if (ms <= 2)
	{
		while (get_time_ms() < target_time)
			usleep(50);
	}
	else
	{
		usleep((ms - 1) * 1000);
		while (get_time_ms() < target_time)
			usleep(50);
	}
}

void	print_status(t_philosopher *philo, char *status)
{
	long long	timestamp;

	pthread_mutex_lock(&philo->data->print_mutex);
	if (!philo->data->simulation_over)
	{
		timestamp = get_time_ms() - philo->data->start_time;
		printf("%lld %d %s\n", timestamp, philo->id + 1, status);
	}
	pthread_mutex_unlock(&philo->data->print_mutex);
}
