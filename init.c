#include "philo.h"

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
		printf("Usage: %s NB NB NB NB [NB]\n", argv[0]);
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
	if (data->num_philosophers <= 0 || data->num_philosophers > 200
		|| data->time_to_die < 0 || data->time_to_eat < 0
		|| data->time_to_sleep < 0 || (argc == 6 && data->meals_required <= 0))
	{
		printf("Error: Invalid arguments\n");
		return (1);
	}
	return (0);
}

int	init_data(t_data *data)
{
	int	i;

	data->simulation_over = 0;
	data->start_time = get_time_ms();
	data->philosophers = malloc(sizeof(t_philosopher) * data->num_philosophers);
	if (!data->philosophers)
		return (1);
	data->forks = malloc(sizeof(pthread_mutex_t) * data->num_philosophers);
	if (!data->forks)
	{
		free(data->philosophers);
		return (1);
	}
	data->fork_states = malloc(sizeof(t_fork_state) * data->num_philosophers);
	if (!data->fork_states)
	{
		free(data->philosophers);
		free(data->forks);
		return (1);
	}
	if (queue_init(&data->admission_queue, data->num_philosophers * 2) != 0)
	{
		free(data->philosophers);
		free(data->forks);
		free(data->fork_states);
		return (1);
	}
	if (pthread_mutex_init(&data->admission_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&data->print_mutex, NULL) != 0)
		return (1);
	i = 0;
	while (i < data->num_philosophers)
	{
		if (pthread_mutex_init(&data->forks[i], NULL) != 0)
			return (1);
		data->fork_states[i] = FORK_FREE;
		i++;
	}
	i = 0;
	while (i < data->num_philosophers)
	{
		data->philosophers[i].id = i;
		data->philosophers[i].last_meal_time = data->start_time;
		data->philosophers[i].request_time = 0;
		data->philosophers[i].meals_eaten = 0;
		data->philosophers[i].gate_flag = 0;
		data->philosophers[i].done_flag = 0;
		data->philosophers[i].data = data;
		i++;
	}
	return (0);
}

void	cleanup_data(t_data *data)
{
	int	i;

	if (data->forks)
	{
		i = 0;
		while (i < data->num_philosophers)
		{
			pthread_mutex_destroy(&data->forks[i]);
			i++;
		}
		free(data->forks);
	}
	if (data->fork_states)
		free(data->fork_states);
	pthread_mutex_destroy(&data->admission_mutex);
	pthread_mutex_destroy(&data->print_mutex);
	queue_destroy(&data->admission_queue);
	if (data->philosophers)
		free(data->philosophers);
}
