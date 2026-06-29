#include "codex.h"



void	*ft_handler(void *coder_v)
{
	coder_t		*coder;
	uint32_t	timestamps[3];
	uint32_t	timestamp;
	coder = (coder_t *)coder_v;
	while (coder->comp_counter)
	{
		timestamp = ft_timestamp();
		// if ((!(timestamp - coder->d_left->ms_cooldown >= coder->cooldown)) || 
		// (!(timestamp - coder->d_right->ms_cooldown >= coder->cooldown)))
			// continue ;
		pthread_mutex_lock(&coder->d_left->mtx);
		pthread_mutex_lock(&coder->d_right->mtx);
		coder->comp_counter--;
		usleep(coder->ms_compile * 1000);
		timestamps[0] = ft_timestamp(); 				//compile
		coder->burn_counter_ms = timestamps[0];
		pthread_mutex_unlock(&coder->d_left->mtx);
		pthread_mutex_unlock(&coder->d_right->mtx);
		timestamps[1] = ft_timestamp();					//debug
		coder->d_left->ms_cooldown = timestamps[1];
		coder->d_right->ms_cooldown = timestamps[1];
		usleep(coder->ms_debug * 1000);
		timestamps[2] = ft_timestamp();					//refactor
		usleep(coder->ms_refactor * 1000);
		ft_log(timestamps[0], coder->id, "is compiling");
		ft_log(timestamps[1], coder->id, "is debugging");
		ft_log(timestamps[2], coder->id, "is refactoring");
	}
	return (NULL);
}

void	ft_start(coder_t coders[])
{
	pthread_t	thread_id;
	// coder_t		*coder;
	int			i;

	i = 0;
	while (coders[i].id != -1)
	{
		pthread_create(&thread_id, NULL, ft_handler, (void *)(&coders[i]));
		coders[i].thread_id = thread_id;
		// printf("%d id %lu\n", coders[i].id, thread_id);
		i++;
	}
}

void	ft_join(coder_t coders[])
{
	// va_list		args;
	// coder_t		*coder;
	int			i;
	// va_start(args, num);
	i = 0;
	while (coders[i].id != -1)
	{
		// printf("%d %lu\n", coders[i].id, coders[i].thread_id);
		pthread_join(coders[i].thread_id, NULL);
		i++;
	}
}

// void	ft_start(int num, ...)
// {
// 	pthread_t	thread_id;
// 	va_list		args;
// 	coder_t		*coder;

// 	va_start(args, num);
// 	while (num > 0)
// 	{
// 		coder = va_arg(args, coder_t*);
// 		pthread_create(&thread_id, NULL, ft_handler, (void *)coder);
// 		coder->thread_id = thread_id;
// 		num--;
// 	}
// 	va_end(args);
// }

// void	ft_join(int num, ...)
// {
// 	va_list		args;
// 	coder_t		*coder;

// 	va_start(args, num);
// 	while (num > 0)
// 	{
// 		coder = va_arg(args, coder_t*);
// 		pthread_join(coder->thread_id, NULL);
// 		num--;
// 	}
// }