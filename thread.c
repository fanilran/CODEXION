/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanilran <fanilran@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 12:14:21 by fanilran          #+#    #+#             */
/*   Updated: 2026/09/15 12:25:25 by fanilran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	*create_threads(t_data *config, t_coder *coders)
{
	int			i;
	pthread_t	monitor_t;

	coders->thread = malloc(sizeof(pthread_t) * config->number_of_coder);
	if (!coders->thread)
		return (NULL);
	i = 0;
	while (i < config->number_of_coder)
	{
		pthread_create(&coders->thread[i], NULL, routine, &coders[i]);
		i++;
	}
	pthread_create(&monitor_t, NULL, monitor, coders);
	i = 0;
	while (i < config->number_of_coder)
	{
		pthread_join(coders->thread[i], NULL);
		i++;
	}
	pthread_join(monitor_t, NULL);
	free(coders->thread);
	return (NULL);
}
