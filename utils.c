/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrhilane <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/16 06:49:02 by hrhilane          #+#    #+#             */
/*   Updated: 2025/08/16 06:49:04 by hrhilane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
	if (!is_simulation_over(philo->data))
	{
		timestamp = get_time_ms() - philo->data->start_time;
		printf("%lld %d %s\n", timestamp, philo->id + 1, status);
	}
	pthread_mutex_unlock(&philo->data->print_mutex);
}

static int	ft_atoi(char *str)
{
	int	result;
	int	i;

	result = 0;
	i = 0;
	while (str[i] == ' ' || str[i] == '\t')
		i++;
	if (str[i] == '-')
		return (-1);
	if (str[i] == '+')
		i++;
	while (str[i] >= '0' && str[i] <= '9')
	{
		result = result * 10 + (str[i] - '0');
		i++;
	}
	if (str[i] != '\0')
		return (-1);
	return (result);
}

int	parse_arguments(int argc, char **argv, t_data *data)
{
	if (argc < 5 || argc > 6)
	{
		printf("Usage: %s philos t_die t_eat t_sleep [iters]\n" \
	"\tphilos : number of philosophers\n" \
	"\tt_die  : time to die in milliseconds\n" \
	"\tt_eat  : time to eat in milliseconds\n" \
	"\tt_sleep: time to sleep in milliseconds\n" \
	"\titers  : number of times each philosopher must eat (optional)\n"\
	, argv[0]);
		return (1);
	}
	data->num_philosophers = ft_atoi(argv[1]);
	data->time_to_die = ft_atoi(argv[2]);
	data->time_to_eat = ft_atoi(argv[3]);
	data->time_to_sleep = ft_atoi(argv[4]);
	if (argc == 6)
		data->meals_required = ft_atoi(argv[5]);
	else
		data->meals_required = -1;
	if (data->num_philosophers <= 0
		|| data->time_to_die < 0 || data->time_to_eat < 0
		|| data->time_to_sleep < 0 || (argc == 6 && data->meals_required <= 0))
		return (printf("Error: Invalid arguments\n"), 1);
	return (0);
}
