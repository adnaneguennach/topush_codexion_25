/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aguennac <aguennac@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 11:36:48 by aguennac          #+#    #+#             */
/*   Updated: 2026/07/25 16:57:07 by aguennac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static long	get_key(t_coder *c)
{
	long	key;
	t_table	*t;

	t = c->table;
	pthread_mutex_lock(&t->state);
	if (t->edf)
		key = c->last_compile + t->t_burnout;
	else
	{
		key = t->ticket;
		t->ticket++;
	}
	pthread_mutex_unlock(&t->state);
	return (key);
}

static void	sleep_cooldown(t_dongle *d, long now)
{
	long			diff;
	struct timeval	tv;
	struct timespec	ts;

	diff = d->free_at - now;
	gettimeofday(&tv, NULL);
	ts.tv_sec = tv.tv_sec + (diff / 1000);
	ts.tv_nsec = (tv.tv_usec * 1000) + ((diff % 1000) * 1000000);
	if (ts.tv_nsec >= 1000000000)
	{
		ts.tv_sec++;
		ts.tv_nsec -= 1000000000;
	}
	pthread_cond_timedwait(&d->cond, &d->mutex, &ts);
}

static void	wait_for_turn(t_coder *c, t_dongle *d)
{
	long	now;

	while (1)
	{
		if (sim_over(c->table))
			break ;
		if (d->in_use || !d->queue.size
			|| d->queue.data[0].coder_id != c->id)
			pthread_cond_wait(&d->cond, &d->mutex);
		else
		{
			now = get_time_ms();
			if (now < d->free_at)
				sleep_cooldown(d, now);
			else
				break ;
		}
	}
}

int	take_dongle(t_coder *c, t_dongle *d)
{
	t_request	r;

	r.coder_id = c->id;
	r.key = get_key(c);
	pthread_mutex_lock(&d->mutex);
	heap_push(&d->queue, r);
	wait_for_turn(c, d);
	heap_remove(&d->queue, c->id);
	if (sim_over(c->table))
	{
		pthread_mutex_unlock(&d->mutex);
		return (0);
	}
	d->in_use = 1;
	pthread_mutex_unlock(&d->mutex);
	log_state(c, "has taken a dongle");
	return (1);
}

void	release_dongle(t_table *t, t_dongle *d)
{
	pthread_mutex_lock(&d->mutex);
	d->in_use = 0;
	d->free_at = get_time_ms() + t->cooldown;
	pthread_cond_broadcast(&d->cond);
	pthread_mutex_unlock(&d->mutex);
}
