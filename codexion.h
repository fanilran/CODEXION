/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanilran <fanilran@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/15 21:05:03 by fanilran          #+#    #+#             */
/*   Updated: 2026/10/02 02:12:23 by fanilran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H
# include <stdlib.h>
# include <string.h>
# include <stdio.h>
# include <unistd.h>
# include <time.h>
# include <limits.h>
# include <sys/time.h>
# include "struct.h"

int		pars(t_data *config, int argc, char *argv[]);
int		init_coder_dongle(t_data *config, t_coder **coders, t_dongle **dongles);
void	destroy_all(t_data *config, t_coder *coders, t_dongle *dongles);
int		create_threads(t_data *config, t_coder *coders);
void	*routine(void *arg);
void	*monitor(void *arg);
int		take_dongle(t_coder *coder);
void	release_dongle(t_coder *coder, t_dongle *dongle);
void	compiles(t_coder *coder);
void	debuges(t_coder *coder);
void	refactores(t_coder *coder);
long	get_timestamp_ms(long start_time);
long	get_current_ms(void);
int		is_stopped(t_coder *coder);
void	log_msg(t_coder *coder, char *msg);
void	wait_ms(t_coder *coder, long time);
void	stop_all(t_coder *coders, int burned_id);
int		check_burnout(t_coder *coders);
int		check_all_done(t_coder *coders);
void	heap_push(t_dongle *dongle, t_coder *coder);
void	heap_pop(t_dongle *dongle, t_coder *coder);
int		is_front(t_dongle *dongle, t_coder *coder);

#endif
