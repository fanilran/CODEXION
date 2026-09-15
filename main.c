/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanilran <fanilran@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/16 04:30:25 by fanilran          #+#    #+#             */
/*   Updated: 2026/09/15 11:47:15 by fanilran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char *argv[])
{
	t_data		config;
	t_coder		*coders;
	t_dongle	*dongles;
	int			i;

	if (!pars(&config, argc, (char **)argv))
		return (1);
	if (!init_coder_dongle(&config, &coders, &dongles))
		return (1);
	config.start_time = get_current_ms();
	pthread_mutex_init(&coders->config->stop_mutex, NULL);
	create_threads(&config, coders);
	i = 0;
	while (i < config.number_of_coder)
	{
		pthread_mutex_destroy(&dongles[i].lock);
		pthread_mutex_destroy(&coders[i].activity_mutex);
		pthread_cond_destroy(&dongles[i].cond);
		i++;
	}
	pthread_mutex_destroy(&config.print_mutex);
	free(dongles);
	free(coders);
	return (0);
}
