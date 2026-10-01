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

static int	start_threads(t_coder *coders, pthread_t *threads, int *count)
{
	int	i;

	i = 0;
	while (i < coders->config->number_of_coder)
	{
		if (pthread_create(&threads[i], NULL, routine, &coders[i]) != 0)
			return (0);
		i++;
		*count = i;
	}
	return (1);
}

int	create_threads(t_data *config, t_coder *coders)
{
	pthread_t	*threads;
	pthread_t	monitor_t;
	int			count;
	int			ok;

	threads = malloc(sizeof(pthread_t) * config->number_of_coder);
	if (!threads)
		return (0);
	count = 0;
	config->start_time = get_current_ms();
	ok = start_threads(coders, threads, &count);
	if (ok)
		ok = (pthread_create(&monitor_t, NULL, monitor, coders) == 0);
	if (!ok)
		stop_all(coders, 0);
	while (count > 0)
		pthread_join(threads[--count], NULL);
	if (ok)
		pthread_join(monitor_t, NULL);
	free(threads);
	return (ok);
}
