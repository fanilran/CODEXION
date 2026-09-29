/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fanilran <fanilran@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/29 16:46:25 by fanilran          #+#    #+#             */
/*   Updated: 2026/09/29 17:59:38 by fanilran         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int request(t_coder *coders, t_dongle dongle)
{
    t_request   new_request;
    (void)dongle;

    pthread_mutex_lock(&coders->schedule_mutex);
    new_request.id_coder = coders->id;
    new_request.create_at = get_timestamp_ms(coders->config->start_time);
    new_request.deadline = coders->last_compile + coders->config->time_to_burnout;
    pthread_mutex_unlock(&coders->schedule_mutex);
    return (1);
}