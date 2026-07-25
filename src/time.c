/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aguennac <aguennac@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 16:01:20 by aguennac          #+#    #+#             */
/*   Updated: 2026/07/25 16:01:21 by aguennac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((long)tv.tv_sec * 1000 + tv.tv_usec / 1000);
}


void	ms_sleep(t_table *t, long ms)
{
	long	end;

	end = get_time_ms() + ms;
	while (get_time_ms() < end)
	{
		if (sim_over(t))
			return ;
		usleep(200);
	}
}
