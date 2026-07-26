/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aguennac <aguennac@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 16:01:46 by aguennac          #+#    #+#             */
/*   Updated: 2026/07/26 11:47:06 by aguennac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_dongle(t_table *t, t_dongle *d)
{
	d->in_use = 0;
	d->free_at = 0;
	d->queue.size = 0;
	d->queue.data = malloc(sizeof(t_request) * (t->n_coders + 1));
	if (!d->queue.data)
		return (0);
	pthread_mutex_init(&d->mutex, NULL);
	pthread_cond_init(&d->cond, NULL);
	return (1);
}

static int	init_dongles(t_table *t)
{
	int	i;

	i = 0;
	while (i < t->n_coders)
	{
		if (!init_dongle(t, &t->dongles[i]))
			return (0);
		i++;
	}
	return (1);
}

static void	init_coders(t_table *t)
{
	int	i;

	i = 0;
	while (i < t->n_coders)
	{
		t->coders[i].id = i + 1;
		t->coders[i].compiles = 0;
		t->coders[i].last_compile = t->start_time;
		t->coders[i].table = t;
		i++;
	}
}

int	init_table(t_table *t)
{
	pthread_mutex_init(&t->state, NULL);
	pthread_mutex_init(&t->print, NULL);
	t->over = 0;
	t->ticket = 0;
	t->start_time = get_time_ms();
	t->dongles = malloc(sizeof(t_dongle) * t->n_coders);
	t->coders = malloc(sizeof(t_coder) * t->n_coders);
	if (!t->dongles || !t->coders)
		return (0);
	memset(t->dongles, 0, sizeof(t_dongle) * t->n_coders);
	memset(t->coders, 0, sizeof(t_coder) * t->n_coders);
	init_coders(t);
	if (!init_dongles(t))
		return (0);
	return (1);
}

int	start_threads(t_table *t)
{
	int	i;

	t->start_time = get_time_ms();
	i = 0;
	while (i < t->n_coders)
	{
		t->coders[i].last_compile = t->start_time;
		i++;
	}
	i = 0;
	while (i < t->n_coders)
	{
		if (pthread_create(&t->coders[i].thread, NULL,
				coder_routine, &t->coders[i]) != 0)
			return (kill_started(t, i));
		i++;
	}
	if (pthread_create(&t->monitor, NULL, monitor_routine, t) != 0)
		return (kill_started(t, t->n_coders));
	return (1);
}
