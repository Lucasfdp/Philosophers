#include "philo.h"

void grab_forks(t_philo *philo)
{
    // Always grab lower-numbered fork first to prevent circular wait (deadlock prevention)
    pthread_mutex_t *first_fork;
    pthread_mutex_t *second_fork;
    
    if (philo->left_fork < philo->right_fork)
    {
        first_fork = philo->left_fork;
        second_fork = philo->right_fork;
    }
    else
    {
        first_fork = philo->right_fork;
        second_fork = philo->left_fork;
    }
    
    pthread_mutex_lock(first_fork);
    safe_print(philo, "has taken a fork");
    pthread_mutex_lock(second_fork);
    safe_print(philo, "has taken a fork");
}

void	eat(t_philo *philo)
{
	safe_print(philo, "is eating");

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

	usleep(philo->config->time_to_eat * 1000);
}

void	release_forks(t_philo *philo)
{
	pthread_mutex_unlock(philo->right_fork);
	pthread_mutex_unlock(philo->left_fork);
}

void *philosopher_routine(void *arg)
{
    t_philo *philo = (t_philo *)arg;

    // Small delay for even philosophers to break symmetry
    if (philo->id % 2 == 0)
        usleep(1000);

    while (1)
    {
        // Check if someone died or simulation ended
        pthread_mutex_lock(&philo->config->death_lock);
        if (philo->config->someone_died)
        {
            pthread_mutex_unlock(&philo->config->death_lock);
            break;
        }
        pthread_mutex_unlock(&philo->config->death_lock);

        // Think
        safe_print(philo, "is thinking");
        
        // Grab forks, eat, and release
        grab_forks(philo);
        eat(philo);
        release_forks(philo);
        
        // Sleep
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
        usleep(1000);  // Check every 1ms for better precision (within 10ms requirement)
    }
    return (NULL);
}
