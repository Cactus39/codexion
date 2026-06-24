#ifndef CODEX_H
#define CODEX_H

#include <stdio.h>  //DELETE
#include <stdint.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdarg.h>

typedef struct dongle_s
{
	pthread_mutex_t		mtx;
	pthread_cond_t		cond;
	uint32_t			id;
	int					ms_cooldown;
}	dongle_t;

typedef struct coder_s
{
	uint32_t		id;
	uint32_t		ms_burn;
	uint32_t		burn_counter_ms;
	uint32_t		ms_compile;
	uint32_t		ms_debug;
	uint32_t		ms_refactor;
	uint32_t		comp_counter;
	dongle_t		*d_left;
	dongle_t		*d_right;
	pthread_mutex_t	*log_mtx;
	pthread_t		thread_id;
	int				cooldown;
}	coder_t;

void		ft_parse_input(int argc, char **argv);
void		ft_log(uint32_t timestamp, uint32_t id, char *message);
void		*ft_handler(void *coder);
void		ft_start(int n, ...);
void		ft_join(int n, ...);

#endif