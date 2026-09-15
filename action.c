/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   action.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanilran <fanilran@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 15:01:17 by fanilran          #+#    #+#             */
/*   Updated: 2026/09/15 16:22:22 by fanilran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	compiles(t_coder *coder)
{
	long	timestamp;
	long	start;
	
	if (coder->config->stop)
		return;
	pthread_mutex_lock(&coder->config->print_mutex);
	timestamp = get_timestamp_ms(coder->config->start_time);
	printf("%ld %d is compiling\n", timestamp, coder->id);
	pthread_mutex_unlock(&coder->config->print_mutex);
	start = get_current_ms();
	while (!coder->config->stop && get_current_ms() - start <= coder->config->time_to_compile)
	{
		usleep(1000);
	}
	// usleep(coder->config->time_to_compile * 1000);
}

void	debuges(t_coder *coder)
{
	long	timestamp;
	long		start;

	pthread_mutex_lock(&coder->config->print_mutex);
	timestamp = get_timestamp_ms(coder->config->start_time);
	printf("%ld %d is debugging\n", timestamp, coder->id);
	pthread_mutex_unlock(&coder->config->print_mutex);
	start = get_current_ms();
	while (!coder->config->stop && get_current_ms() - start <= coder->config->time_to_compile)
	{
		usleep(1000);
	}
	// usleep(coder->config->time_to_debug * 1000);
}

void	refactores(t_coder *coder)
{
	long	timestamp;
	long		start;

	pthread_mutex_lock(&coder->config->print_mutex);
	timestamp = get_timestamp_ms(coder->config->start_time);
	printf("%ld %d is refactoring\n", timestamp, coder->id);
	pthread_mutex_unlock(&coder->config->print_mutex);
	start = get_current_ms();
	while (!coder->config->stop && get_current_ms() - start <= coder->config->time_to_compile)
	{
		usleep(1000);
	}
	// usleep(coder->config->time_to_refactor * 1000);
}
