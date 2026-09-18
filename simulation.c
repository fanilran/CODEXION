/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanilran <fanilran@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 11:14:21 by fanilran          #+#    #+#             */
/*   Updated: 2026/09/18 17:09:04 by fanilran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	check_burnout(t_coder *coders)
{
	int	i;
	long	deadline;
	long	now;
	int		done;
	pthread_mutex_t	test;
	
	i = 0;
	pthread_mutex_init(&test, NULL);
	while (i < coders->config->number_of_coder)
	{
		pthread_mutex_lock(&coders[i].activity_mutex);
		deadline = coders[i].last_compile_start + coders[i].config->time_to_burnout;
		done = coders[i].compile_done;
		pthread_mutex_unlock(&coders[i].activity_mutex);
		now = get_timestamp_ms(coders[i].config->start_time);
		if (now >= deadline && done < coders->config->number_of_compiles_required)
		// if (now >= deadline)
		{
			pthread_mutex_lock(&coders[i].config->stop_mutex);
			coders[i].config->stop = 1;
			pthread_mutex_unlock(&coders[i].config->stop_mutex);
			log_msg(&coders[i], "burned out");
			return (1);
		}
		i++;
	}
	pthread_mutex_destroy(&test);
	return (0);
}

int	check_all_done(t_coder *coders)
{
	int	i;
	int	done;

	i = 0;
	while (i < coders->config->number_of_coder)
	{
		pthread_mutex_lock(&coders[i].activity_mutex);
		done = coders[i].compile_done;
		pthread_mutex_unlock(&coders[i].activity_mutex);
		if (done < coders->config->number_of_compiles_required)
			return (0);
		i++;
	}
	return (1);
}

void	*routine(void *arg)
{
	t_coder	*coder;
	int		i;

	coder = (t_coder *)arg;
	i = 0;
	while (i < coder->config->number_of_compiles_required)
	{
		take_dongle(coder);

		pthread_mutex_lock(&coder->activity_mutex);
		coder->last_compile_start = get_timestamp_ms(coder->config->start_time);
		pthread_mutex_unlock(&coder->activity_mutex);

		compiles(coder);

		release_dongle(coder, coder->left);
		release_dongle(coder, coder->right);

		debuges(coder);
		
		refactores(coder);
		i++;
	}
	return (NULL);
}

void	*monitor(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (1)
	{
		if (check_burnout(coder) || check_all_done(coder))
			break ;
		usleep(1000);
	}
	return (NULL);	
}
