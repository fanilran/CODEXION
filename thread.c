/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanilran <fanilran@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 12:14:21 by fanilran          #+#    #+#             */
/*   Updated: 2026/09/19 13:18:18 by fanilran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	*create_threads(t_data *config, t_coder *coders)
{
	int			i;
	int			n;
	pthread_t	monitor_t;

	coders->thread = malloc(sizeof(pthread_t) * config->number_of_coder);
	if (!coders->thread)
		return (NULL);
	n = 0;
	while (n < config->number_of_coder)
	{
		if (pthread_create(&coders->thread[n], NULL, routine, &coders[n]))
			break ;
		n++;
	}
	pthread_create(&monitor_t, NULL, monitor, coders);
	i = 0;
	while (i < n)
	{
		pthread_join(coders->thread[i], NULL);
		i++;
	}
	pthread_join(monitor_t, NULL);
	free(coders->thread);
	return (NULL);
}
