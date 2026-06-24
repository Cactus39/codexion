#ifndef CODEX_H
#define CODEX_H

#include <stdio.h>  //DELETE
#include <stdint.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

typedef struct dongle_s
{
	pthread_mutex_t		mtx;
	pthread_cond_t		cond;
	uint32_t			ms_cooldown;
}	dongle_t;

typedef struct coder_s
{
	uint32_t		id;
	uint32_t		ms_burn;
	uint32_t		ms_compile;
	uint32_t		ms_debug;
	uint32_t		ms_refactor;
	uint32_t		comp_counter;
	dongle_t	*d_left;
	dongle_t	*d_right;
}	coder_t;

void		ft_parse_input(int argc, char **argv);
void		*ft_handler(void *coder);
pthread_t	ft_loop(coder_t *coder);



#endif