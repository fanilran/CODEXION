/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanilran <fanilran@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 15:55:48 by fanilran          #+#    #+#             */
/*   Updated: 2026/10/01 20:10:54 by fanilran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	is_burned(t_coder *coder)
{
	long	now;
	long	last;
	int		done;

	pthread_mutex_lock(&coder->activity_mutex);
	last = coder->last_compile;
	done = coder->compile_done;
	pthread_mutex_unlock(&coder->activity_mutex);
	if (done >= coder->config->compiles_required)
		return (0);
	now = get_timestamp_ms(coder->config->start_time);
	return (now - last > coder->config->time_to_burnout);
}

static void	wake_all(t_coder *coders)
{
	int	i;

	i = 0;
	while (i < coders->config->number_of_coder)
	{
		pthread_mutex_lock(&coders[i].left->lock);
		pthread_cond_broadcast(&coders[i].left->cond);
		pthread_mutex_unlock(&coders[i].left->lock);
		i++;
	}
}

void	stop_all(t_coder *coders, int burned_id)
{
	t_data	*cfg;

	cfg = coders->config;
	pthread_mutex_lock(&cfg->print_mutex);
	pthread_mutex_lock(&cfg->stop_mutex);
	cfg->stop = 1;
	pthread_mutex_unlock(&cfg->stop_mutex);
	if (burned_id > 0)
		printf("%ld %d burned out\n",
			get_timestamp_ms(cfg->start_time), burned_id);
	pthread_mutex_unlock(&cfg->print_mutex);
	wake_all(coders);
}

int	check_burnout(t_coder *coders)
{
	int	i;

	i = 0;
	while (i < coders->config->number_of_coder)
	{
		if (is_burned(&coders[i]))
		{
			stop_all(coders, coders[i].id);
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
		if (done < coders->config->compiles_required)
			return (0);
		i++;
	}
	return (1);
}
