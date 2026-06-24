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
	return(-1);
}

void	*ft_handler(void *coder_v)
{
	coder_t		*coder;
	coder = (coder_t *)coder_v;
	while (coder->comp_counter)
	{
		pthread_mutex_lock(&coder->d_left->mtx);
		ft_log(ft_timestamp(), coder->id, "is compiling");
		coder->comp_counter--;
		usleep(coder->ms_compile * 1000);
		pthread_mutex_unlock(&coder->d_left->mtx);
		ft_log(ft_timestamp(), coder->id, "is debugging");
		usleep(coder->ms_debug * 1000);
		ft_log(ft_timestamp(), coder->id, "is refactoring");
		usleep(coder->ms_refactor * 1000);
	}
}



pthread_t	ft_loop(coder_t *coder)
{
	pthread_t	thread_id;

	pthread_create(&thread_id, NULL, ft_handler, (void * )coder);
	return (thread_id);
}