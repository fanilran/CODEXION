/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   action.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanilran <fanilran@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/02 02:48:53 by fanilran          #+#    #+#             */
/*   Updated: 2026/10/02 02:48:56 by fanilran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	compiles(t_coder *coder)
{
	if (is_stopped(coder))
		return ;
	pthread_mutex_lock(&coder->activity_mutex);
	coder->compile_done++;
	coder->last_compile = get_timestamp_ms(coder->config->start_time);
	pthread_mutex_unlock(&coder->activity_mutex);
	log_msg(coder, "is compiling");
	wait_ms(coder, coder->config->time_to_compile);
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
