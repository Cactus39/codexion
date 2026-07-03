#include "codex.h"

int		ft_validate_dongle(coder_t *coder)
{
	uint32_t	ts;

	ts = ft_timestamp();
	if (coder->d_left->is_busy || coder->d_right->is_busy)
		return(0);
	if ((ts - coder->d_left->cooldown_counter > coder->d_left->ms_cooldown) &&
	(ts - coder->d_right->cooldown_counter > coder->d_right->ms_cooldown))
		return (1);
	return(0);
}

void	*ft_handler(void *coder_v)
{
	coder_t		*coder;
	uint32_t	timestamps[3];
	uint32_t	timestamp;
	coder = (coder_t *)coder_v;
	while (coder->comp_counter && coder->running)
	{
		timestamp = ft_timestamp();
		// if ((!(timestamp - coder->d_left->ms_cooldown >= coder->cooldown)) || 
		// (!(timestamp - coder->d_right->ms_cooldown >= coder->cooldown)))
			// continue ;
		// if (!ft_validate_dongle(coder))
			// continue ;
		pthread_mutex_lock(&coder->d_left->mtx);
		pthread_mutex_lock(&coder->d_right->mtx);
		coder->d_left->is_busy = 1;
		coder->d_right->is_busy = 1;
		// printf("Coder [%d] takes dongles [%d] and [%d]\n", coder->id, coder->d_left->id, coder->d_right->id);
		coder->comp_counter--;
		timestamps[0] = ft_timestamp(); 				//compile
		usleep(coder->ms_compile * 1000);
		coder->burn_counter_ms = timestamps[0];
		// printf("Coder [%d] relases dongles [%d] and [%d]\n", coder->id, coder->d_left->id, coder->d_right->id);
		pthread_mutex_unlock(&coder->d_left->mtx);
		pthread_mutex_unlock(&coder->d_right->mtx);
		coder->d_left->is_busy = 0;
		coder->d_right->is_busy = 0;
		timestamps[1] = ft_timestamp();					//debug
		coder->d_left->cooldown_counter = timestamps[1];
		coder->d_right->cooldown_counter = timestamps[1];
		usleep(coder->ms_debug * 1000);
		timestamps[2] = ft_timestamp();					//refactor
		usleep(coder->ms_refactor * 1000);
		ft_log(timestamps[0], coder->id, "is compiling");
		ft_log(timestamps[1], coder->id, "is debugging");
		ft_log(timestamps[2], coder->id, "is refactoring");
	}
	return (NULL);
}

void	*ft_stop_monitor(coder_t* coders)
{
	int		i;

	i = 0;
	while (coders[i].id != -1)
	{
		coders[i].running = 0;
		i++;
	}
}

void	*ft_start_monitor(void *coders_v)
{
	pthread_t	monitor_id;
	int			i;
	uint32_t	ts;
	coder_t		*coders;
	int			flag;

	flag = 1;
	coders = (coder_t*)coders_v;
	while (flag)
	{
		i = 0;
		ts = ft_timestamp();
		while (coders[i].id != -1)
		{
			printf("[MONITORING]... coder id = %d coder burnout == %d limit == %d\n", 
				coders[i].id, ts - coders[i].burn_counter_ms, coders[i].ms_burn);
			if (ts - coders[i].burn_counter_ms > coders[i].ms_burn)
			{
				flag = 0;
				ft_stop_monitor(coders);
				break ;
			}
			i++;

		}
		printf("timestamp %d\n\n", ts);
		usleep(10000);
	}
	i = 0;
	ts = ft_timestamp();
	while (coders[i].id != -1)
	{
		printf("[MONITORING last]... coder id = %d coder burnout == %d limit == %d\n", 
			coders[i].id, ts - coders[i].burn_counter_ms, coders[i].ms_burn);
			i++;
		}
	printf("timestamp %d\n\n", ts);
}

