/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luferna3 <luferna3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 07:07:23 by luferna3          #+#    #+#             */
/*   Updated: 2025/10/26 07:08:34 by luferna3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	grab_forks(t_philo *philo)
{
	if (philo->left_fork < philo->right_fork)
	{
		pthread_mutex_lock(philo->left_fork);
		safe_print(philo, "has taken a fork");
		pthread_mutex_lock(philo->right_fork);
		safe_print(philo, "has taken a fork");
	}
	else
	{
		pthread_mutex_lock(philo->right_fork);
		safe_print(philo, "has taken a fork");
		pthread_mutex_lock(philo->left_fork);
		safe_print(philo, "has taken a fork");
	}
}

void	eat(t_philo *philo)
{
	pthread_mutex_lock(&philo->meal_lock);
	philo->last_meal_time = get_time_in_ms();
	philo->meals_eaten++;
	if (philo->config->num_must_eat > 0
		&& philo->meals_eaten == philo->config->num_must_eat)
		{
		pthread_mutex_lock(&philo->config->eat_count_lock);
		philo->config->philos_done_eating++;
		pthread_mutex_unlock(&philo->config->eat_count_lock);
	}
	pthread_mutex_unlock(&philo->meal_lock);
	safe_print(philo, "is eating");
	usleep(philo->config->time_to_eat * 1000);
}

void	release_forks(t_philo *philo)
{
	pthread_mutex_unlock(philo->left_fork);
	pthread_mutex_unlock(philo->right_fork);
}

// void *philosopher_routine(void *arg)
// {
// 	t_philo *philo = (t_philo *)arg;
// 	int can_afford_thinking;
	
// 	pthread_mutex_lock(&philo->meal_lock);
// 	philo->last_meal_time = get_time_in_ms();
// 	pthread_mutex_unlock(&philo->meal_lock);
// 	if (philo->config->num_philos % 2 == 0)
// 	{
// 		if (philo->id % 2 == 0)
// 			usleep(philo->config->time_to_eat * 1000);
// 	}
// 	else
// 	{
// 		if (philo->id == philo->config->num_philos)
// 			usleep(philo->config->time_to_eat * 1000);
// 	}
// 	can_afford_thinking = (philo->config->time_to_die > 
// 	                       philo->config->time_to_eat + philo->config->time_to_sleep + 30);
	
// 	while (1)
// 	{
// 		pthread_mutex_lock(&philo->config->death_lock);
// 		if (philo->config->someone_died)
// 		{
// 			pthread_mutex_unlock(&philo->config->death_lock);
// 			break;
// 		}
// 		pthread_mutex_unlock(&philo->config->death_lock);
// 		if (can_afford_thinking)
// 		{
// 			safe_print(philo, "is thinking");
// 		}
// 		grab_forks(philo);
// 		eat(philo);
// 		release_forks(philo);
// 		safe_print(philo, "is sleeping");
// 		usleep(philo->config->time_to_sleep * 1000);
// 	}
// 	return (NULL);
// }

static void	init_philo_meal_time(t_philo *philo)
{
	pthread_mutex_lock(&philo->meal_lock);
	philo->last_meal_time = get_time_in_ms();
	pthread_mutex_unlock(&philo->meal_lock);
}

static void	stagger_philo_start(t_philo *philo)
{
	if (philo->config->num_philos % 2 == 0)
	{
		if (philo->id % 2 == 0)
			usleep(philo->config->time_to_eat * 1000);
	}
	else
	{
		if (philo->id == philo->config->num_philos)
			usleep(philo->config->time_to_eat * 1000);
	}
}

static int	can_afford_thinking(t_philo *philo)
{
	return (philo->config->time_to_die > 
		philo->config->time_to_eat + philo->config->time_to_sleep + 30);
}

static int	check_simulation_end(t_philo *philo)
{
	pthread_mutex_lock(&philo->config->death_lock);
	if (philo->config->someone_died)
	{
		pthread_mutex_unlock(&philo->config->death_lock);
		return (1);
	}
	pthread_mutex_unlock(&philo->config->death_lock);
	return (0);
}

void	*philosopher_routine(void *arg)
{
	t_philo	*philo;
	int		thinking_allowed;

	philo = (t_philo *)arg;
	init_philo_meal_time(philo);
	stagger_philo_start(philo);
	thinking_allowed = can_afford_thinking(philo);
	while (1)
	{
		if (check_simulation_end(philo))
			break;
		if (thinking_allowed)
			safe_print(philo, "is thinking");
		grab_forks(philo);
		eat(philo);
		release_forks(philo);
		safe_print(philo, "is sleeping");
		usleep(philo->config->time_to_sleep * 1000);
	}
	return (NULL);
}

void	*single_philosopher_routine(void *arg)
{
	t_philo *philo = (t_philo *)arg;

	safe_print(philo, "has taken a fork");
	usleep(philo->config->time_to_die * 1000);
	
	return (NULL);
}

// void *monitor_routine(void *arg)
// {
// 	t_config *info = (t_config *)arg;
// 	int i;
// 	long now;
// 	long time_since_meal;

// 	while (1)
// 	{
// 		if (info->num_must_eat > 0)
// 		{
// 			pthread_mutex_lock(&info->eat_count_lock);
// 			if (info->philos_done_eating == info->num_philos)
// 			{
// 				pthread_mutex_unlock(&info->eat_count_lock);
// 				pthread_mutex_lock(&info->death_lock);
// 				info->someone_died = 1;
// 				pthread_mutex_unlock(&info->death_lock);
// 				return (NULL);
// 			}
// 			pthread_mutex_unlock(&info->eat_count_lock);
// 		}
// 		i = 0;
// 		while (i < info->num_philos)
// 		{
// 			pthread_mutex_lock(&info->philos[i].meal_lock);
// 			now = get_time_in_ms();
// 			time_since_meal = now - info->philos[i].last_meal_time;
// 			if (time_since_meal > info->time_to_die)
// 			{
// 				pthread_mutex_unlock(&info->philos[i].meal_lock);
// 				pthread_mutex_lock(&info->death_lock);
// 				if (!info->someone_died)
// 				{
// 					info->someone_died = 1;
// 					pthread_mutex_unlock(&info->death_lock);					
// 					pthread_mutex_lock(&info->print_lock);
// 					printf("%ld %d died\n", now - info->start_time, info->philos[i].id);
// 					pthread_mutex_unlock(&info->print_lock);
// 				}
// 				else
// 					pthread_mutex_unlock(&info->death_lock);
// 				return (NULL);
// 			}
// 			pthread_mutex_unlock(&info->philos[i].meal_lock);
// 			i++;
// 		}
// 		usleep(1000);
// 	}
// 	return (NULL);
// }

static int	check_all_eaten(t_config *info)
{
	if (info->num_must_eat > 0)
	{
		pthread_mutex_lock(&info->eat_count_lock);
		if (info->philos_done_eating == info->num_philos)
		{
			pthread_mutex_unlock(&info->eat_count_lock);
			pthread_mutex_lock(&info->death_lock);
			info->someone_died = 1;
			pthread_mutex_unlock(&info->death_lock);
			return (1);
		}
		pthread_mutex_unlock(&info->eat_count_lock);
	}
	return (0);
}

static void	print_death(t_config *info, int philo_id, long timestamp)
{
	pthread_mutex_lock(&info->death_lock);
	if (!info->someone_died)
	{
		info->someone_died = 1;
		pthread_mutex_unlock(&info->death_lock);
		pthread_mutex_lock(&info->print_lock);
		printf("%ld %d died\n", timestamp, philo_id);
		pthread_mutex_unlock(&info->print_lock);
	}
	else
		pthread_mutex_unlock(&info->death_lock);
}

static int	check_philo_death(t_config *info, int i)
{
	long	now;
	long	time_since_meal;

	pthread_mutex_lock(&info->philos[i].meal_lock);
	now = get_time_in_ms();
	time_since_meal = now - info->philos[i].last_meal_time;
	if (time_since_meal > info->time_to_die)
	{
		pthread_mutex_unlock(&info->philos[i].meal_lock);
		print_death(info, info->philos[i].id, now - info->start_time);
		return (1);
	}
	pthread_mutex_unlock(&info->philos[i].meal_lock);
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_config	*info;
	int			i;

	info = (t_config *)arg;
	while (1)
	{
		if (check_all_eaten(info))
			return (NULL);
		i = 0;
		while (i < info->num_philos)
		{
			if (check_philo_death(info, i))
				return (NULL);
			i++;
		}
		usleep(1000);
	}
	return (NULL);
}
