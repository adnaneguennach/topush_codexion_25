/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aguennac <aguennac@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 16:01:07 by aguennac          #+#    #+#             */
/*   Updated: 2026/07/26 11:46:53 by aguennac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	set_dongles(t_coder *c, t_dongle **first, t_dongle **second)
{
	t_table	*t;
	int		left;
	int		right;

	t = c->table;
	left = c->id - 1;
	right = c->id % t->n_coders;
	if (left < right)
	{
		*first = &t->dongles[left];
		*second = &t->dongles[right];
	}
	else
	{
		*first = &t->dongles[right];
		*second = &t->dongles[left];
	}
}

static void	*lone_coder(t_coder *c, t_dongle *d)
{
	take_dongle(c, d);
	while (!sim_over(c->table))
		usleep(500);
	release_dongle(c->table, d);
	return (NULL);
}

static void	do_compile(t_coder *c)
{
	t_table	*t;

	t = c->table;
	pthread_mutex_lock(&t->state);
	c->last_compile = get_time_ms();
	pthread_mutex_unlock(&t->state);
	log_state(c, "is compiling");
	ms_sleep(t, t->t_compile);
	pthread_mutex_lock(&t->state);
	c->compiles++;
	pthread_mutex_unlock(&t->state);
}

static int	one_cycle(t_coder *c, t_dongle *first, t_dongle *second)
{
	t_table	*t;

	t = c->table;
	if (!take_dongle(c, first))
		return (0);
	if (!take_dongle(c, second))
	{
		release_dongle(t, first);
		return (0);
	}
	do_compile(c);
	release_dongle(t, first);
	release_dongle(t, second);
	log_state(c, "is debugging");
	ms_sleep(t, t->t_debug);
	log_state(c, "is refactoring");
	ms_sleep(t, t->t_refactor);
	return (!sim_over(t));
}

void	*coder_routine(void *arg)
{
	t_coder		*c;
	t_dongle	*first;
	t_dongle	*second;

	c = arg;
	set_dongles(c, &first, &second);
	if (first == second)
		return (lone_coder(c, first));
	while (one_cycle(c, first, second))
		continue ;
	return (NULL);
}
