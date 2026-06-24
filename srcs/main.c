#include "codex.h"

int	main(void)
{
	struct timeval tv;
	printf("\033c");
	int t = gettimeofday(&tv, NULL);
	dongle_t	dongle = {.ms_cooldown = 100};
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutex_init(&dongle.mtx, &attr);
	pthread_mutexattr_destroy(&attr);

	coder_t	coder = {.id = 0,
					.ms_burn = 1000,
					.ms_compile = 100,
					.ms_refactor = 100,
					.ms_debug = 100,
					.comp_counter = 10,
					.d_left = &dongle,
					.d_right = NULL};

	coder_t	coder_1 = {.id = 1,
					.ms_burn = 1000,
					.ms_compile = 100,
					.ms_refactor = 100,
					.ms_debug = 100,
					.comp_counter = 5,
					.d_left = &dongle,
					.d_right = NULL};

	
	t = gettimeofday(&tv, NULL);
		
	// printf("%lld\n",(tv.tv_sec * 1000 + tv.tv_usec / 1000));
	pthread_t t1 = ft_loop(&coder);
	pthread_t t2 =ft_loop(&coder_1);
		
	t = gettimeofday(&tv, NULL);
	// printf("%lld\n",(tv.tv_sec * 1000 + tv.tv_usec / 1000));
	
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

}