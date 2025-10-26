/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luferna3 <luferna3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 07:06:27 by luferna3          #+#    #+#             */
/*   Updated: 2025/10/26 07:06:28 by luferna3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

// int main(int ac, char *av[])
// {
// 	t_config philo_info;
// 	pthread_t monitor;
// 	int i;

// 	if (ac != 5 && ac != 6)
// 	{
// 		printf("Error\nUsage: ./philo n t_die t_eat t_sleep [n_eat]\n");
// 		return (0);
// 	}
// 	if (!preparing_struct(ac, av, &philo_info))
// 		return (printf("Error\nInvalid arguments\n"), 0);
// 	philo_info.start_time = get_time_in_ms();
// 	if (!creating_philosophers(&philo_info))
// 		return (printf("Error\nFailed to create philosophers\n"), 0);
// 	if (philo_info.num_philos == 1)
// 	{
// 		if (pthread_create(&philo_info.philos[0].thread,
// 				NULL, single_philosopher_routine, &philo_info.philos[0]) != 0)
// 		{
// 			printf("Error\nFailed to create philosopher thread\n");
// 			destroy_mutex(&philo_info);
// 			free(philo_info.forks);
// 			free(philo_info.philos);
// 			return (0);
// 		}
// 		philo_info.threads_created = 1;
// 		if (pthread_create(&monitor, NULL, monitor_routine, &philo_info) == 0)
// 			pthread_join(monitor, NULL);
// 		pthread_join(philo_info.philos[0].thread, NULL);
// 		destroy_mutex(&philo_info);
// 		free(philo_info.forks);
// 		free(philo_info.philos);
// 		return (0);
// 	}
// 	i = 0;
// 	while (i < philo_info.num_philos)
// 	{
// 		if (pthread_create(&philo_info.philos[i].thread,
// 				NULL, philosopher_routine, &philo_info.philos[i]) != 0)
// 		{
// 			printf("Error\nFailed to create philosopher thread %d\n", i + 1);
// 			philo_info.threads_created = i;
// 			pthread_mutex_lock(&philo_info.death_lock);
// 			philo_info.someone_died = 1;
// 			pthread_mutex_unlock(&philo_info.death_lock);
// 			while (--i >= 0)
// 				pthread_join(philo_info.philos[i].thread, NULL);
// 			destroy_mutex(&philo_info);
// 			free(philo_info.forks);
// 			free(philo_info.philos);
// 			return (0);
// 		}
// 		i++;
// 	}
// 	philo_info.threads_created = philo_info.num_philos;
// 	if (pthread_create(&monitor, NULL, monitor_routine, &philo_info) != 0)
// 	{
// 		printf("Error\nFailed to create monitor\n");
// 		pthread_mutex_lock(&philo_info.death_lock);
// 		philo_info.someone_died = 1;
// 		pthread_mutex_unlock(&philo_info.death_lock);
// 		i = 0;
// 		while (i < philo_info.threads_created)
// 		{
// 			pthread_join(philo_info.philos[i].thread, NULL);
// 			i++;
// 		}
// 		destroy_mutex(&philo_info);
// 		free(philo_info.forks);
// 		free(philo_info.philos);
// 		return (0);
// 	}
// 	pthread_join(monitor, NULL);
// 	i = 0;
// 	while (i < philo_info.threads_created)
// 	{
// 		pthread_join(philo_info.philos[i].thread, NULL);
// 		i++;
// 	}
// 	destroy_mutex(&philo_info);
// 	free(philo_info.forks);
// 	free(philo_info.philos);
// 	return (0);
// }

static void	handle_single_philo(t_config *philo_info)
{
	pthread_t	monitor;

	if (pthread_create(&philo_info->philos[0].thread,
			NULL, single_philosopher_routine, &philo_info->philos[0]) != 0)
	{
		printf("Error\nFailed to create philosopher thread\n");
		destroy_mutex(philo_info);
		free(philo_info->forks);
		free(philo_info->philos);
		return ;
	}
	philo_info->threads_created = 1;
	if (pthread_create(&monitor, NULL, monitor_routine, philo_info) == 0)
		pthread_join(monitor, NULL);
	pthread_join(philo_info->philos[0].thread, NULL);
	destroy_mutex(philo_info);
	free(philo_info->forks);
	free(philo_info->philos);
}

static int	create_philo_threads(t_config *philo_info)
{
	int	i;

	i = 0;
	while (i < philo_info->num_philos)
	{
		if (pthread_create(&philo_info->philos[i].thread,
				NULL, philosopher_routine, &philo_info->philos[i]) != 0)
		{
			printf("Error\nFailed to create philosopher thread %d\n", i + 1);
			philo_info->threads_created = i;
			pthread_mutex_lock(&philo_info->death_lock);
			philo_info->someone_died = 1;
			pthread_mutex_unlock(&philo_info->death_lock);
			while (--i >= 0)
				pthread_join(philo_info->philos[i].thread, NULL);
			return (0);
		}
		i++;
	}
	philo_info->threads_created = philo_info->num_philos;
	return (1);
}

static int	create_monitor_thread(t_config *philo_info, pthread_t *monitor)
{
	if (pthread_create(monitor, NULL, monitor_routine, philo_info) != 0)
	{
		printf("Error\nFailed to create monitor\n");
		pthread_mutex_lock(&philo_info->death_lock);
		philo_info->someone_died = 1;
		pthread_mutex_unlock(&philo_info->death_lock);
		return (0);
	}
	return (1);
}

static void	cleanup_and_exit(t_config *philo_info)
{
	int	i;

	i = 0;
	while (i < philo_info->threads_created)
	{
		pthread_join(philo_info->philos[i].thread, NULL);
		i++;
	}
	destroy_mutex(philo_info);
	free(philo_info->forks);
	free(philo_info->philos);
}

int	main(int ac, char *av[])
{
	t_config	philo_info;
	pthread_t	monitor;

	if (ac != 5 && ac != 6)
		return (printf("Error\nUsage: ./philo n t_die t_eat t_sleep [n_eat]\n"), 0);
	if (!preparing_struct(ac, av, &philo_info))
		return (printf("Error\nInvalid arguments\n"), 0);
	philo_info.start_time = get_time_in_ms();
	if (!creating_philosophers(&philo_info))
		return (printf("Error\nFailed to create philosophers\n"), 0);
	if (philo_info.num_philos == 1)
		return (handle_single_philo(&philo_info), 0);
	if (!create_philo_threads(&philo_info))
		return (cleanup_and_exit(&philo_info), 0);
	if (!create_monitor_thread(&philo_info, &monitor))
		return (cleanup_and_exit(&philo_info), 0);
	pthread_join(monitor, NULL);
	cleanup_and_exit(&philo_info);
	return (0);
}
