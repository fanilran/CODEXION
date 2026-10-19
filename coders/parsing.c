/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanilran <fanilran@student.42antananari    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/13 14:38:04 by fanilran          #+#    #+#             */
/*   Updated: 2026/10/19 16:39:14 by fanilran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int	ft_is_digit(char *str)
{
	int			i;
	long long	nb;

	i = 0;
	nb = 0;
	if (str[i] == '+')
		i++;
	if (!str[i])
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		nb = nb * 10 + (str[i] - '0');
		if (nb > INT_MAX)
			return (0);
		i++;
	}
	return (1);
}

static int	verify_s(char *str)
{
	if (strcmp(str, "fifo") != 0 && strcmp(str, "edf") != 0)
	{
		fprintf(
			stderr, "ERROR: For this argument '%s'"
			", set it to <fifo> or <edf>!\n", str
			);
		return (0);
	}
	return (1);
}

static int	valid_arg(char *argv[])
{
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
				"ERROR: The argument '%s' is invalid "
				"(negative, not an integer, or exceeds INT_MAX)!\n", argv[i]
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
			stderr,
			"Error: expected 8 arguments.\n"
			"Usage: ./codexion number_of_coders time_to_burnout "
			"time_to_compile time_to_debug time_to_refactor "
			"compiles_required dongle_cooldown scheduler\n"
			"Example: ./codexion 5 800 200 200 200 3 50 fifo\n"
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
