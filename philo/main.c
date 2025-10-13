#include "philo.h"

int	main(int ac, char *av[])
{
	t_config	philo_info;
	pthread_t	monitor;
	int			i;

	if (ac != 5 && ac != 6)
	{
		printf("Error\nUsage: ./philo n t_die t_eat t_sleep [n_eat]\n");
		return (0);
	}
	if (!preparing_struct(ac, av, &philo_info))
		return (printf("Error\nstruct\n"), 0);
	printf("Starting simulation with %d philosophers\n", philo_info.num_philos);
	philo_info.start_time = get_time_in_ms();
	if (!creating_philosophers(&philo_info))
		return (printf("Error\nFailed to create philosophers\n"), 0);
	i = 0;
	while (i < philo_info.num_philos)
	{
		if (pthread_create(&philo_info.philos[i].thread,
				NULL, philosopher_routine, &philo_info.philos[i]) != 0)
		{
			printf("Error\nFailed to create philosopher thread %d\n", i + 1);
			return (0);
		}
		i++;
	}
	if (pthread_create(&monitor, NULL, monitor_routine, &philo_info) != 0)
	{
		printf("Error\nFailed to create monitor\n");
		return (0);
	}
	pthread_join(monitor, NULL);
	i = 0;
	while (i < philo_info.num_philos)
		pthread_join(philo_info.philos[i++].thread, NULL);
	free_all(&philo_info, 1);
	return (0);
}
