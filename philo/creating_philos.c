/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   creating_philos.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luferna3 <luferna3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 06:47:13 by luferna3          #+#    #+#             */
/*   Updated: 2025/10/26 06:52:06 by luferna3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

void	initialise_philosophers(t_config *phinfo)
{
	int	i;

	i = 0;
	while (i < phinfo->num_philos)
	{
		phinfo->philos[i].id = i + 1;
		phinfo->philos[i].meals_eaten = 0;
		phinfo->philos[i].last_meal_time = 0;
		phinfo->philos[i].config = phinfo;
		phinfo->philos[i].thread = 0;
		pthread_mutex_init(&phinfo->philos[i].meal_lock, NULL);
		phinfo->philos[i].left_fork = &phinfo->forks[i];
		phinfo->philos[i].right_fork = &phinfo->forks[(i + 1)
			% phinfo->num_philos];
		i++;
	}
}

int	creating_philosophers(t_config *phinfo)
{
	phinfo->philos = malloc(sizeof(t_philo) * phinfo->num_philos);
	if (!phinfo->philos)
		return (0);
	phinfo->forks = malloc(sizeof(pthread_mutex_t) * phinfo->num_philos);
	if (!phinfo->forks)
	{
		free_all(phinfo, 1);
		return (0);
	}
	if (!initialise_fork_mutexes(phinfo))
		return (0);
	if (pthread_mutex_init(&phinfo->print_lock, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&phinfo->death_lock, NULL) != 0)
	{
		free_all(phinfo, 1);
		return (0);
	}
	if (pthread_mutex_init(&phinfo->eat_count_lock, NULL) != 0)
	{
		free_all(phinfo, 1);
		return (0);
	}
	initialise_philosophers(phinfo);
	return (1);
}
