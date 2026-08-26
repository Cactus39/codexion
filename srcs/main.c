/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsutormi <dsutormi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 18:47:38 by dsutormi          #+#    #+#             */
/*   Updated: 2026/08/25 18:47:40 by dsutormi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

static void	ft_start(t_coder coders[])
{
	pthread_t	thread_id;
	int			i;

	i = 0;
	while (coders[i].id != -1)
	{
		pthread_create(&thread_id, NULL, ft_handler, (void *)(&coders[i]));
		coders[i].thread_id = thread_id;
		i++;
	}
}

static void	ft_join(t_coder coders[])
{
	int			i;

	i = 0;
	while (coders[i].id != -1)
	{
		pthread_join(coders[i].thread_id, NULL);
		i++;
	}
}

static void	ft_free_tree(t_node *head)
{
	if (head == NULL)
		return ;
	ft_free_tree(head->left);
	if (head->left)
		free(head->left);
	ft_free_tree(head->right);
	if (head->right)
		free(head->right);
	if (head->parent == NULL)
		free(head);
}

int	ft_validate(t_coder *coder)
{
	uint32_t	ts;

	ts = ft_timestamp();
	pthread_mutex_lock(&coder->s->validate_mtx);
	if (coder->d_left->is_busy || coder->d_right->is_busy)
	{
		pthread_mutex_unlock(&coder->s->validate_mtx);
		return (0);
	}
	if ((ts - coder->d_left->cld_count < coder->d_left->ms_cld))
	{
		pthread_mutex_unlock(&coder->s->validate_mtx);
		return (0);
	}
	if ((ts - coder->d_right->cld_count < coder->d_right->ms_cld))
	{
		pthread_mutex_unlock(&coder->s->validate_mtx);
		return (0);
	}
	coder->d_left->is_busy = 1;
	coder->d_right->is_busy = 1;
	pthread_mutex_unlock(&coder->s->validate_mtx);
	return (1);
}

int	main(int argc, char **argv)
{
	pthread_t		mon_fif_fif2[3];
	t_settings		*settings;
	t_coder			*coders;
	t_dongle		*dongles;

	coders = NULL;
	dongles = NULL;
	settings = ft_parse_input(argc, argv);
	if (settings == NULL)
		return (0);
	dongles = ft_init_dongles(settings);
	coders = ft_init_coders(settings, dongles);
	ft_start(coders);
	pthread_create(&mon_fif_fif2[1], NULL, ft_run_fifo, (void *)coders);
	pthread_create(&mon_fif_fif2[2], NULL, ft_run_fifo_2, (void *)coders);
	pthread_create(&mon_fif_fif2[0], NULL, ft_start_monitor, (void *)coders);
	ft_join(coders);
	pthread_join(mon_fif_fif2[1], NULL);
	pthread_join(mon_fif_fif2[2], NULL);
	pthread_join(mon_fif_fif2[0], NULL);
	ft_free_tree(settings->fifo_head);
	free(coders);
	free(dongles);
	free(settings);
	return (0);
}
