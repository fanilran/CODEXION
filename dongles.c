/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanilran <fanilran@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 11:14:21 by fanilran          #+#    #+#             */
/*   Updated: 2026/09/18 16:45:31 by fanilran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	take_one(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->lock);
	while (dongle->available != 1)
	{
		if (is_stopped(coder))
			return;
		pthread_cond_wait(&dongle->cond, &dongle->lock);
	}
	dongle->available = 0;
	pthread_mutex_unlock(&dongle->lock);

	if (is_stopped(coder))
		return ;
	log_msg(coder, "has taken a dongle");
}

int	take_dongle(t_coder *coder)
{
	if (coder->id % 2 == 0)
	{
		usleep(1000);
		take_one(coder, coder->left);
		take_one(coder, coder->right);
	}
	else
	{
		take_one(coder, coder->right);
		take_one(coder, coder->left);
	}
	return (1);
}

void	release_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->lock);
	dongle->released_at = get_timestamp_ms(coder->config->start_time);
	pthread_mutex_unlock(&dongle->lock);
	usleep(coder->config->dongle_cooldown * 1000);
	pthread_mutex_lock(&dongle->lock);
	dongle->available = 1;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->lock);
}
