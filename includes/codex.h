/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dsutormi <dsutormi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/25 18:47:01 by dsutormi          #+#    #+#             */
/*   Updated: 2026/08/25 18:47:06 by dsutormi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEX_H
# define CODEX_H

# include <stdio.h>
# include <stdint.h>
# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>
# include <stdlib.h>
# include <string.h>

typedef struct s_node
{
	uint32_t		value;
	int				id;
	struct s_node	*parent;
	struct s_node	*left;
	struct s_node	*right;
}	t_node;

typedef struct s_settings
{
	int					number_of_coders;
	int					time_to_burnout;
	int					time_to_compile;
	int					time_to_debug;
	int					time_to_refactor;
	int					number_of_compiles_required;
	int					dongle_cooldown;
	char				scheduler[5];
	pthread_mutex_t		line_mtx;
	pthread_mutex_t		log_mtx;
	pthread_mutex_t		validate_mtx;
	pthread_mutex_t		etx;
	pthread_cond_t		sig;
	int					pr_stat;
	t_node				*fifo_head;
}	t_settings;

typedef struct s_dongle
{
	pthread_mutex_t		mtx;
	int					id;
	uint32_t			ms_cld;
	int					cld_count;
	int					is_busy;
}	t_dongle;

typedef struct s_coder
{
	int				id;
	uint32_t		ms_burn;
	uint32_t		bur_c_ms;
	uint32_t		ms_compile;
	uint32_t		ms_l;
	uint32_t		ms_r;
	uint32_t		ms_debug;
	uint32_t		ms_refactor;
	uint32_t		coms_left;
	uint32_t		ms_refac_started;
	uint32_t		ms_finished;
	t_dongle		*d_left;
	t_dongle		*d_right;
	pthread_mutex_t	*log_mtx;
	pthread_mutex_t	*line_mtx;
	pthread_mutex_t	status_mtx;
	pthread_t		thread_id;
	uint8_t			ready;
	int				running;
	t_settings		*s;
}	t_coder;

t_settings	*ft_parse_input(int argc, char **argv);
void		ft_log(t_coder *coder);
void		*ft_handler(void *coder);
uint32_t	ft_timestamp(void);
t_coder		*ft_init_coders(t_settings *settings, t_dongle *dongles);
t_dongle	*ft_init_dongles(t_settings *settings);
void		*ft_start_monitor(void *coders);
void		ft_append_tree(t_node **head, t_node *node);
t_node		*ft_delrestruct_tree(t_node **tree, t_node *node_to_del);
void		*ft_run_fifo_2(void *coders);
void		*ft_run_fifo(void *coders);
int			ft_validate(t_coder *coder);
#endif
