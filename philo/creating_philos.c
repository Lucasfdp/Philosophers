#include "philo.h"

int	initialise_fork_mutexes(t_config *philo_info)
{
	int	i;

	i = 0;
	while (i < philo_info->num_philos)
	{
		if (pthread_mutex_init(&philo_info->forks[i], NULL) != 0)
		{
			while (--i >= 0)
				pthread_mutex_destroy(&philo_info->forks[i]);
			free(philo_info->forks);
			free(philo_info->philos);
			return (0);
		}
		i++;
	}
	return (1);
}

void initialise_philosophers(t_config *philo_info)
{
    int i;

    i = 0;
    while (i < philo_info->num_philos)
    {
        philo_info->philos[i].id = i + 1;
        philo_info->philos[i].meals_eaten = 0;
        philo_info->philos[i].last_meal_time = philo_info->start_time;
        philo_info->philos[i].config = philo_info;
        philo_info->philos[i].thread = 0;
        pthread_mutex_init(&philo_info->philos[i].meal_lock, NULL);
        philo_info->philos[i].left_fork = &philo_info->forks[i];
        philo_info->philos[i].right_fork = &philo_info->forks[(i + 1) % philo_info->num_philos];
        i++;
    }
}

int	creating_philosophers(t_config *philo_info)
{
	philo_info->philos = malloc(sizeof(t_philo) * philo_info->num_philos);
	if (!philo_info->philos)
		return (0);
	philo_info->forks = malloc(sizeof(pthread_mutex_t) * philo_info->num_philos);
	if (!philo_info->forks)
	{
		free_all(philo_info, 1);
		return (0);
	}
	if (!initialise_fork_mutexes(philo_info))
		return (0);
	if (pthread_mutex_init(&philo_info->print_lock, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&philo_info->death_lock, NULL) != 0)
	{
		free_all(philo_info, 1);
		return (0);
	}
	if (pthread_mutex_init(&philo_info->eat_count_lock, NULL) != 0)
	{
		free_all(philo_info, 1);
		return (0);
	}
	initialise_philosophers(philo_info);
	return (1);
}
