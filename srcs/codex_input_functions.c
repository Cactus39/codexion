#include "codex.h"

settings_t	ft_parse_input(int argc, char **argv)
{
	settings_t	settings;
	if (argc == 1)
	{
		settings.number_of_coders = 6;
		settings.time_to_burnout = 30;
		settings.time_to_compile = 10;
		settings.time_to_debug = 10;
		settings.time_to_refactor = 10;
		settings.number_of_compiles_required = 10;
		settings.dongle_cooldown = 10;
		strcpy(settings.scheduler, "fifo");
		return settings;
	}
	
	if (argc != 8)
	{
		strcpy(settings.scheduler, "err");
		return settings;
	}
	settings.number_of_coders = atoi(argv[1]);
	settings.time_to_burnout = atoi(argv[2]);
	settings.time_to_compile = atoi(argv[3]);
	settings.time_to_debug = atoi(argv[4]);
	settings.time_to_refactor = atoi(argv[5]);
	settings.number_of_compiles_required = atoi(argv[6]);
	settings.dongle_cooldown = atoi(argv[7]);
	strcpy(settings.scheduler, "fifo");
	return (settings);
}

dongle_t	*ft_init_dongles(settings_t *settings)
{
	dongle_t	*dongles;
	int			i;
	pthread_mutexattr_t attr;
	
	pthread_mutexattr_init(&attr);
	i = 0;
	dongles = malloc(sizeof(dongle_t) * (settings->number_of_coders));
	if (dongles == NULL)
		return (NULL);
	while (i < settings->number_of_coders)
	{
		dongles[i].id = i;
		dongles[i].ms_cooldown = settings->dongle_cooldown;
		dongles[i].cooldown_counter = 0;
		dongles[i].is_busy = 0;
		pthread_mutex_init(&(dongles[i].mtx), &attr);
		i++;
	}
	pthread_mutexattr_destroy(&attr);
	return (&dongles[0]);
}

coder_t	*ft_init_coders(settings_t *settings, dongle_t *dongles)
{
	coder_t	*coders;
	coder_t	coder;
	int		i;

	i = 0;
	coders = malloc(sizeof(coder_t) * (settings->number_of_coders + 1));
	if (coders == NULL)
		return (NULL);
	while (i < settings->number_of_coders)
	{
		coders[i].id = i;
		coders[i].ms_burn = settings->time_to_burnout;
		coders[i].burn_counter_ms = 0; //????
		coders[i].ms_compile = settings->time_to_compile;
		coders[i].ms_debug = settings->time_to_debug;
		coders[i].ms_refactor = settings->time_to_refactor;
		coders[i].comp_counter = settings->number_of_compiles_required;
		coders[i].d_left = &dongles[i];
		coders[i].d_right = &dongles[(i + 1) % settings->number_of_coders];
		coders[i].thread_id = 0;
		coders[i].log_mtx = settings->log_mtx;
		coders[i].running = 1;
		i++;
	}
	coders[i].id = -1;
	return (&coders[0]);
}