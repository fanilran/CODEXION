/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   useful.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanilran <fanilran@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 15:55:48 by fanilran          #+#    #+#             */
/*   Updated: 2026/09/18 15:55:50 by fanilran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_stopped(t_coder *coder)
{
	int	stopped;

	pthread_mutex_lock(&coder->config->stop_mutex);
	stopped = coder->config->stop;
	pthread_mutex_unlock(&coder->config->stop_mutex);
	return (stopped);
}

void	log_msg(t_coder *coder, char *msg)
{
	long	timestamp;

	pthread_mutex_lock(&coder->config->print_mutex);
	timestamp = get_timestamp_ms(coder->config->start_time);
	printf("%ld %d %s\n", timestamp, coder->id, msg);
	pthread_mutex_unlock(&coder->config->print_mutex);
}

void	wait_ms(t_coder *coder, long time)
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