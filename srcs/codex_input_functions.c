/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codex_input_functions.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsutormi <dsutormi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 18:47:30 by dsutormi          #+#    #+#             */
/*   Updated: 2026/08/25 18:47:44 by dsutormi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codex.h"

static int	ft_validate_args(int argc, char **argv)
{
	int		i;

	if (argc != 9)
		return (1);
	if (strcmp(argv[8], "fifo") && strcmp(argv[8], "edf"))
		return (1);
	argc = 7;
	while (argc > 1)
	{
		i = strlen(argv[argc]) - 1;
		while (i >= 0)
		{
			if (!(argv[argc][i] >= '0' && argv[argc][i] <= '9'))
				return (1);
			if (atoi(argv[argc]) < 0)
				return (1);
			i--;
		}
		argc--;
	}
	if (atoi(argv[1]) < 1)
		return (1);
	return (0);
}

t_settings	*ft_parse_input(int argc, char **argv)
{
	t_settings	*settings;

	settings = malloc(sizeof(t_settings) * 1);
	if (settings == NULL)
		return (NULL);
	pthread_mutex_init(&settings->log_mtx, NULL);
	pthread_mutex_init(&settings->line_mtx, NULL);
	pthread_mutex_init(&settings->etx, NULL);
	pthread_mutex_init(&settings->validate_mtx, NULL);
	pthread_cond_init(&settings->sig, NULL);
	settings->fifo_head = NULL;
	if (ft_validate_args(argc, argv))
		return (free(settings), NULL);
	settings->number_of_coders = atoi(argv[1]);
	settings->time_to_burnout = atoi(argv[2]);
	settings->time_to_compile = atoi(argv[3]);
	settings->time_to_debug = atoi(argv[4]);
	settings->time_to_refactor = atoi(argv[5]);
	settings->number_of_compiles_required = atoi(argv[6]);
	settings->dongle_cooldown = atoi(argv[7]);
	strcpy(settings->scheduler, argv[8]);
	settings->pr_stat = 1;
	return (settings);
}

t_dongle	*ft_init_dongles(t_settings *settings)
{
	t_dongle	*dongles;
	int			i;

	i = 0;
	printf("\033c");
	dongles = malloc(sizeof(t_dongle) * (settings->number_of_coders));
	if (dongles == NULL)
		return (NULL);
	while (i < settings->number_of_coders)
	{
		dongles[i].id = i;
		dongles[i].ms_cld = settings->dongle_cooldown;
		dongles[i].cld_count = settings->dongle_cooldown;
		dongles[i].is_busy = 0;
		pthread_mutex_init(&(dongles[i].mtx), NULL);
		i++;
	}
	return (&dongles[0]);
}

static void	ft_set_coder(t_coder *coder, t_dongle *dongles, int i)
{
	pthread_mutex_init(&(coder->status_mtx), NULL);
	coder->id = i;
	coder->ms_burn = coder->s->time_to_burnout;
	coder->bur_c_ms = 0;
	coder->ms_compile = coder->s->time_to_compile;
	coder->ms_debug = coder->s->time_to_debug;
	coder->ms_refactor = coder->s->time_to_refactor;
	coder->coms_left = coder->s->number_of_compiles_required;
	coder->d_left = &dongles[i];
	coder->d_right = &dongles[(i + 1) % coder->s->number_of_coders];
	coder->log_mtx = &coder->s->log_mtx;
	coder->line_mtx = &coder->s->line_mtx;
	coder->running = 1;
	coder->ready = 2;
	coder->ms_finished = 0;
	coder->ms_refac_started = 0;
	coder->s = coder->s;
}

t_coder	*ft_init_coders(t_settings *settings, t_dongle *dongles)
{
	t_coder	*coders;
	int		i;

	i = -1;
	coders = malloc(sizeof(t_coder) * (settings->number_of_coders + 1));
	if (coders == NULL)
		return (NULL);
	while (++i < settings->number_of_coders)
	{
		coders[i].s = settings;
		ft_set_coder(&coders[i], dongles, i);
	}
	coders[i].id = -1;
	return (&coders[0]);
}
