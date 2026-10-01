/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanilran <fanilran@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/02 02:49:56 by fanilran          #+#    #+#             */
/*   Updated: 2026/10/02 02:49:58 by fanilran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	swap_request(t_request *a, t_request *b)
{
	t_request	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

static void	heapify(t_dongle *dongles, char *scheduler)
{
	if (strcmp(scheduler, "fifo") == 0)
	{
		if (dongles->heap[0].create_at > dongles->heap[1].create_at)
			swap_request(&dongles->heap[0], &dongles->heap[1]);
	}
	else if (strcmp(scheduler, "edf") == 0)
	{
		if (dongles->heap[0].deadline > dongles->heap[1].deadline)
			swap_request(&dongles->heap[0], &dongles->heap[1]);
	}
}

void	heap_push(t_dongle *dongle, t_coder *coder)
{
	int	i;

	if (dongle->index >= 2)
		return ;
	i = dongle->index;
	dongle->heap[i].id_coder = coder->id;
	dongle->heap[i].create_at = get_timestamp_ms(coder->config->start_time);
	pthread_mutex_lock(&coder->activity_mutex);
	dongle->heap[i].deadline = coder->last_compile
		+ coder->config->time_to_burnout;
	pthread_mutex_unlock(&coder->activity_mutex);
	dongle->index++;
	if (dongle->index == 2)
		heapify(dongle, coder->config->scheduler);
}

void	heap_pop(t_dongle *dongle, t_coder *coder)
{
	if (dongle->index <= 0)
		return ;
	if (dongle->heap[0].id_coder == coder->id)
	{
		if (dongle->index == 2)
			dongle->heap[0] = dongle->heap[1];
		dongle->index--;
	}
	else if (dongle->index == 2 && dongle->heap[1].id_coder == coder->id)
		dongle->index--;
}

int	is_front(t_dongle *dongle, t_coder *coder)
{
	if (dongle->index == 0)
		return (0);
	return (dongle->heap[0].id_coder == coder->id);
}
