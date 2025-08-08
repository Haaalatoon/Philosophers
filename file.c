#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdbool.h>

/* Shared data structure */
typedef struct s_shared_data
{
	pthread_mutex_t	*forks;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	death_mutex;
	long			simulation_start;
	bool			simulation_running;
	int				philosophers_finished;
	int				num_philosophers;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				must_eat_count;
}	t_shared_data;

/* Philosopher structure */
typedef struct s_philosopher
{
	int				id;
	int				first_fork;
	int				second_fork;
	long			last_meal_time;
	int				meals_eaten;
	pthread_t		thread;
	t_shared_data	*shared;
}	t_philosopher;

/* Structure to pass data to death monitor */
typedef struct s_monitor_data
{
	t_shared_data	*shared;
	t_philosopher	*philosophers;
}	t_monitor_data;

/* Function prototypes */
long	get_current_time_ms(void);
int		get_elapsed_time(long start_time);
void	precise_sleep(int duration_ms);
void	print_status(t_philosopher *philo, char *status);
bool	is_simulation_running(t_shared_data *shared);
void	stop_simulation(t_shared_data *shared);
int		is_valid_number(char *str);
void	error_exit(char *message);
void	*philosopher_routine(void *arg);
void	*death_monitor(void *arg);

/* Utility functions */
long	get_current_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

int	get_elapsed_time(long start_time)
{
	return ((int)(get_current_time_ms() - start_time));
}

void	precise_sleep(int duration_ms)
{
	long	start_time;
	long	current_time;

	start_time = get_current_time_ms();
	
	if (duration_ms > 10)
		usleep((duration_ms - 10) * 1000);
	
	current_time = get_current_time_ms();
	while ((current_time - start_time) < duration_ms)
	{
		usleep(100);
		current_time = get_current_time_ms();
	}
}

void	print_status(t_philosopher *philo, char *status)
{
	int	timestamp;

	pthread_mutex_lock(&philo->shared->print_mutex);
	if (is_simulation_running(philo->shared))
	{
		timestamp = get_elapsed_time(philo->shared->simulation_start);
		printf("%d %d %s\n", timestamp, philo->id, status);
	}
	pthread_mutex_unlock(&philo->shared->print_mutex);
}

bool	is_simulation_running(t_shared_data *shared)
{
	bool	running;

	pthread_mutex_lock(&shared->death_mutex);
	running = shared->simulation_running;
	pthread_mutex_unlock(&shared->death_mutex);
	return (running);
}

void	stop_simulation(t_shared_data *shared)
{
	pthread_mutex_lock(&shared->death_mutex);
	shared->simulation_running = false;
	pthread_mutex_unlock(&shared->death_mutex);
}

