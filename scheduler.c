/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanilran <fanilran@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 16:46:25 by fanilran          #+#    #+#             */
/*   Updated: 2026/10/01 16:43:41 by fanilran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	swap_request(int *a, int *b)
{
	int	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

static void	heap_push(t_request data, t_request **heap, t_dongle *dongle)
{
	t_request	*new_request;

	new_request = malloc(sizeof(t_request));
	if (!new_request)
		return ;
	*new_request = data;
	heap[dongle->index] = new_request;
	dongle->index++;
}

static void	heapify(t_coder *coders, t_dongle *dongles)
{
	if (strcmp(coders->config->scheduler, "fifo") == 0)
	{
		if (dongles->heap[0].create_at > dongles->heap[1].create_at)
			swap_request(&dongles->heap[0], &dongles->heap[1]);
	}
	else if (strcmp(coders->config->scheduler, "edf") == 0)
	{
		if (dongles->heap[0].deadline > dongles->heap[1].deadline)
			swap_request(&dongles->heap[0], &dongles->heap[1]);
	}
}

int request(t_coder *coders, t_dongle *dongle)
{
    t_request   new_request;

    if (!coders || !dongle)
        return (0);
    pthread_mutex_lock(&coders->schedule_mutex);
    new_request.id_coder = coders->id;
    new_request.create_at = get_timestamp_ms(coders->config->start_time);
    new_request.deadline = coders->last_compile
        + coders->config->time_to_burnout;
    pthread_mutex_unlock(&coders->schedule_mutex);
	heap_push(new_request, dongle->heap, dongle);
	if (dongle->index == 2)
		heapify(coders, dongle);
	return (1);
}