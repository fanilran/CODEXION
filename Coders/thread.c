/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   thread.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanilran <fanilran@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/13 14:38:35 by fanilran          #+#    #+#             */
/*   Updated: 2026/10/14 12:15:47 by fanilran         ###   ########.fr       */
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
		pthread_create(&coders->thread[n], NULL, routine, &coders[n]);
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
