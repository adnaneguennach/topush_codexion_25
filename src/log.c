/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aguennac <aguennac@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 16:01:40 by aguennac          #+#    #+#             */
/*   Updated: 2026/07/25 17:02:28 by aguennac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	sim_over(t_table *t)
{
	int	over;

	pthread_mutex_lock(&t->state);
	over = t->over;
	pthread_mutex_unlock(&t->state);
	return (over);
}

void	log_state(t_coder *c, char *msg)
{
	t_table	*t;

	t = c->table;
	pthread_mutex_lock(&t->print);
	pthread_mutex_lock(&t->state);
	if (!t->over)
		printf("%ld %d %s\n", get_time_ms() - t->start_time, c->id, msg);
	pthread_mutex_unlock(&t->state);
	pthread_mutex_unlock(&t->print);
}
