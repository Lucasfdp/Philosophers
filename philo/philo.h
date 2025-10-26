/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: luferna3 <luferna3@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/26 07:09:00 by luferna3          #+#    #+#             */
/*   Updated: 2025/10/26 07:11:22 by luferna3         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <unistd.h>
# include <pthread.h>
# include <sys/time.h>
# include <limits.h>

typedef struct s_config
{
	int				num_philos;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				num_must_eat;
	long			start_time;
	int				someone_died;
	int				philos_done_eating;
	int				threads_created;
	pthread_mutex_t	eat_count_lock;
	pthread_mutex_t	print_lock;
	pthread_mutex_t	death_lock;
	pthread_mutex_t	*forks;
	struct s_philo	*philos;
}	t_config;

typedef struct s_philo
{
	int				id;
	int				meals_eaten;
	long			last_meal_time;
	pthread_t		thread;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	pthread_mutex_t	meal_lock;
	t_config		*config;
}	t_philo;

int		ft_atoi(char *str);
int		preparing_struct(int ac, char **av, t_config *philo_info);
void	free_all(t_config *philo_info, int destroy_mutexes);
void	destroy_mutex(t_config *philo_info);
int		initialise_fork_mutexes(t_config *philo_info);
void	initialise_philosophers(t_config *philo_info);
int		creating_philosophers(t_config *philo_info);
void	safe_print(t_philo *philo, char *msg);
long	get_time_in_ms(void);
void	grab_forks(t_philo *philo);
void	eat(t_philo *philo);
void	release_forks(t_philo *philo);
void	*philosopher_routine(void *arg);
void	*monitor_routine(void *arg);
void	*single_philosopher_routine(void *arg);

#endif