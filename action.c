/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   action.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanilran <fanilran@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 15:01:17 by fanilran          #+#    #+#             */
/*   Updated: 2026/09/18 14:45:36 by fanilran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	is_stopped(t_coder *coder)
{
	int	stopped;

	pthread_mutex_lock(&coder->config->stop_mutex);
	stopped = coder->config->stop;
	pthread_mutex_unlock(&coder->config->stop_mutex);
	return (stopped);
}


static void	wait_ms(t_coder *coder, long time)
{
	long	start;

	start = get_current_ms();
	while (get_current_ms() - start < time)
	{
		if (is_stopped(coder))
			return ;
		usleep(200);
	}
}


void	compiles(t_coder *coder)
{
	long	timestamp;

	if (is_stopped(coder))
		return ;
	pthread_mutex_lock(&coder->config->print_mutex);
	timestamp = get_timestamp_ms(coder->config->start_time);
	printf("%ld %d is compiling\n", timestamp, coder->id);
	pthread_mutex_unlock(&coder->config->print_mutex);
	wait_ms(coder, coder->config->time_to_compile);
}

void	debuges(t_coder *coder)
{
	long	timestamp;

	if (is_stopped(coder))
		return ;
	pthread_mutex_lock(&coder->config->print_mutex);
	timestamp = get_timestamp_ms(coder->config->start_time);
	printf("%ld %d is debugging\n", timestamp, coder->id);
	pthread_mutex_unlock(&coder->config->print_mutex);
	wait_ms(coder, coder->config->time_to_compile);
}

void	refactores(t_coder *coder)
{
	long	timestamp;

	if (is_stopped(coder))
		return ;
	pthread_mutex_lock(&coder->config->print_mutex);
	timestamp = get_timestamp_ms(coder->config->start_time);
	printf("%ld %d is refactoring\n", timestamp, coder->id);
	pthread_mutex_unlock(&coder->config->print_mutex);
	wait_ms(coder, coder->config->time_to_compile);
}
