/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanilran <fanilran@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/14 13:32:17 by fanilran          #+#    #+#             */
/*   Updated: 2026/09/19 12:09:29 by fanilran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	is_positive(char *str)
{
	int	i;

	i = 0;
	if (!str || str[i] == '\0')
		return (0);
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

static int	check_all_digits(char *argv[])
{
	int	i;

	i = 1;
	while (i <= 7)
	{
		if (!is_positive(argv[i]))
		{
			fprintf(
				stderr,
				"ERROR: The argument '%s' is invalid (not negativ)!\n", argv[i]
				);
			return (0);
		}
		i++;
	}
	if (atoi(argv[1]) == 0)
	{
		fprintf(
			stderr,
			"ERROR: Number of coder must be a positive integer (not 0)!\n"
			);
		return (0);
	}
	return (1);
}

int	pars(t_data *config, int argc, char *argv[])
{
	if (argc != 9)
	{
		fprintf(
			stderr, "Error: The 'len' argument is missing, "
			"or there are too many of the 9 required arguments!\n");
		return (0);
	}
	if (!check_all_digits(argv) || !verify_s(argv[8]))
		return (0);
	config->number_of_coder = atoi(argv[1]);
	config->time_to_burnout = atoi(argv[2]);
	config->time_to_compile = atoi(argv[3]);
	config->time_to_debug = atoi(argv[4]);
	config->time_to_refactor = atoi(argv[5]);
	config->number_of_compiles_required = atoi(argv[6]);
	config->dongle_cooldown = atoi(argv[7]);
	config->scheduler = argv[8];
	return (1);
}
