#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <sys/time.h>
# include <stdbool.h>

typedef struct s_shared_data
{
	pthread_mutex_t	*forks;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	death_mutex;
	long			simulation_start;
	bool			simulation_running;
	int			philosophers_finished;
	int			num_philosophers;
	int			time_to_die;
	int			time_to_eat;
	int			time_to_sleep;
	int			must_eat_count;
} t_shared_data;

typedef struct s_philosopher
{
	int			id;
	int			first_fork;
	int			second_fork;
	long			last_meal_time;
	int			meals_eaten;
	pthread_t		thread;
	t_shared_data		*shared;
} t_philosopher;

extern t_philosopher *g_philosophers;

// main.c
int	parse_arguments(int argc, char **argv, t_shared_data *shared);
void	error_exit(char *message);

// init.c
int	init_shared_data(t_shared_data *shared, int argc, char **argv);
int	init_philosophers(t_philosopher *philos, t_shared_data *shared);

// cleanup.c
void	cleanup_resources(t_shared_data *shared);

// monitor.c
void	*death_monitor(void *arg);

// routine.c
void	*philosopher_routine(void *arg);

// utils.c
long	get_current_time_ms(void);
int	get_elapsed_time(long start_time);
void	precise_sleep(int duration_ms);
int	is_valid_number(char *str);

// print.c
void	print_status(t_philosopher *philo, char *status);
bool	is_simulation_running(t_shared_data *shared);
void	stop_simulation(t_shared_data *shared);

#endif
