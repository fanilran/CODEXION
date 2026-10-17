/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanilran <fanilran@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/13 14:38:04 by fanilran          #+#    #+#             */
/*   Updated: 2026/10/17 20:07:37 by fanilran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	ft_is_digit(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

static int	verify_s(char *str)
{
	if (strcmp(str, "fifo") == 0 || strcmp(str, "edf") == 0)
		return (1);
	fprintf(stderr, "ERROR: For this argument, set it to 'fifo' or 'edf'!\n");
	return (0);
}

static int	valid_arg(char *argv[])
{
	if (
		atoi(argv[2]) == 0 || atoi(argv[2]) > 1000000000
		|| atoi(argv[3]) == 0 || atoi(argv[3]) > 60000
		|| atoi(argv[4]) == 0 || atoi(argv[4]) > 60000
		|| atoi(argv[5]) == 0 || atoi(argv[5]) > 60000
		|| atoi(argv[7]) == 0 || atoi(argv[7]) > 60000
	)
	{
		fprintf(stderr,
			"ERROR: Time to: 'compiling, debuging, refactoring, "
			"donglecooldown' must be a positive integer (not 0 / not > 5000),"
			"and time to 'burned out' not > 1000000000\n"
			);
		return (0);
	}
	if (atoi(argv[1]) == 0 || atoi(argv[1]) > 300)
	{
		fprintf(
			stderr,
			"ERROR: Number of coder must be a "
			"positive integer (not 0 / not > 300)!\n"
			);
		return (0);
	}
	return (1);
}

static int	check_all_digits(char *argv[])
{
	int	i;

	i = 1;
	while (i <= 7)
	{
		if (!ft_is_digit(argv[i]))
		{
			fprintf(
				stderr,
				"ERROR: The argument '%s' is invalid (not negativ)!\n", argv[i]
				);
			return (0);
		}
		i++;
	}
	if (!valid_arg(argv))
		return (0);
	return (1);
}

int	pars(t_data *config, int argc, char *argv[])
{
	if (argc != 9)
	{
		fprintf(
			stderr, "Error: The 'len' argument is missing, "
			"or there are too many of the 9 required arguments!\n"
			);
		return (0);
	}
	if (!check_all_digits(argv) || !verify_s(argv[8]))
		return (0);
	config->number_of_coder = atoi(argv[1]);
	config->time_to_burnout = atoi(argv[2]);
	config->time_to_compile = atoi(argv[3]);
	config->time_to_debug = atoi(argv[4]);
	config->time_to_refactor = atoi(argv[5]);
	config->compiles_required = atoi(argv[6]);
	config->dongle_cooldown = atoi(argv[7]);
	config->scheduler = argv[8];
	return (1);
}
