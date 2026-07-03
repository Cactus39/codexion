#include "codex.h"

int	main(int argc, char **argv)
{
	// struct timeval tv;
	printf("\033c");
	settings_t	settings;
	settings = ft_parse_input(argc, argv);
	if (strcmp(settings.scheduler, "err") == 0)
		return (0);
	coder_t		*coders;
	dongle_t	*dongles;

	dongles = ft_init_dongles(&settings);
	coders = ft_init_coders(&settings, dongles);

	ft_start(coders);
	pthread_t	monitor_id;
	pthread_create(&monitor_id, NULL, ft_start_monitor, (void*)coders);

	
	ft_join(coders);
	pthread_join(monitor_id, NULL);
	free(coders);
	free(dongles);
	
	// pthread_join(t1, NULL);
	// pthread_join(t2, NULL);

}