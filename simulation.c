/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanilran <fanilran@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 11:14:21 by fanilran          #+#    #+#             */
/*   Updated: 2026/09/18 14:45:49 by fanilran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	check_burnout(t_coder *coders)
{
	int	i;
	long	d;
	long	n;

	i = 0;
	while (i < coders->config->number_of_coder)
	{
		pthread_mutex_lock(&coders[i].activity_mutex);
		d = coders[i].last_compile_start + coders[i].config->time_to_burnout;
		pthread_mutex_unlock(&coders[i].activity_mutex);
		n = get_timestamp_ms(coders[i].config->start_time);
		if (n > d)
		{
			pthread_mutex_lock(&coders[i].config->stop_mutex);
			coders[i].config->stop = 1;
			pthread_mutex_unlock(&coders[i].config->stop_mutex);
			pthread_mutex_lock(&coders[i].config->print_mutex);
			printf("%ld %d burned out\n", n, coders[i].id);
			pthread_mutex_unlock(&coders[i].config->print_mutex);
			return (1);
		}
		i++;
	}
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

		pthread_mutex_lock(&coder->activity_mutex);
		coder->compile_done = i;
		pthread_mutex_unlock(&coder->activity_mutex);
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
