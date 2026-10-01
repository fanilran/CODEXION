/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanilran <fanilran@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/16 04:30:25 by fanilran          #+#    #+#             */
/*   Updated: 2026/09/29 12:29:45 by fanilran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char *argv[])
{
	t_data		config;
	t_coder		*coders;
	t_dongle	*dongles;
	int			ok;

	if (!pars(&config, argc, argv))
		return (1);
	if (!init_coder_dongle(&config, &coders, &dongles))
		return (1);
	ok = create_threads(&config, coders);
	destroy_all(&config, coders, dongles);
	return (!ok);
}
