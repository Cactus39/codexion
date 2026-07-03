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

void	ft_start(coder_t coders[])
{
	pthread_t	thread_id;
	int			i;

	i = 0;
	while (coders[i].id != -1)
	{
		pthread_create(&thread_id, NULL, ft_handler, (void *)(&coders[i]));
		coders[i].thread_id = thread_id;
		i++;
	}
}

void	ft_join(coder_t coders[])
{
	int			i;

	i = 0;
	while (coders[i].id != -1)
	{
		pthread_join(coders[i].thread_id, NULL);
		i++;
	}
}
