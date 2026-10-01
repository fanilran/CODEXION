/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanilran <fanilran@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 11:14:21 by fanilran          #+#    #+#             */
/*   Updated: 2026/10/02 00:35:58 by fanilran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	dongle_ready(t_dongle *dongle, t_coder *coder)
{
	long	now;
	long	time_since_release;

	if (dongle->available == 0)
		return (0);
	now = get_timestamp_ms(coder->config->start_time);
	time_since_release = now - dongle->released_at;
	if (time_since_release < coder->config->dongle_cooldown)
		return (0);
	return (1);
}

static void	take_one(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->lock);
	heap_push(dongle, coder);
	while (!is_stopped(coder))
	{
		if (dongle_ready(dongle, coder) && is_front(dongle, coder))
			break ;
		pthread_cond_wait(&dongle->cond, &dongle->lock);
	}
	if (is_stopped(coder))
	{
		heap_pop(dongle);
		pthread_mutex_unlock(&dongle->lock);
		return ;
	}
	heap_pop(dongle);
	dongle->available = 0;
	pthread_mutex_unlock(&dongle->lock);
	log_msg(coder, "has taken a dongle");
}

int	take_dongle(t_coder *coders)
{
	if (coders->id % 2 == 0)
	{
		take_one(coders, coders->left);
		take_one(coders, coders->right);
	}
	else
	{
		take_one(coders, coders->right);
		take_one(coders, coders->left);
	}
	return (1);
}

void	release_dongle(t_coder *coders, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->lock);
	dongle->released_at = get_timestamp_ms(coders->config->start_time);
	dongle->available = 1;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->lock);
}
