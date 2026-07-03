#ifndef CODEX_H
#define CODEX_H

#include <stdio.h>  //DELETE
#include <stdint.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

typedef struct settings_s
{
	int		number_of_coders;
	int		time_to_burnout;
	int		time_to_compile;
	int		time_to_debug;
	int		time_to_refactor;
	int		number_of_compiles_required;
	int		dongle_cooldown;
	char	scheduler[5];
}	settings_t;

typedef struct dongle_s
{
	pthread_mutex_t		mtx;
	pthread_cond_t		cond;
	int					id;
	int					ms_cooldown;
	int					is_busy;
}	dongle_t;

typedef struct coder_s
{
	int				id;
	uint32_t		ms_burn;
	uint32_t		burn_counter_ms;
	uint32_t		ms_compile;
	uint32_t		ms_debug;
	uint32_t		ms_refactor;
	uint32_t		comp_counter;
	dongle_t		*d_left;
	dongle_t		*d_right;
	// pthread_mutex_t	*log_mtx;
	pthread_t		thread_id;
	int				running;
}	coder_t;

settings_t	ft_parse_input(int argc, char **argv);
void		ft_log(uint32_t timestamp, uint32_t id, char *message);
void		*ft_handler(void *coder);
void		ft_start(coder_t arr[]);
void		ft_join(coder_t arr[]);
uint32_t	ft_timestamp(void);
void		ft_log(uint32_t timestamp, uint32_t id, char *message);

coder_t	*ft_init_coders(settings_t *settings, dongle_t * dongles);
dongle_t	*ft_init_dongles(settings_t *settings);

void	*ft_start_monitor(void *coders);


#endif