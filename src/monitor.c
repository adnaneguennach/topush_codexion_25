/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aguennac <aguennac@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 16:01:31 by aguennac          #+#    #+#             */
/*   Updated: 2026/07/25 17:02:39 by aguennac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	check_coder(t_table *t, int i, int *done)
{
	if (get_time_ms() - t->coders[i].last_compile >= t->t_burnout)
	{
		t->over = 1;
		printf("%ld %d burned out\n",
			get_time_ms() - t->start_time, t->coders[i].id);
		return (1);
	}
	if (t->coders[i].compiles < t->must_compile)
		*done = 0;
	return (0);
}

static int	check_end(t_table *t)
{
	int	i;
	int	done;

	pthread_mutex_lock(&t->print);
	pthread_mutex_lock(&t->state);
	done = 1;
	i = 0;
	while (i < t->n_coders)
	{
		if (check_coder(t, i, &done))
		{
			pthread_mutex_unlock(&t->state);
			pthread_mutex_unlock(&t->print);
			return (1);
		}
		i++;
	}
	if (done)
		t->over = 1;
	pthread_mutex_unlock(&t->state);
	pthread_mutex_unlock(&t->print);
	return (done);
}

void	*monitor_routine(void *arg)
{
	t_table	*t;

	t = arg;
	while (!check_end(t))
		usleep(500);
	broadcast_all(t);
	return (NULL);
}
