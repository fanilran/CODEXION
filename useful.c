/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   useful.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanilran <fanilran@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 15:55:48 by fanilran          #+#    #+#             */
/*   Updated: 2026/09/25 12:22:01 by fanilran         ###   ########.fr       */
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

int	check_burnout(t_coder *coders)
{
	int		i;
	long	deadline;
	long	now;
	int		done;

	i = 0;
	while (i < coders->config->number_of_coder)
	{
		pthread_mutex_lock(&coders[i].activity_mutex);
		deadline = coders[i].last_compile + coders[i].config->time_to_burnout;
		done = coders[i].compile_done;
		pthread_mutex_unlock(&coders[i].activity_mutex);
		now = get_timestamp_ms(coders[i].config->start_time);
		if (now > deadline && done < coders->config->compiles_required)
		{
			pthread_mutex_lock(&coders[i].config->stop_mutex);
			coders[i].config->stop = 1;
			pthread_mutex_unlock(&coders[i].config->stop_mutex);
			pthread_cond_broadcast(&coders->left->cond);
			pthread_cond_broadcast(&coders->right->cond);
			log_msg(&coders[i], "burned out");
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
