/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_args.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luferna3 <luferna3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 07:06:32 by luferna3          #+#    #+#             */
/*   Updated: 2025/10/26 07:06:33 by luferna3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	is_positive_number(const char *str)
{
	int	i;

	i = 0;
	if (!str || str[0] == '\0')
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

// int preparing_struct(int ac, char **av, t_config *philo_info)
// {
// 	if (!is_positive_number(av[1])
// 		|| !is_positive_number(av[2])
// 		|| !is_positive_number(av[3])
// 		|| !is_positive_number(av[4])
// 		|| (ac == 6 && !is_positive_number(av[5])))
// 		return (0);
// 	philo_info->num_philos = ft_atoi(av[1]);
// 	philo_info->time_to_die = ft_atoi(av[2]);
// 	philo_info->time_to_eat = ft_atoi(av[3]);
// 	philo_info->time_to_sleep = ft_atoi(av[4]);
// 	if (ac == 6)
// 		philo_info->num_must_eat = ft_atoi(av[5]);
// 	else
// 		philo_info->num_must_eat = -1;
// 	if (philo_info->num_philos <= 0
// 		|| philo_info->time_to_die <= 0
// 		|| philo_info->time_to_eat <= 0
// 		|| philo_info->time_to_sleep <= 0
// 		|| (ac == 6 && philo_info->num_must_eat <= 0))
// 		return (0);
// 	philo_info->someone_died = 0;
// 	philo_info->philos_done_eating = 0;
// 	philo_info->threads_created = 0;
// 	philo_info->start_time = 0;
// 	philo_info->philos = NULL;
// 	philo_info->forks = NULL;
// 	return (1);
// }

static int	validate_args(int ac, char **av)
{
	if (!is_positive_number(av[1])
		|| !is_positive_number(av[2])
		|| !is_positive_number(av[3])
		|| !is_positive_number(av[4])
		|| (ac == 6 && !is_positive_number(av[5])))
		return (0);
	return (1);
}

static int	validate_values(t_config *philo_info, int ac)
{
	if (philo_info->num_philos <= 0
		|| philo_info->time_to_die <= 0
		|| philo_info->time_to_eat <= 0
		|| philo_info->time_to_sleep <= 0
		|| (ac == 6 && philo_info->num_must_eat <= 0))
		return (0);
	return (1);
}

int	preparing_struct(int ac, char **av, t_config *philo_info)
{
	if (!validate_args(ac, av))
		return (0);
	philo_info->num_philos = ft_atoi(av[1]);
	philo_info->time_to_die = ft_atoi(av[2]);
	philo_info->time_to_eat = ft_atoi(av[3]);
	philo_info->time_to_sleep = ft_atoi(av[4]);
	if (ac == 6)
		philo_info->num_must_eat = ft_atoi(av[5]);
	else
		philo_info->num_must_eat = -1;
	if (!validate_values(philo_info, ac))
		return (0);
	philo_info->someone_died = 0;
	philo_info->philos_done_eating = 0;
	philo_info->threads_created = 0;
	philo_info->start_time = 0;
	philo_info->philos = NULL;
	philo_info->forks = NULL;
	return (1);
}
