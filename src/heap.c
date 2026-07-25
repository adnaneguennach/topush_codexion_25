/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aguennac <aguennac@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 16:01:53 by aguennac          #+#    #+#             */
/*   Updated: 2026/07/25 16:01:53 by aguennac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"


static int	req_less(t_request a, t_request b)
{
	if (a.key != b.key)
		return (a.key < b.key);
	return (a.coder_id < b.coder_id);
}

static void	sift_up(t_heap *h, int i)
{
	t_request	tmp;

	while (i > 0 && req_less(h->data[i], h->data[(i - 1) / 2]))
	{
		tmp = h->data[i];
		h->data[i] = h->data[(i - 1) / 2];
		h->data[(i - 1) / 2] = tmp;
		i = (i - 1) / 2;
	}
}

static void	sift_down(t_heap *h, int i)
{
	t_request	tmp;
	int			child;

	while (2 * i + 1 < h->size)
	{
		child = 2 * i + 1;
		if (child + 1 < h->size
			&& req_less(h->data[child + 1], h->data[child]))
			child++;
		if (!req_less(h->data[child], h->data[i]))
			return ;
		tmp = h->data[i];
		h->data[i] = h->data[child];
		h->data[child] = tmp;
		i = child;
	}
}

void	heap_push(t_heap *h, t_request r)
{
	h->data[h->size] = r;
	h->size++;
	sift_up(h, h->size - 1);
}

void	heap_remove(t_heap *h, int coder_id)
{
	int	i;

	i = 0;
	while (i < h->size && h->data[i].coder_id != coder_id)
		i++;
	if (i == h->size)
		return ;
	h->size--;
	h->data[i] = h->data[h->size];
	sift_down(h, i);
	sift_up(h, i);
}
