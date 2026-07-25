/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aguennac <aguennac@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 16:01:26 by aguennac          #+#    #+#             */
/*   Updated: 2026/07/25 17:02:53 by aguennac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	print_usage(void)
{
	fprintf(stderr, "Usage: ./codexion number_of_coders time_to_burnout "
		"time_to_compile time_to_debug time_to_refactor "
		"number_of_compiles_required dongle_cooldown scheduler\n");
	fprintf(stderr, "All numbers must be positive integers, "
		"scheduler must be 'fifo' or 'edf'\n");
	return (0);
}

static int	is_number(char *s)
{
	int	i;

	if (!s[0])
		return (0);
	i = 0;
	while (s[i])
	{
		if (s[i] < '0' || s[i] > '9')
			return (0);
		i++;
	}
	return (i <= 10);
}

static long	to_long(char *s)
{
	long	n;
	int		i;

	n = 0;
	i = 0;
	while (s[i])
	{
		n = n * 10 + (s[i] - '0');
		i++;
	}
	return (n);
}

static int	set_values(t_table *t, char **argv)
{
	long	coders;

	coders = to_long(argv[1]);
	if (coders < 1 || coders > INT_MAX)
		return (0);
	t->n_coders = coders;
	t->t_burnout = to_long(argv[2]);
	t->t_compile = to_long(argv[3]);
	t->t_debug = to_long(argv[4]);
	t->t_refactor = to_long(argv[5]);
	t->must_compile = to_long(argv[6]);
	t->cooldown = to_long(argv[7]);
	if (t->t_burnout > INT_MAX || t->t_compile > INT_MAX
		|| t->t_debug > INT_MAX || t->t_refactor > INT_MAX)
		return (0);
	if (t->must_compile > INT_MAX || t->cooldown > INT_MAX)
		return (0);
	return (1);
}

int	parse_args(t_table *t, int argc, char **argv)
{
	int	i;

	if (argc != 9)
		return (print_usage());
	i = 1;
	while (i <= 7)
	{
		if (!is_number(argv[i]))
			return (print_usage());
		i++;
	}
	if (strcmp(argv[8], "fifo") != 0 && strcmp(argv[8], "edf") != 0)
		return (print_usage());
	t->edf = (strcmp(argv[8], "edf") == 0);
	if (!set_values(t, argv))
		return (print_usage());
	return (1);
}
