/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanilran <fanilran@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/11 11:14:21 by fanilran          #+#    #+#             */
/*   Updated: 2026/09/25 11:54:21 by fanilran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	*routine(void *arg)
{
	t_coder	*coder;
	int		i;

	coder = (t_coder *)arg;
	i = 0;
	while (i < coder->config->compiles_required)
	{
		take_dongle(coder);
		compiles(coder);
		debuges(coder);
		refactores(coder);
		i++;
	}
	return (NULL);
}

void	*monitor(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (1)
	{
		if (check_burnout(coder) || check_all_done(coder))
			break ;
		usleep(1000);
	}
	return (NULL);
}
