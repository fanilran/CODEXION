/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   action.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanilran <fanilran@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/13 14:37:40 by fanilran          #+#    #+#             */
/*   Updated: 2026/10/14 15:34:58 by fanilran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	compiles(t_coder *coder)
{
	if (is_stopped(coder))
		return ;
	pthread_mutex_lock(&coder->activity_mutex);
	coder->last_compile = get_timestamp_ms(coder->config->start_time);
	pthread_mutex_unlock(&coder->activity_mutex);
	log_msg(coder, "is compiling");
	wait_ms(coder, coder->config->time_to_compile);
	pthread_mutex_lock(&coder->activity_mutex);
	coder->compile_done++;
	pthread_mutex_unlock(&coder->activity_mutex);
}

void	debuges(t_coder *coder)
{
	if (is_stopped(coder))
		return ;
	log_msg(coder, "is debugging");
	wait_ms(coder, coder->config->time_to_debug);
}

void	refactores(t_coder *coder)
{
	if (is_stopped(coder))
		return ;
	log_msg(coder, "is refactoring");
	wait_ms(coder, coder->config->time_to_refactor);
}
