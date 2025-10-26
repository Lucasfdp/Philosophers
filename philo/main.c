#include "philo.h"

int main(int ac, char *av[])
{
    t_config philo_info;
    pthread_t monitor;
    int i;

    if (ac != 5 && ac != 6)
    {
        printf("Error\nUsage: ./philo n t_die t_eat t_sleep [n_eat]\n");
        return (0);
    }
    if (!preparing_struct(ac, av, &philo_info))
        return (printf("Error\nInvalid arguments\n"), 0);
    
    philo_info.start_time = get_time_in_ms();
    
    if (!creating_philosophers(&philo_info))
        return (printf("Error\nFailed to create philosophers\n"), 0);
    
    // Handle special case: single philosopher
    if (philo_info.num_philos == 1)
    {
        if (pthread_create(&philo_info.philos[0].thread,
                NULL, single_philosopher_routine, &philo_info.philos[0]) != 0)
        {
            printf("Error\nFailed to create philosopher thread\n");
            destroy_mutex(&philo_info);
            free(philo_info.forks);
            free(philo_info.philos);
            return (0);
        }
        philo_info.threads_created = 1;
        
        if (pthread_create(&monitor, NULL, monitor_routine, &philo_info) == 0)
            pthread_join(monitor, NULL);
        
        pthread_join(philo_info.philos[0].thread, NULL);
        destroy_mutex(&philo_info);
        free(philo_info.forks);
        free(philo_info.philos);
        return (0);
    }
    
    // Create philosopher threads
    i = 0;
    while (i < philo_info.num_philos)
    {
        if (pthread_create(&philo_info.philos[i].thread,
                NULL, philosopher_routine, &philo_info.philos[i]) != 0)
        {
            printf("Error\nFailed to create philosopher thread %d\n", i + 1);
            philo_info.threads_created = i;
            pthread_mutex_lock(&philo_info.death_lock);
            philo_info.someone_died = 1;
            pthread_mutex_unlock(&philo_info.death_lock);
            
            // Wait for created threads
            while (--i >= 0)
                pthread_join(philo_info.philos[i].thread, NULL);
            
            destroy_mutex(&philo_info);
            free(philo_info.forks);
            free(philo_info.philos);
            return (0);
        }
        i++;
    }
    philo_info.threads_created = philo_info.num_philos;
    
    // Create monitor thread
    if (pthread_create(&monitor, NULL, monitor_routine, &philo_info) != 0)
    {
        printf("Error\nFailed to create monitor\n");
        pthread_mutex_lock(&philo_info.death_lock);
        philo_info.someone_died = 1;
        pthread_mutex_unlock(&philo_info.death_lock);
        
        i = 0;
        while (i < philo_info.threads_created)
        {
            pthread_join(philo_info.philos[i].thread, NULL);
            i++;
        }
        
        destroy_mutex(&philo_info);
        free(philo_info.forks);
        free(philo_info.philos);
        return (0);
    }
    
    // Wait for monitor to finish
    pthread_join(monitor, NULL);
    
    // Wait for all philosopher threads
    i = 0;
    while (i < philo_info.threads_created)
    {
        pthread_join(philo_info.philos[i].thread, NULL);
        i++;
    }
    
    // Cleanup
    destroy_mutex(&philo_info);
    free(philo_info.forks);
    free(philo_info.philos);
    
    return (0);
}
