/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   action.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanilran <fanilran@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 15:01:17 by fanilran          #+#    #+#             */
/*   Updated: 2026/09/18 16:49:35 by fanilran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	compiles(t_coder *coder)
{
	if (is_stopped(coder))
	{
		
		return ;
	}
	log_msg(coder, "is compiling");
	pthread_mutex_lock(&coder->activity_mutex);
	coder->compile_done++;
	pthread_mutex_unlock(&coder->activity_mutex);
	wait_ms(coder, coder->config->time_to_compile);
}

void	debuges(t_coder *coder)
{
	if (is_stopped(coder))
		return ;
	log_msg(coder, "is debugging");
	wait_ms(coder, coder->config->time_to_compile);
}

void	refactores(t_coder *coder)
{
	if (is_stopped(coder))
		return ;
	log_msg(coder, "is refactoring");
	wait_ms(coder, coder->config->time_to_compile);
}
