/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aguennac <aguennac@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 17:03:17 by aguennac          #+#    #+#             */
/*   Updated: 2026/07/25 17:03:19 by aguennac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <limits.h>

typedef struct s_request
{
	long	key;
	int		coder_id;
}	t_request;

typedef struct s_heap
{
	t_request	*data;
	int			size;
}	t_heap;

typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	int				in_use;
	long			free_at;
	t_heap			queue;
}	t_dongle;

struct	s_table;

typedef struct s_coder
{
	int				id;
	long			compiles;
	long			last_compile;
	pthread_t		thread;
	struct s_table	*table;
}	t_coder;

typedef struct s_table
{
	int				n_coders;
	int				edf;
	int				over;
	long			t_burnout;
	long			t_compile;
	long			t_debug;
	long			t_refactor;
	long			must_compile;
	long			cooldown;
	long			start_time;
	long			ticket;
	pthread_mutex_t	state;
	pthread_mutex_t	print;
	pthread_t		monitor;
	t_dongle		*dongles;
	t_coder			*coders;
}	t_table;

int		parse_args(t_table *t, int argc, char **argv);

int		init_table(t_table *t);
int		start_threads(t_table *t);

long	get_time_ms(void);
void	ms_sleep(t_table *t, long ms);

int		sim_over(t_table *t);
void	log_state(t_coder *c, char *msg);

void	heap_push(t_heap *h, t_request r);
void	heap_remove(t_heap *h, int coder_id);

int		take_dongle(t_coder *c, t_dongle *d);
void	release_dongle(t_table *t, t_dongle *d);

void	*coder_routine(void *arg);

void	*monitor_routine(void *arg);

void	broadcast_all(t_table *t);
int		kill_started(t_table *t, int count);
void	join_all(t_table *t);
void	clean_table(t_table *t);

#endif
