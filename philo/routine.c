#include "philo.h"

// void grab_forks(t_philo *philo)
// {
// 	// For even number of philosophers, use different strategy
// 	// Even IDs grab right fork first, odd IDs grab left fork first
// 	if (philo->id % 2 == 0)
// 	{
// 		pthread_mutex_lock(philo->right_fork);
// 		safe_print(philo, "has taken a fork");
// 		pthread_mutex_lock(philo->left_fork);
// 		safe_print(philo, "has taken a fork");
// 	}
// 	else
// 	{
// 		pthread_mutex_lock(philo->left_fork);
// 		safe_print(philo, "has taken a fork");
// 		pthread_mutex_lock(philo->right_fork);
// 		safe_print(philo, "has taken a fork");
// 	}
// }

// void eat(t_philo *philo)
// {
// 	// Update last_meal_time BEFORE printing to avoid race condition
// 	pthread_mutex_lock(&philo->meal_lock);
// 	philo->last_meal_time = get_time_in_ms();
// 	philo->meals_eaten++;
// 	if (philo->config->num_must_eat > 0
// 		&& philo->meals_eaten == philo->config->num_must_eat)
// 	{
// 		pthread_mutex_lock(&philo->config->eat_count_lock);
// 		philo->config->philos_done_eating++;
// 		pthread_mutex_unlock(&philo->config->eat_count_lock);
// 	}
// 	pthread_mutex_unlock(&philo->meal_lock);

// 	safe_print(philo, "is eating");
// 	usleep(philo->config->time_to_eat * 1000);  // Just sleep for the full duration
// }

// void release_forks(t_philo *philo)
// {
// 	// Release in the same order as acquired (though order doesn't matter for unlock)
// 	pthread_mutex_unlock(philo->left_fork);
// 	pthread_mutex_unlock(philo->right_fork);
// }

// void *philosopher_routine(void *arg)
// {
// 	t_philo *philo = (t_philo *)arg;
// 	int holding_forks = 0;

// 	// Initial stagger
// 	if (philo->id % 2 == 0)
// 		usleep(philo->config->time_to_eat * 1000);

// 	while (1)
// 	{
// 		// Check if someone died or simulation ended
// 		pthread_mutex_lock(&philo->config->death_lock);
// 		if (philo->config->someone_died)
// 		{
// 			pthread_mutex_unlock(&philo->config->death_lock);
// 			if (holding_forks)
// 				release_forks(philo);
// 			break;
// 		}
// 		pthread_mutex_unlock(&philo->config->death_lock);

// 		// Think - just print, no delay
// 		safe_print(philo, "is thinking");

// 		// Grab forks, eat, and release
// 		grab_forks(philo);
// 		holding_forks = 1;
		
// 		eat(philo);
		
// 		release_forks(philo);
// 		holding_forks = 0;
		
// 		// Sleep
// 		safe_print(philo, "is sleeping");
// 		usleep(philo->config->time_to_sleep * 1000);
		
// 		// Add a tiny thinking period for even-numbered philosophers
// 		// This helps maintain the staggered pattern
// 		if (philo->id % 2 == 0 && philo->config->num_philos % 2 == 0)
// 			usleep(1000); // 1ms to stay slightly behind odd philosophers
// 	}
// 	return (NULL);
// }


void grab_forks(t_philo *philo)
{
	// Always grab lower ID fork first to prevent deadlock
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

void eat(t_philo *philo)
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

void release_forks(t_philo *philo)
{
	pthread_mutex_unlock(philo->left_fork);
	pthread_mutex_unlock(philo->right_fork);
}

void *philosopher_routine(void *arg)
{
	t_philo *philo = (t_philo *)arg;
	//long time_since_last_meal;
	//long time_until_death;
	int can_afford_thinking;
	
	// Initialize last_meal_time when thread actually starts  
	pthread_mutex_lock(&philo->meal_lock);
	philo->last_meal_time = get_time_in_ms();
	pthread_mutex_unlock(&philo->meal_lock);
	
	// Stagger based on whether we have even or odd total philosophers
	if (philo->config->num_philos % 2 == 0)
	{
		// Even total: delay even IDs by time_to_eat
		if (philo->id % 2 == 0)
			usleep(philo->config->time_to_eat * 1000);
	}
	else
	{
		// Odd total: last philosopher waits
		if (philo->id == philo->config->num_philos)
			usleep(philo->config->time_to_eat * 1000);
	}
	
	// Determine if timing is tight (small buffer between death time and eat+sleep time)
	can_afford_thinking = (philo->config->time_to_die > 
	                       philo->config->time_to_eat + philo->config->time_to_sleep + 30);
	
	while (1)
	{
		pthread_mutex_lock(&philo->config->death_lock);
		if (philo->config->someone_died)
		{
			pthread_mutex_unlock(&philo->config->death_lock);
			break;
		}
		pthread_mutex_unlock(&philo->config->death_lock);

		// Only print thinking if we can afford the time
		if (can_afford_thinking)
		{
			safe_print(philo, "is thinking");
		}
		
		// EAT
		grab_forks(philo);
		eat(philo);
		release_forks(philo);
		
		// SLEEP
		safe_print(philo, "is sleeping");
		usleep(philo->config->time_to_sleep * 1000);
	}
	return (NULL);
}

void *single_philosopher_routine(void *arg)
{
	t_philo *philo = (t_philo *)arg;

	safe_print(philo, "has taken a fork");
	usleep(philo->config->time_to_die * 1000);
	
	return (NULL);
}

void *monitor_routine(void *arg)
{
	t_config *info = (t_config *)arg;
	int i;
	long now;
	long time_since_meal;

	while (1)
	{
		// Check if all philosophers have eaten enough
		if (info->num_must_eat > 0)
		{
			pthread_mutex_lock(&info->eat_count_lock);
			if (info->philos_done_eating == info->num_philos)
			{
				pthread_mutex_unlock(&info->eat_count_lock);
				pthread_mutex_lock(&info->death_lock);
				info->someone_died = 1;
				pthread_mutex_unlock(&info->death_lock);
				return (NULL);
			}
			pthread_mutex_unlock(&info->eat_count_lock);
		}

		// Check for deaths
		i = 0;
		while (i < info->num_philos)
		{
			pthread_mutex_lock(&info->philos[i].meal_lock);
			now = get_time_in_ms();
			time_since_meal = now - info->philos[i].last_meal_time;
			
			if (time_since_meal > info->time_to_die)
			{
				pthread_mutex_unlock(&info->philos[i].meal_lock);
				
				pthread_mutex_lock(&info->death_lock);
				if (!info->someone_died)
				{
					info->someone_died = 1;
					pthread_mutex_unlock(&info->death_lock);
					
					// Print death message immediately
					pthread_mutex_lock(&info->print_lock);
					printf("%ld %d died\n", now - info->start_time, info->philos[i].id);
					pthread_mutex_unlock(&info->print_lock);
				}
				else
					pthread_mutex_unlock(&info->death_lock);
				return (NULL);
			}
			pthread_mutex_unlock(&info->philos[i].meal_lock);
			i++;
		}
		usleep(1000);  // Check every 1ms
	}
	return (NULL);
}