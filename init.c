/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanilran <fanilran@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 11:14:29 by fanilran          #+#    #+#             */
/*   Updated: 2026/09/04 13:36:30 by fanilran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	destroy_all(t_data *config, t_coder *coders, t_dongle *dongles)
{
	int	i;

	i = 0;
	while (i < config->number_of_coder)
	{
		if (dongles)
		{
			pthread_mutex_destroy(&dongles[i].lock);
			pthread_cond_destroy(&dongles[i].cond);
		}
		if (coders)
			pthread_mutex_destroy(&coders[i].activity_mutex);
		i++;
	}
	pthread_mutex_destroy(&config->print_mutex);
	pthread_mutex_destroy(&config->stop_mutex);
	free(dongles);
	free(coders);
}

static int	init_dongles(t_data *config, t_dongle **dongles)
{
	int	i;

	*dongles = malloc(sizeof(t_dongle) * config->number_of_coder);
	if (!*dongles)
		return (0);
	i = 0;
	while (i < config->number_of_coder)
	{
		pthread_mutex_init(&(*dongles)[i].lock, NULL);
		pthread_cond_init(&(*dongles)[i].cond, NULL);
		(*dongles)[i].available = 1;
		(*dongles)[i].released_at = -config->dongle_cooldown;
		(*dongles)[i].index = 0;
		i++;
	}
	return (1);
}

static int	init_coders(t_data *config, t_coder **coders, t_dongle **dongles)
{
	int	i;

	*coders = malloc(sizeof(t_coder) * config->number_of_coder);
	if (!*coders)
		return (0);
	i = 0;
	while (i < config->number_of_coder)
	{
		pthread_mutex_init(&(*coders)[i].activity_mutex, NULL);
		(*coders)[i].config = config;
		(*coders)[i].id = i + 1;
		(*coders)[i].compile_done = 0;
		(*coders)[i].last_compile = 0;
		(*coders)[i].left = &(*dongles)[i];
		(*coders)[i].right = &(*dongles)[(i + 1) % config->number_of_coder];
		i++;
	}
	return (1);
}

int	init_coder_dongle(t_data *config, t_coder **coders, t_dongle **dongles)
{
	*coders = NULL;
	*dongles = NULL;
	config->stop = 0;
	pthread_mutex_init(&config->print_mutex, NULL);
	pthread_mutex_init(&config->stop_mutex, NULL);
	if (!init_dongles(config, dongles) || !init_coders(config, coders, dongles))
	{
		destroy_all(config, *coders, *dongles);
		return (0);
	}
	config->start_time = get_current_ms();
	return (1);
}
