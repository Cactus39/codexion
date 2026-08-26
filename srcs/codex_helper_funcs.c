/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codex_helper_funcs.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsutormi <dsutormi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 18:47:28 by dsutormi          #+#    #+#             */
/*   Updated: 2026/08/25 18:47:45 by dsutormi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

void	ft_log(t_coder *coder)
{
	int		id;

	if (coder->running == 0)
		return ;
	id = coder->id + 1;
	pthread_mutex_lock(coder->log_mtx);
	printf("%u %d has taken a dongle\n", coder->ms_l, id);
	printf("%u %d has taken a dongle\n", coder->ms_r, id);
	printf("%u %d is compiling\n", coder->bur_c_ms, id);
	printf("%u %d is debugging\n", coder->d_left->cld_count, id);
	printf("%u %d is refactoring\n\n", coder->ms_refac_started, id);
	pthread_mutex_unlock(coder->log_mtx);
}

uint32_t	ft_timestamp(void)
{
	static long long	start_ms;
	static int			flag;
	struct timeval		tv;

	if (!flag && gettimeofday(&tv, 0) == 0)
	{
		flag = 1;
		start_ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	}
	if (gettimeofday(&tv, 0) == 0)
		return ((tv.tv_sec * 1000 + tv.tv_usec / 1000) - start_ms);
	return (0);
}

static void	*ft_stop_monitor(t_coder *coders)
{
	int		i;

	i = 0;
	while (coders[i].id != -1)
	{
		pthread_mutex_lock(&coders[i].s->etx);
		coders[i].running = 0;
		pthread_mutex_unlock(&coders[i].s->etx);
		i++;
	}
	pthread_mutex_lock(coders[0].log_mtx);
	return (NULL);
}

static int	ft_check_coder(void *coders_v)
{
	int			i;
	int			cleft;
	t_coder		*cdr;
	uint32_t	ts;

	cdr = (t_coder *)coders_v;
	i = 0;
	cleft = 0;
	usleep(1000);
	while (cdr[i].id != -1)
	{
		ts = ft_timestamp();
		pthread_mutex_lock(&(cdr->s->etx));
		if ((ts - cdr[i].bur_c_ms) > cdr[i].ms_burn && cdr[i].coms_left)
			return (pthread_mutex_unlock(&(cdr->s->etx)), i + 1);
		cleft += cdr[i].coms_left;
		pthread_mutex_unlock(&(cdr->s->etx));
		i++;
	}
	if (cleft)
		return (0);
	return (-1);
}

void	*ft_start_monitor(void *coders_v)
{
	uint32_t	ts;
	t_coder		*cdr;
	int			flag;

	flag = 0;
	cdr = (t_coder *)coders_v;
	while (!(flag))
		flag = ft_check_coder(cdr);
	ts = ft_timestamp();
	cdr[0].s->pr_stat = 0;
	pthread_cond_broadcast(&cdr[0].s->sig);
	if (flag != -1)
	{
		ft_stop_monitor(cdr);
		printf("\n%u %d burned out\n", ts, flag);
		pthread_mutex_unlock(cdr[0].log_mtx);
		pthread_cond_broadcast(&cdr[0].s->sig);
	}
	else
	{
		usleep((cdr[0].s->time_to_compile + cdr[0].s->time_to_debug) * 1000);
		usleep((cdr[0].s->dongle_cooldown + cdr[0].s->time_to_refactor) * 1000);
		printf("\nAll coders finished successfully\n");
	}
	return (NULL);
}
