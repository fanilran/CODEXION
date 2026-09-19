/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanilran <fanilran@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/15 21:05:03 by fanilran          #+#    #+#             */
/*   Updated: 2026/09/19 11:45:37 by fanilran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H
# include <stdlib.h>
# include <string.h>
# include <stdio.h>
# include <unistd.h>
# include <sys/time.h>
# include "struct.h"

int		pars(t_data *config, int agrc, char *argv[]);
int		init_coder_dongle(t_data *config, t_coder **coders, t_dongle **dongles);
void	*create_threads(t_data *config, t_coder *coders);
void	*routine(void *arg);
void	*monitor(void *arg);
void	release_dongle(t_coder *coder, t_dongle *dongle);
int     take_dongle(t_coder *coder);
void	compiles(t_coder *coder);
void	debuges(t_coder *coder);
void	refactores(t_coder *coder);
long	get_timestamp_ms(long start_time);
long	get_current_ms();
int     is_stopped(t_coder *coder);
void	log_msg(t_coder *coder, char *msg);
void	wait_ms(t_coder *coder, long time);
int     check_burnout(t_coder *coders);
int     check_all_done(t_coder *coders);

#endif
