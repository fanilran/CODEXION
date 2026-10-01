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

	if (dongle->available == 0)
		return (0);
	now = get_timestamp_ms(coder->config->start_time);
	return (now - dongle->released_at >= coder->config->dongle_cooldown);
}

static void	wait_turn(t_dongle *dongle, t_coder *coder)
{
	struct timeval	tv;
	struct timespec	ts;
	long			remain;

	remain = coder->config->dongle_cooldown
		- (get_timestamp_ms(coder->config->start_time) - dongle->released_at);
	if (dongle->available == 0 || remain <= 0)
	{
		pthread_cond_wait(&dongle->cond, &dongle->lock);
		return ;
	}
	gettimeofday(&tv, NULL);
	ts.tv_sec = tv.tv_sec + remain / 1000;
	ts.tv_nsec = tv.tv_usec * 1000 + (remain % 1000) * 1000000;
	ts.tv_sec += ts.tv_nsec / 1000000000;
	ts.tv_nsec %= 1000000000;
	pthread_cond_timedwait(&dongle->cond, &dongle->lock, &ts);
}

static int	take_one(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->lock);
	heap_push(dongle, coder);
	while (!is_stopped(coder))
	{
		if (dongle_ready(dongle, coder) && is_front(dongle, coder))
			break ;
		wait_turn(dongle, coder);
	}
	heap_pop(dongle, coder);
	if (is_stopped(coder))
	{
		pthread_mutex_unlock(&dongle->lock);
		return (0);
	}
	dongle->available = 0;
	pthread_mutex_unlock(&dongle->lock);
	log_msg(coder, "has taken a dongle");
	return (1);
}

int	take_dongle(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	first = coder->right;
	second = coder->left;
	if (coder->id % 2 == 0)
	{
		first = coder->left;
		second = coder->right;
	}
	if (!take_one(coder, first))
		return (0);
	if (!take_one(coder, second))
	{
		release_dongle(coder, first);
		return (0);
	}
	return (1);
}

void	release_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->lock);
	dongle->released_at = get_timestamp_ms(coder->config->start_time);
	dongle->available = 1;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->lock);
}
