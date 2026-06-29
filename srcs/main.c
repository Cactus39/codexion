#include "codex.h"

int	main(void)
{
	// struct timeval tv;
	printf("\033c");
	pthread_mutex_t log_mtx;
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutex_init(&log_mtx, &attr);
	pthread_mutexattr_destroy(&attr);


	// int t = gettimeofday(&tv, NULL);
	dongle_t	dongle_l = {.ms_cooldown = -1000};
	dongle_t	dongle_r = {.ms_cooldown = -1000};
	pthread_mutexattr_init(&attr);
	pthread_mutex_init(&dongle_l.mtx, &attr);
	pthread_mutex_init(&dongle_r.mtx, &attr);
	pthread_mutexattr_destroy(&attr);
	
	coder_t	coder = {.id = 0,
					.ms_burn = 1000,
					.burn_counter_ms = 0,
					.ms_compile = 10,
					.ms_refactor = 10,
					.ms_debug = 10,
					.comp_counter = 5,
					.d_left = &dongle_l,
					.d_right = &dongle_r,
					.cooldown = 100,
					.log_mtx = &log_mtx};

	coder_t	coder_1 = {.id = 1,
					.ms_burn = 1000,
					.burn_counter_ms = 0,
					.ms_compile = 10,
					.ms_refactor = 10,
					.ms_debug = 10,
					.comp_counter = 5,
					.d_left = &dongle_l,
					.d_right = &dongle_r,
					.cooldown = 100,
					.log_mtx = &log_mtx};


	ft_start(2, &coder, &coder_1);

	ft_join(2, &coder, &coder_1);
	// pthread_join(t1, NULL);
	// pthread_join(t2, NULL);

}