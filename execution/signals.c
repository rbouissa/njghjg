#include "../minishell.h"

void	ctrlc_handler(int sig)
{
	int	i;

	(void)sig;
	i = 0;
	write(1, "\n", 1);
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
	if (my_global->hrd_id)
	{
		kill(my_global->hrd_id, SIGINT);
	}
	if (my_global->id)
	{
		while (my_global->id[i])
		{
			kill(my_global->id[i], SIGINT);
			i++;
		}
	}
	my_global->status = 1;
}

void	ctrld_handler(int sig)
{
	int	i;

	(void)sig;
	i = 0;
	if (my_global->hrd_id != 0)
		rl_on_new_line();
	else
	{
		rl_on_new_line();
		rl_redisplay();
	}
	if (my_global->id)
	{
		while (my_global->id[i])
		{
			kill(my_global->id[i], SIGQUIT);
			i++;
		}
	}
}

void	sig_sil(int sig)
{
	(void)sig;
	rl_on_new_line();
	//rl_redisplay();
	return ;
}