int	is_valid_number(char *str)
{
	int	i;

	if (!str || !*str)
		return (0);
	i = 0;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

void	error_exit(char *message)
{
	printf("Error: %s\n", message);
	exit(1);
}

/* Fixed death monitor - receives philosophers array via parameter */
void	*death_monitor(void *arg)
{
	t_monitor_data	*monitor_data;
	t_shared_data	*shared;
	t_philosopher	*philosophers;
	int				i;
	long			current_time;
	long			time_since_last_meal;

	monitor_data = (t_monitor_data *)arg;
	shared = monitor_data->shared;
	philosophers = monitor_data->philosophers;

	while (is_simulation_running(shared))
	{
		current_time = get_current_time_ms();
		
		i = 0;
		while (i < shared->num_philosophers)
		{
			pthread_mutex_lock(&shared->death_mutex);
			
			time_since_last_meal = current_time - philosophers[i].last_meal_time;
			
			if (time_since_last_meal > shared->time_to_die)
			{
				shared->simulation_running = false;
				pthread_mutex_unlock(&shared->death_mutex);
				
				pthread_mutex_lock(&shared->print_mutex);
				printf("%d %d died\n", 
					get_elapsed_time(shared->simulation_start), 
					philosophers[i].id);
				pthread_mutex_unlock(&shared->print_mutex);
				
				return (NULL);
			}
			
			pthread_mutex_unlock(&shared->death_mutex);
			i++;
		}

		usleep(1000);
	}

	return (NULL);
}

/* Main philosopher routine */
void	*philosopher_routine(void *arg)
{
	t_philosopher	*philo;
	
	philo = (t_philosopher *)arg;

	/* Single philosopher case */
	if (philo->shared->num_philosophers == 1)
	{
		print_status(philo, "has taken a fork");
		precise_sleep(philo->shared->time_to_die);
		return (NULL);
	}

	/* Stagger even philosophers to prevent initial rush */
	if (philo->id % 2 == 0)
		precise_sleep(philo->shared->time_to_eat / 2);

	while (is_simulation_running(philo->shared))
	{
		/* THINKING */
		print_status(philo, "is thinking");

		/* EATING - Resource Hierarchy Applied */
		
		/* Acquire first fork (lower-numbered) */
		pthread_mutex_lock(&philo->shared->forks[philo->first_fork]);
		if (!is_simulation_running(philo->shared))
		{
			pthread_mutex_unlock(&philo->shared->forks[philo->first_fork]);
			break;
		}
		print_status(philo, "has taken a fork");

		/* Acquire second fork (higher-numbered) */
		pthread_mutex_lock(&philo->shared->forks[philo->second_fork]);
		if (!is_simulation_running(philo->shared))
		{
			pthread_mutex_unlock(&philo->shared->forks[philo->second_fork]);
			pthread_mutex_unlock(&philo->shared->forks[philo->first_fork]);
			break;
		}
		print_status(philo, "has taken a fork");

		/* Update last meal time - CRITICAL for death detection */
		pthread_mutex_lock(&philo->shared->death_mutex);
		philo->last_meal_time = get_current_time_ms();
		pthread_mutex_unlock(&philo->shared->death_mutex);
		
		print_status(philo, "is eating");
		precise_sleep(philo->shared->time_to_eat);
		
		philo->meals_eaten++;

		/* Check meal completion */
		if (philo->shared->must_eat_count != -1 && 
			philo->meals_eaten >= philo->shared->must_eat_count)
		{
			pthread_mutex_lock(&philo->shared->death_mutex);
			philo->shared->philosophers_finished++;
			if (philo->shared->philosophers_finished >= philo->shared->num_philosophers)
			{
				philo->shared->simulation_running = false;
			}
			pthread_mutex_unlock(&philo->shared->death_mutex);
		}

		/* Release forks */
		pthread_mutex_unlock(&philo->shared->forks[philo->second_fork]);
		pthread_mutex_unlock(&philo->shared->forks[philo->first_fork]);

		if (!is_simulation_running(philo->shared))
			break;

		/* SLEEPING */
		print_status(philo, "is sleeping");
		precise_sleep(philo->shared->time_to_sleep);
	}

	return (NULL);
}

/* Parse arguments */
int	parse_arguments(int argc, char **argv, t_shared_data *shared)
{
	int	i;

	i = 1;
	while (i < argc)
	{
		if (!is_valid_number(argv[i]))
		{
			printf("Error: All arguments must be positive integers\n");
			return (1);
		}
		i++;
	}

	shared->num_philosophers = atoi(argv[1]);
	shared->time_to_die = atoi(argv[2]);
	shared->time_to_eat = atoi(argv[3]);
	shared->time_to_sleep = atoi(argv[4]);
	
	if (argc == 6)
		shared->must_eat_count = atoi(argv[5]);
	else
		shared->must_eat_count = -1;

	if (shared->num_philosophers <= 0 || shared->num_philosophers > 200)
	{
		printf("Error: Number of philosophers must be between 1 and 200\n");
		return (1);
	}
	if (shared->time_to_die <= 0 || shared->time_to_eat <= 0 || shared->time_to_sleep <= 0)
	{
		printf("Error: All time values must be positive\n");
		return (1);
	}
	if (argc == 6 && shared->must_eat_count <= 0)
	{
		printf("Error: Number of required meals must be positive\n");
		return (1);
	}

	return (0);
}

/* Initialize shared data */
int	init_shared_data(t_shared_data *shared, int argc, char **argv)
{
	int	i;

	if (parse_arguments(argc, argv, shared) != 0)
		return (1);

	shared->simulation_running = true;
	shared->philosophers_finished = 0;
	shared->simulation_start = get_current_time_ms();

	if (pthread_mutex_init(&shared->print_mutex, NULL) != 0)
	{
		printf("Error: Failed to initialize print mutex\n");
		return (1);
	}
	if (pthread_mutex_init(&shared->death_mutex, NULL) != 0)
	{
		printf("Error: Failed to initialize death mutex\n");
		pthread_mutex_destroy(&shared->print_mutex);
		return (1);
	}

	shared->forks = malloc(sizeof(pthread_mutex_t) * shared->num_philosophers);
	if (!shared->forks)
	{
		printf("Error: Failed to allocate memory for forks\n");
		pthread_mutex_destroy(&shared->print_mutex);
		pthread_mutex_destroy(&shared->death_mutex);
		return (1);
	}

	i = 0;
	while (i < shared->num_philosophers)
	{
		if (pthread_mutex_init(&shared->forks[i], NULL) != 0)
		{
			printf("Error: Failed to initialize fork mutex %d\n", i);
			while (--i >= 0)
				pthread_mutex_destroy(&shared->forks[i]);
			free(shared->forks);
			pthread_mutex_destroy(&shared->print_mutex);
			pthread_mutex_destroy(&shared->death_mutex);
			return (1);
		}
		i++;
	}

	return (0);
}

/* Initialize philosophers with resource hierarchy */
int	init_philosophers(t_philosopher *philos, t_shared_data *shared)
{
	int	i;
	int	left_fork;
	int	right_fork;

	i = 0;
	while (i < shared->num_philosophers)
	{
		philos[i].id = i + 1;
		philos[i].shared = shared;
		philos[i].meals_eaten = 0;
		philos[i].last_meal_time = shared->simulation_start;

		left_fork = i;
		right_fork = (i + 1) % shared->num_philosophers;

		/* Resource hierarchy: always acquire lower-numbered fork first */
		if (left_fork < right_fork)
		{
			philos[i].first_fork = left_fork;
			philos[i].second_fork = right_fork;
		}
		else
		{
			philos[i].first_fork = right_fork;
			philos[i].second_fork = left_fork;
		}

		i++;
	}

	return (0);
}

/* Cleanup resources */
void	cleanup_resources(t_shared_data *shared)
{
	int	i;

	i = 0;
	while (i < shared->num_philosophers)
	{
		pthread_mutex_destroy(&shared->forks[i]);
		i++;
	}
	
	free(shared->forks);
	pthread_mutex_destroy(&shared->print_mutex);
	pthread_mutex_destroy(&shared->death_mutex);
}

/* Main function - NO GLOBAL VARIABLES */
int	main(int argc, char **argv)
{
	t_shared_data	shared;
	t_philosopher	*philosophers;
	pthread_t		death_monitor_thread;
	t_monitor_data	monitor_data;
	int				i;

	if (argc != 5 && argc != 6)
	{
		printf("Usage: %s number_of_philosophers time_to_die time_to_eat ", argv[0]);
		printf("time_to_sleep [number_of_times_each_philosopher_must_eat]\n");
		return (1);
	}

	if (init_shared_data(&shared, argc, argv) != 0)
		return (1);

	philosophers = malloc(sizeof(t_philosopher) * shared.num_philosophers);
	if (!philosophers)
		error_exit("Failed to allocate memory for philosophers");

	if (init_philosophers(philosophers, &shared) != 0)
	{
		free(philosophers);
		return (1);
	}

	/* Setup monitor data structure instead of global variable */
	monitor_data.shared = &shared;
	monitor_data.philosophers = philosophers;

	/* Create death monitor thread with proper data passing */
	if (pthread_create(&death_monitor_thread, NULL, death_monitor, &monitor_data) != 0)
	{
		error_exit("Failed to create death monitor thread");
	}

	/* Create philosopher threads */
	i = 0;
	while (i < shared.num_philosophers)
	{
		if (pthread_create(&philosophers[i].thread, NULL, 
			philosopher_routine, &philosophers[i]) != 0)
		{
			error_exit("Failed to create philosopher thread");
		}
		i++;
	}

	/* Wait for all threads */
	i = 0;
	while (i < shared.num_philosophers)
	{
		pthread_join(philosophers[i].thread, NULL);
		i++;
	}

	pthread_join(death_monitor_thread, NULL);

	/* Cleanup */
	cleanup_resources(&shared);
	free(philosophers);

	return (0);
}