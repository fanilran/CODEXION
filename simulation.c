/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanilran <fanilran@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 11:14:21 by fanilran          #+#    #+#             */
/*   Updated: 2026/10/01 20:34:03 by fanilran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	*routine(void *arg)
{
	t_coder	*coder;
	int		i;

	coder = (t_coder *)arg;
	i = 0;
	while (i < coder->config->compiles_required && !is_stopped(coder))
	{
		if (!take_dongle(coder))
			break ;
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
	int		i;

	coder = (t_coder *)arg;
	while (1)
	{
		if (check_burnout(coder) || check_all_done(coder))
			break ;
		usleep(1000);
	}
	i = 0;
	while (i < coder->config->number_of_coder)
	{
		pthread_mutex_lock(&coder[i].left->lock);
		pthread_cond_broadcast(&coder[i].left->cond);
		pthread_mutex_unlock(&coder[i].left->lock);
		i++;
	}
	return (NULL);
}
