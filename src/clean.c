/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aguennac <aguennac@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 16:01:14 by aguennac          #+#    #+#             */
/*   Updated: 2026/07/25 17:01:55 by aguennac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	broadcast_all(t_table *t)
{
	int	i;

	i = 0;
	while (i < t->n_coders)
	{
		pthread_mutex_lock(&t->dongles[i].mutex);
		pthread_cond_broadcast(&t->dongles[i].cond);
		pthread_mutex_unlock(&t->dongles[i].mutex);
		i++;
	}
}

int	kill_started(t_table *t, int count)
{
	int	i;

	pthread_mutex_lock(&t->state);
	t->over = 1;
	pthread_mutex_unlock(&t->state);
	broadcast_all(t);
	i = 0;
	while (i < count)
	{
		pthread_join(t->coders[i].thread, NULL);
		i++;
	}
	fprintf(stderr, "Error: pthread_create failed\n");
	return (0);
}

void	join_all(t_table *t)
{
	int	i;

	i = 0;
	while (i < t->n_coders)
	{
		pthread_join(t->coders[i].thread, NULL);
		i++;
	}
	pthread_join(t->monitor, NULL);
}

void	clean_table(t_table *t)
{
	int	i;

	i = 0;
	while (t->dongles && i < t->n_coders)
	{
		if (t->dongles[i].queue.data)
		{
			pthread_mutex_destroy(&t->dongles[i].mutex);
			pthread_cond_destroy(&t->dongles[i].cond);
			free(t->dongles[i].queue.data);
		}
		i++;
	}
	free(t->dongles);
	free(t->coders);
	pthread_mutex_destroy(&t->state);
	pthread_mutex_destroy(&t->print);
}
