/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aguennac <aguennac@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/25 16:01:36 by aguennac          #+#    #+#             */
/*   Updated: 2026/07/26 11:47:11 by aguennac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char **argv)
{
	t_table	t;

	if (!parse_args(&t, argc, argv))
		return (1);
	if (!init_table(&t))
	{
		fprintf(stderr, "Error: allocation failed\n");
		clean_table(&t);
		return (1);
	}
	if (!start_threads(&t))
	{
		clean_table(&t);
		return (1);
	}
	join_all(&t);
	clean_table(&t);
	return (0);
}
