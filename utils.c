#include "philo.h"

long get_current_time_ms(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

int get_elapsed_time(long start_time)
{
	return ((int)(get_current_time_ms() - start_time));
}

void precise_sleep(int duration_ms)
{
	long start = get_current_time_ms();
	if (duration_ms > 10)
		usleep((duration_ms - 10) * 1000);
	while ((get_current_time_ms() - start) < duration_ms)
		usleep(100);
}

int is_valid_number(char *str)
{
	int i = 0;
	if (!str || !*str)
		return (0);
	while (str[i])
		if (str[i] < '0' || str[i++] > '9')
			return (0);
	return (1);
}

void error_exit(char *message)
{
	printf("Error: %s\n", message);
	exit(1);
}
