#include "codex.h"

void	ft_log(uint32_t timestamp, uint32_t id, char *message)
{
	printf("%d %d %s\n", timestamp, id, message);
}

uint32_t	ft_timestamp()
{
	static long long	start_ms;
	static int			flag;
	struct				timeval tv;
	if (!flag && gettimeofday(&tv, 0) == 0)
	{
		flag = 1;
		start_ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	}
	if (gettimeofday(&tv, 0) == 0)
		return ((tv.tv_sec * 1000 + tv.tv_usec / 1000) - start_ms);
	return(0);
}

void	*ft_handler(void *coder_v)
{
	coder_t		*coder;
	uint32_t	timestamps[3];
	uint32_t	timestamp;
	coder = (coder_t *)coder_v;
	while (coder->comp_counter)
	{
		timestamp = ft_timestamp();
		// printf("llooop left %d\n", !(timestamp - coder->d_left->ms_cooldown >= coder->cooldown));
		// printf("llooop right %d\n", !(timestamp - coder->d_right->ms_cooldown >= coder->cooldown));
		if ((!(timestamp - coder->d_left->ms_cooldown >= coder->cooldown)) || 
		(!(timestamp - coder->d_right->ms_cooldown >= coder->cooldown)))
			continue ;
		pthread_mutex_lock(&coder->d_left->mtx);
		pthread_mutex_lock(&coder->d_right->mtx);
		coder->comp_counter--;
		usleep(coder->ms_compile * 1000);
		timestamps[0] = ft_timestamp(); 				//compile
		coder->burn_counter_ms = timestamps[0];
		pthread_mutex_unlock(&coder->d_left->mtx);
		pthread_mutex_unlock(&coder->d_right->mtx);
		coder->d_left->ms_cooldown = timestamps[1];
		coder->d_right->ms_cooldown = timestamps[1];
		timestamps[1] = ft_timestamp();					//debug
		// printf("\nts1 %d\n\n", timestamps[1]);
		usleep(coder->ms_debug * 1000);
		timestamps[2] = ft_timestamp();					//refactor
		usleep(coder->ms_refactor * 1000);
		// timestamp = ft_timestamp();
		// pthread_mutex_lock(coder->log_mtx);
		ft_log(timestamps[0], coder->id, "is compiling");
		ft_log(timestamps[1], coder->id, "is debugging");
		ft_log(timestamps[2], coder->id, "is refactoring");
		// printf("tslast %d\n", timestamp);
		// printf("burnout timer id [%d] [%d]\n", coder->id, timestamp - coder->burn_counter_ms);
		// printf("dongle left timer id [%d] [%d] [%d]\n", timestamp, coder->d_left->ms_cooldown, timestamp - coder->d_left->ms_cooldown);
		// printf("dongle right timer id [%d] [%d] [%d]\n", timestamp, coder->d_right->ms_cooldown, timestamp - coder->d_right->ms_cooldown);
		// pthread_mutex_unlock(coder->log_mtx);
	}
}

void	ft_start(int num, ...)
{
	pthread_t	thread_id;
	va_list		args;
	coder_t		*coder;

	va_start(args, num);
	while (num > 0)
	{
		coder = va_arg(args, coder_t*);
		pthread_create(&thread_id, NULL, ft_handler, (void *)coder);
		coder->thread_id = thread_id;
		num--;
	}
	va_end(args);
}

void	ft_join(int num, ...)
{
	va_list		args;
	coder_t		*coder;

	va_start(args, num);
	while (num > 0)
	{
		coder = va_arg(args, coder_t*);
		pthread_join(coder->thread_id, NULL);
		num--;
	}
}