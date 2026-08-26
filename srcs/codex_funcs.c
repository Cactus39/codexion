/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codex_funcs.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsutormi <dsutormi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 18:47:25 by dsutormi          #+#    #+#             */
/*   Updated: 2026/08/25 18:47:46 by dsutormi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

static void	ft_start_compile(void *c)
{
	pthread_mutex_lock(&(((t_coder *)c)->status_mtx));
	pthread_mutex_lock(&((t_coder *)c)->d_left->mtx);
	((t_coder *)c)->ms_l = ft_timestamp();
	if (&((t_coder *)c)->d_right->mtx == &((t_coder *)c)->d_left->mtx)
	{
		pthread_mutex_unlock(&((t_coder *)c)->s->etx);
		pthread_mutex_unlock(&((t_coder *)c)->d_left->mtx);
		pthread_cancel(((t_coder *)c)->thread_id);
	}
	pthread_mutex_lock(&((t_coder *)c)->d_right->mtx);
	((t_coder *)c)->ms_r = ft_timestamp();
	((t_coder *)c)->bur_c_ms = ft_timestamp();
	((t_coder *)c)->coms_left--;
	pthread_mutex_unlock(&((t_coder *)c)->s->etx);
	usleep(((t_coder *)c)->ms_compile * 1000);
}

static void	ft_finish_compile(void *c)
{
	pthread_mutex_lock(&((t_coder *)c)->s->validate_mtx);
	((t_coder *)c)->d_right->cld_count = ft_timestamp();
	((t_coder *)c)->d_right->is_busy = 0;
	((t_coder *)c)->d_left->is_busy = 0;
	((t_coder *)c)->d_left->cld_count = ft_timestamp() + 1;
	pthread_mutex_lock(&((t_coder *)c)->s->etx);
	pthread_mutex_unlock(&((t_coder *)c)->d_left->mtx);
	pthread_mutex_unlock(&((t_coder *)c)->d_right->mtx);
	pthread_mutex_unlock(&((t_coder *)c)->s->etx);
	pthread_mutex_unlock(&((t_coder *)c)->s->validate_mtx);
	usleep(((t_coder *)c)->ms_debug * 1000);
	((t_coder *)c)->ms_refac_started = ft_timestamp();
	usleep(((t_coder *)c)->ms_refactor * 1000);
}

void	*ft_handler(void *c)
{
	while (((t_coder *)c)->coms_left && ((t_coder *)c)->running)
	{
		pthread_mutex_lock(&((t_coder *)c)->s->etx);
		while ((((t_coder *)c)->ready != 1))
		{
			pthread_cond_wait(&((t_coder *)c)->s->sig, &((t_coder *)c)->s->etx);
			if (((t_coder *)c)->running == 0 || ((t_coder *)c)->s->pr_stat == 0)
				break ;
		}
		if (((t_coder *)c)->running == 0 || ((t_coder *)c)->s->pr_stat == 0)
		{
			pthread_mutex_unlock(&((t_coder *)c)->s->etx);
			break ;
		}
		ft_start_compile(c);
		ft_finish_compile(c);
		if (((t_coder *)c)->coms_left > 0 && ((t_coder *)c)->running)
			((t_coder *)c)->ready = 2;
		else
			((t_coder *)c)->ready = 3;
		ft_log(((t_coder *)c));
		((t_coder *)c)->ms_finished = ft_timestamp();
		pthread_mutex_unlock(&(((t_coder *)c)->status_mtx));
	}
	return (NULL);
}
