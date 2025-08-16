#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

struct			s_data;

typedef struct s_queue
{
	int				*buffer;
	int				head;
	int				tail;
	int				size;
	int				capacity;
}					t_queue;

typedef struct s_philosopher
{
	int				id;
	long long		last_meal_time;
	long long		request_time;
	int				meals_eaten;
	volatile int	gate_flag;
	volatile int	done_flag;
	pthread_t		thread;
	struct s_data	*data;
}					t_philosopher;

typedef enum e_fork_state
{
	FORK_FREE,
	FORK_RESERVED,
	FORK_TAKEN
}					t_fork_state;

typedef struct s_data
{
	int				num_philosophers;
	long			time_to_die;
	long			time_to_eat;
	long			time_to_sleep;
	int				meals_required;
	long long		start_time;
	volatile int	simulation_over;
	t_philosopher	*philosophers;
	pthread_mutex_t	*forks;
	t_fork_state	*fork_states;
	t_queue			admission_queue;
	pthread_mutex_t	admission_mutex;
	pthread_mutex_t	print_mutex;
	pthread_t		manager_thread;
	pthread_t		watchdog_thread;
}					t_data;

// init.c
int					parse_arguments(int argc, char **argv, t_data *data);
int					init_data(t_data *data);
void				cleanup_data(t_data *data);

// utils.c
long long			get_time_ms(void);
void				ft_sleep(long ms);
void				print_status(t_philosopher *philo, char *status);

// queue.c
int					queue_init(t_queue *queue, int capacity);
void				queue_destroy(t_queue *queue);
int					queue_enqueue(t_queue *queue, int philosopher_id);
int					queue_dequeue(t_queue *queue);
int					queue_peek(t_queue *queue);
int					queue_is_empty(t_queue *queue);
int					queue_size(t_queue *queue);
int					queue_contains(t_queue *queue, int philosopher_id);

// philosopher.c
void				*philosopher_routine(void *arg);

// routine.c
void				eat(t_philosopher *philo);
void				sleep_phase(t_philosopher *philo);
void				think(t_philosopher *philo);

// manager.c
void				*manager_routine(void *arg);

// forks.c
int					forks_available(t_data *data, int philosopher_id);
void				reserve_forks(t_data *data, int philosopher_id);
void				free_forks(t_data *data, int philosopher_id);

// watchdog.c
void				*watchdog_routine(void *arg);

#endif
