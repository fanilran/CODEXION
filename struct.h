/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanilran <fanilran@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/07 11:59:04 by fanilran          #+#    #+#             */
/*   Updated: 2026/09/17 13:50:47 by fanilran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCT_H
# define STRUCT_H
# include <pthread.h>

typedef struct s_data
{
	int				number_of_coder;
	int				time_to_burnout;
	int				time_to_compile;
	int				time_to_debug;
	int				time_to_refactor;
	int				number_of_compiles_required;
	int				dongle_cooldown;
	char			*scheduler;
	long			start_time;
	int				stop;
	pthread_mutex_t	stop_mutex;
	pthread_mutex_t	print_mutex;
}	t_data;

typedef struct s_dongle
{
	pthread_mutex_t	lock;
	pthread_cond_t	cond;
	int				available;
	long			released_at;
}	t_dongle;

typedef struct s_coder
{
	int				id;
	pthread_t		*thread;
	t_dongle		*left;
	t_dongle		*right;
	t_data			*config;
	int				compile_done;
	long			last_compile_start;
	pthread_mutex_t	activity_mutex;
}	t_coder;

#endif