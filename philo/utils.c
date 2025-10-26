/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luferna3 <luferna3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 06:45:00 by luferna3          #+#    #+#             */
/*   Updated: 2025/10/26 07:05:12 by luferna3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	ft_atoi(char *str)
{
	int		i;
	long	result;

	i = 0;
	result = 0;
	if (!str || str[0] == '\0')
		return (0);
	if (str[0] == '+')
		i++;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		result = result * 10 + (str[i] - '0');
		if (result > INT_MAX)
			return (0);
		i++;
	}
	return (result);
}

void	destroy_mutex(t_config *philo_info)
{
	int	i;

	i = 0;
	if (philo_info->philos)
	{
		while (i < philo_info->num_philos)
		{
			pthread_mutex_destroy(&philo_info->philos[i].meal_lock);
			i++;
		}
	}
	if (philo_info->forks)
	{
		i = 0;
		while (i < philo_info->num_philos)
		{
			pthread_mutex_destroy(&philo_info->forks[i]);
			i++;
		}
	}
	pthread_mutex_destroy(&philo_info->print_lock);
	pthread_mutex_destroy(&philo_info->death_lock);
	pthread_mutex_destroy(&philo_info->eat_count_lock);
}

void	free_all(t_config *philo_info, int destroy_mutexes)
{
	int	i;

	if (!philo_info)
		return ;
	if (philo_info->philos)
	{
		i = 0;
		while (i < philo_info->threads_created)
		{
			pthread_join(philo_info->philos[i].thread, NULL);
			i++;
		}
	}
	if (destroy_mutexes)
		destroy_mutex(philo_info);
	if (philo_info->forks)
		free(philo_info->forks);
	if (philo_info->philos)
		free(philo_info->philos);
}

void	safe_print(t_philo *philo, char *msg)
{
	long	time;

	pthread_mutex_lock(&philo->config->print_lock);
	time = get_time_in_ms() - philo->config->start_time;
	pthread_mutex_lock(&philo->config->death_lock);
	if (!philo->config->someone_died)
		printf("%ld %d %s\n", time, philo->id, msg);
	pthread_mutex_unlock(&philo->config->death_lock);
	pthread_mutex_unlock(&philo->config->print_lock);
}

long	get_time_in_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000L) + (tv.tv_usec / 1000L));
}
