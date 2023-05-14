/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: met-tahe <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/19 18:53:35 by met-tahe          #+#    #+#             */
/*   Updated: 2023/03/19 18:53:41 by met-tahe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	**cmd_paths(t_mini *mini)
{
	t_mini	*tmp;
	char	*paths;
	char	**spl_path;
	int		i;

	i = 0;
	paths = NULL;
	tmp = mini;
	while (tmp)
	{
		if (ft_strcmp(tmp->data_var, "PATH") == 0)
		{
			paths = tmp->data_val;
			break ;
		}
		else
			tmp = tmp->next;
	}
	spl_path = ft_split(paths, ':');
	while (spl_path[i])
	{
		spl_path[i] = ft_strjoin(spl_path[i], "/");
		i++;
	}
	return (spl_path);
}

void	handle_sigint(int sig)
{
	(void)sig;
	printf("Child process received SIGINT\n");
}
int	count_pipes(t_cmd *cmd)
{
	t_cmd	*tmp;
	int		pipe_count;

	tmp = NULL;
	tmp = cmd;
	pipe_count = 0;
	while (tmp)
	{
		pipe_count++;
		tmp = tmp->next;
	}
	return (pipe_count - 1);
}
void	mini_builtins1(char **cmd, t_mini *mini)
{
	int	e;

	e = 1;
	if (ft_strcmp(cmd[0], "export") == 0)
	{
		if (cmd[1] == NULL)
			ft_export(cmd[1]);
		else
		{
			while (cmd[e])
			{
				ft_export(cmd[e]);
				e++;
			}
		}
	}
	else if (ft_strcmp(cmd[0], "unset") == 0)
	{
		e = 1;
		if (cmd[1] == NULL)
			ft_unset(cmd[1], &mini);
		while (cmd[e])
		{
			ft_unset(cmd[e], &mini);
			e++;
		}
	}
}

int	builts_without_pipe(t_cmd *cmd, t_mini *mini)
{
	int	i;

	i = 0;
	while (cmd->cmd[0] && cmd->cmd[0][i] != '\0')
		(cmd->cmd[0][i] = ft_tolower(cmd->cmd[0][i]), i++);
	if (cmd->cmd[0] == NULL)
		return (0);
	else if ((ft_strcmp(cmd->cmd[0], "export") == 0 && cmd->next == NULL
				&& cmd->cmd[1] != NULL) || (ft_strcmp(cmd->cmd[0], "unset") == 0
				&& cmd->next == NULL))
		return (mini_builtins1(cmd->cmd, mini), 1);
	else if (ft_strcmp(cmd->cmd[0], "env") == 0 && cmd->next == NULL)
		return (ft_env(mini), 1);
	else if (ft_strcmp(cmd->cmd[0], "cd") == 0 && cmd->next == NULL)
		return (ft_cd(cmd->cmd[1], mini), 1);
	else if (ft_strcmp(cmd->cmd[0], "exit") == 0 && cmd->next == NULL)
		return (ft_exit(cmd->cmd[1]), 1);
	return (0);
}

int	execute_builts(t_mini *mini, char **cmd)
{
	int	i;

	i = 0;
	while (cmd[0] && cmd[0][i] != '\0')
		(cmd[0][i] = ft_tolower(cmd[0][i]), i++);
	if (cmd[0] == NULL)
		return (0);
	else if (ft_strcmp(cmd[0], "export") == 0 || ft_strcmp(cmd[0],
				"unset") == 0)
		return (mini_builtins1(cmd, mini), 1);
	else if (ft_strcmp(cmd[0], "env") == 0)
		return (ft_env(mini), 1);
	else if (ft_strcmp(cmd[0], "echo") == 0)
		return (echo(cmd), my_global->status = 0, 1);
	else if (ft_strcmp(cmd[0], "cd") == 0)
		return (ft_cd(cmd[1], mini), 1);
	else if (ft_strcmp(cmd[0], "pwd") == 0)
		return (ft_pwd(), 1);
	else if (ft_strcmp(cmd[0], "exit") == 0)
	{
		return (ft_exit(cmd[1]), 1);
	}
	return (0);
}
void	shell_error(char *str)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(str, 2);
	ft_putstr_fd(": command not found\n", 2);
	my_global->status = 127;
	exit(my_global->status);
}
int	hrdpipe_num(t_cmd *tmp)
{
	t_cmd	*tmp1;
	int		i;

	tmp1 = tmp;
	i = 0;
	while (tmp1)
	{
		if (tmp1->herdoc && tmp1->herdoc[0] != NULL)
		{
			i++;
		}
		tmp1 = tmp1->next;
	}
	return (i);
}
void	pipe_herdoc(t_cmd *cmd)
{
	t_cmd	*tmp;
	int		i;

	tmp = cmd;
	i = 0;
	my_global->hrdsize = hrdpipe_num(tmp);
	my_global->hrd = malloc(sizeof(int *) * (my_global->hrdsize + 1));
	while (i <= my_global->hrdsize)
	{
		my_global->hrd[i] = malloc(sizeof(int) * 2);
		if (pipe(my_global->hrd[i]) < 0)
			fct_error("pipe");
		i++;
	}
	if (my_global->hrdsize == 0)
	{
		write(2, "the problem is here\n", 20);
		close(my_global->hrd[0][0]);
		close(my_global->hrd[0][1]);
	}
}

void	sort_in_pipe(int pip_count, int **fd, int i)
{
	if (i == 0)
	{
		close(fd[i][0]);
		dup2(fd[i][1], STDOUT_FILENO);
		close(fd[i][1]);
		close_pipes(pip_count, fd);
	}
	else if (i == pip_count)
	{
		close(fd[i - 1][1]);
		dup2(fd[i - 1][0], STDIN_FILENO);
		close(fd[i - 1][0]);
		close_pipes(pip_count, fd);
	}
	else
	{
		close(fd[i - 1][1]);
		dup2(fd[i - 1][0], STDIN_FILENO);
		close(fd[i - 1][0]);
		close(fd[i][0]);
		dup2(fd[i][1], STDOUT_FILENO);
		close(fd[i][1]);
		close_pipes(pip_count, fd);
	}
}

void	mini_execution(char **cmd, char **env)
{
	if (cmd[0][1] == '\0')
	{
		if (cmd[0][0] == '/')
		{
			printf("minishell: %s: is a directory\n", cmd[0]);
			exit(my_global->status = 126);
		}
		else
		{
			printf("minishell:%s: filename argument required\n%s: usage:%s filename [arguments]\n",
					cmd[0],
					cmd[0],
					cmd[0]);
			exit(my_global->status = 2);
		}
	}
	else if (execve(cmd[0], cmd, env) == -1)
	{
		printf("minishell: %s: is a directory\n", cmd[0]);
		exit(my_global->status = 127);
	}
}

void	child_execution(t_cmd *cmd, char **paths, char **env, int i)
{
	//cas de redirection infile,outfile,herdoc;
	if (cmd->infile != -2 && cmd->infile != 0 && cmd->infile != -1)
	{
		if (dup2(cmd->infile, STDOUT_FILENO) == -1)
			(my_global->status = 1,fct_error("dup2"));
		close(cmd->infile);
	}
	if (cmd->outfile != -2 && cmd->outfile != 0 && cmd->outfile != -1)
	{
		if (dup2(cmd->outfile, STDIN_FILENO) == -1)
			(my_global->status = 1,fct_error("dup2"));
		close(cmd->outfile);
	}
	if (cmd->herdoc && cmd->herdoc[0] != NULL)
	{
		if (dup2(my_global->hrd[i][0], STDIN_FILENO) == -1)
			(my_global->status = 1,fct_error("dup2"));
		close(my_global->hrd[i][0]);
		close(my_global->hrd[i][1]);
	}
	if (execute_builts(my_global->m_env, (cmd)->cmd) == 1)
		exit(my_global->status = 0);
	if ((cmd->cmd[0][0] == '/' || cmd->cmd[0][0] == '.') && access(cmd->cmd[0],
			F_OK) == 0)
		mini_execution(cmd->cmd, env);
	else
		execute(cmd->cmd, paths, env);
}
// fonction qui execute dans le child de heredoc;
void	heredoc_child(int *stat1, t_cmd *tmp1, char *line, int *j)
{
	int	k;

	k = 0;
	if (my_global->hrd_id < 0)
		fct_error("fork");
	else if (my_global->hrd_id == 0)
	{
		(signal(SIGINT, SIG_DFL), signal(SIGQUIT, ctrld_handler));
		k = 1;
		line = heredoc(tmp1->herdoc);
		close(my_global->hrd[*j][0]);
		ft_putstr_fd(line, my_global->hrd[*j][1]);
		close(my_global->hrd[*j][1]);
		(free(line), exit(0));
	}
	else
	{
		waitpid(my_global->hrd_id, stat1, 0);
		close(my_global->hrd[*j][1]);
		//close(my_global->hrd[*j][0]);
		if (WIFSIGNALED(*stat1))
		{
			my_global->status = 1;
			(*stat1) = 1;
		}
	}
	if (k == 1)
	{
		close(my_global->hrd[*j][1]);
		close(my_global->hrd[*j][0]);
	}
}
// this function is used to read the heredoc and write it in the pipe;
void	read_heredoc(t_cmd *cmd, int *stat1)
{
	char			*line;
	int				j;
	int				z;
	t_cmd			*tmp1;
	struct termios	term;
	struct termios	term2;

	j = 0;
	tmp1 = cmd;
	z = 0;
	line = NULL;
	tcgetattr(0, &term);
	term2 = term;
	term.c_lflag &= ~ISIG;
	term.c_lflag &= ~ECHOCTL;
	while (tmp1)
	{
		if (tmp1->herdoc && tmp1->herdoc[0] != NULL)
		{
			z = 1;
			my_global->hrd_id = fork();
			heredoc_child(stat1, tmp1, line, &j);
		}
		j++;
		if (*stat1 == 1)
		{
			my_global->status = 1;
			break ;
		}
		tmp1 = tmp1->next;
	}
}

void	pipes_creation(int pip_count, int **fd)
{
	int	i;

	i = 0;
	// creer les pipes;
	if (pip_count > 0)
	{
		while (i <= pip_count)
		{
			if (pipe(fd[i]) < 0)
				fct_error("pipe");
			i++;
		}
	}
}

void	parent_waiting(t_cmd **cmd, int i, int pip_count, int **fd)
{
	//int	stat2;
	//stat2 = 0;
	//attendre le child process;
	//waitpid(my_global->id[i], &stat2, 0);
	//close((*cmd)->outfile);
	//close((*cmd)->infile);
	//close les fd de pipe et de herdoc dans le parent;
	parent_closing(i, pip_count, fd, (*cmd)->herdoc);
	// if (WIFSIGNALED(stat2))
	// 	my_global->status = 128 + WTERMSIG(stat2);
	// else if (WIFEXITED(stat2))
	// 	my_global->status = WEXITSTATUS(stat2);
	*cmd = (*cmd)->next;
}

void	parent_waiting_uniq(int **fd, int *i, t_cmd *cmd)
{
	int	status;
	int j;

	status = 0;
	(void)fd;
	(void)cmd;
	// attendre que les child process finissent;
	waitpid(my_global->id[*i], &status, 0);
	close(cmd->outfile);
	close(cmd->infile);
	j = 0;
	while (j <= my_global->hrdsize)
	{
		close(my_global->hrd[j][0]);
		close(my_global->hrd[j][1]);
		j++;
	}
	if (WIFSIGNALED(status))
	{
		my_global->status = 128 + WTERMSIG(status);
	}
	if (WIFEXITED(status))
		my_global->status = WEXITSTATUS(status);
	free(my_global->id);
	// fermer les pipes;
	//close_pipes2(pip_count, fd,cmd->herdoc);
	// fermer les pipes de heredoc;
}

void	one_cmd(t_cmd *cmd, t_vars *vars, char **env)
{
	// close les pipes que je n'utulise pas;
	//close_pipes(pip_count, fd);
	// executer les commandes builtins;
	if (builts_without_pipe(cmd, my_global->m_env) == 1)
		return ;
	//creer le child process;
	my_global->id[vars->i] = fork();
	if (my_global->id[vars->i] < 0)
		fct_error("fork");
	else if (my_global->id[vars->i] == 0)
		child_execution(cmd, vars->paths, env, vars->i);
	// le parent process;
	else
		parent_waiting_uniq(vars->fd, &(vars->i), cmd);
}

int	**allocate_fd(int **fd, int pip_count, t_cmd *cmd)
{
	int	i;

	i = 0;
	fd = malloc(sizeof(int *) * (pip_count + 1));
	while (i <= pip_count)
	{
		fd[i] = malloc(sizeof(int) * 2);
		i++;
	}
	//printf("qqqqqqq\n");
	my_global->id = malloc(sizeof(pid_t) * (pip_count + 1));
	my_global->count_cmd = pip_count;
	//creer les pipes;
	pipes_creation(pip_count, fd);
	// creer les pipes pour les heredoc;
	pipe_herdoc(cmd);
	return (fd);
}
void	initiate_vars_structe(t_vars **vars, t_mini *mini, t_cmd *cmd)
{
	(*vars)->i = 0;
	(*vars)->check = 0;
	(*vars)->fd = NULL;
	(*vars)->paths = cmd_paths(mini);
	(*vars)->pip_count = count_pipes(cmd);
	(*vars)->stat1 = 0;
}
void	exec_cmd(t_mini *mini, t_cmd *cmd, char **env)
{
	t_vars	*vars;
	int		j;
	int		stat;

	vars = malloc(sizeof(t_vars));
	my_global->count_cmd = count_pipes(cmd);
	initiate_vars_structe(&vars, mini, cmd);
	vars->fd = allocate_fd(vars->fd, vars->pip_count, cmd);
	read_heredoc(cmd, &(vars->stat1));
	// executer une seule commande;
	if (vars->pip_count == 0)
	{
		one_cmd(cmd, vars, env);
	}
	else
	{
		vars->i = 0;
		while (!vars->stat1 && cmd && vars->i <= vars->pip_count)
		{
			vars->check = 1;
			//  creation des child process;
			my_global->id[vars->i] = fork();
			multiple_cmd(&cmd, vars, env, mini);
			(vars->i)++;
		}
		// fermer tous les pipes;
		if (vars->check == 1)
		{
			stat = 0;
			j = 0;
			waitpid(my_global->id[vars->pip_count], &stat, 0);
			close_pipes(vars->pip_count, vars->fd);
			while (j <= vars->pip_count)
			{
				waitpid(my_global->id[j], NULL, 0);
				j++;
			}
			j = 0;
			while (j <= my_global->hrdsize)
			{
				close(my_global->hrd[j][0]);
				close(my_global->hrd[j][1]);
				j++;
			}
			if (WIFSIGNALED(stat))
				my_global->status = 128 + WTERMSIG(stat);
			else if (WIFEXITED(stat))
				my_global->status = WEXITSTATUS(stat);
			free(my_global->id);
		}
	}
	ft_free(vars->paths);
	//free(my_global->id);
	free(my_global->hrd);
}
void	multiple_cmd(t_cmd **cmd, t_vars *vars, char **env, t_mini *mini)
{
	(void)mini;
	if (my_global->id[vars->i] < 0)
		fct_error("fork");
	// child process;
	else if (my_global->id[vars->i] == 0)
	{
		//handler le signal sigint;
		signal(SIGINT, SIG_DFL);
		sort_in_pipe(vars->pip_count, vars->fd, vars->i);
		// executer les commandes builtins;
		child_execution(*cmd, vars->paths, env, vars->i);
	}
	// incrementer a la commande suivante;
	else
		parent_waiting(cmd, vars->i, vars->pip_count, vars->fd);
}
void	parent_closing(int i, int pip_count, int **fd, char **herdoc)
{
	if (herdoc[0] != NULL)
		close(my_global->hrd[i][0]);
	if (i == 0)
		close(fd[i][1]);
	else if (i == pip_count)
	{
		close(fd[i - 1][0]);
		//close(fd[i - 1][1]);
	}
	else
	{
		close(fd[i][1]);
		close(fd[i - 1][0]);
		close(fd[i - 1][1]);
	}
}

void	execute(char **cmd, char **paths, char **env)
{
	int	j;

	j = 0;
	while (paths[j])
	{
		paths[j] = ft_strjoin(paths[j], cmd[0]);
		if (access(paths[j], F_OK) == 0)
		{
			if (execve(paths[j], cmd, env) == -1)
				fct_error("execve");
		}
		else
			(j)++;
	}
	if (paths[j] == NULL)
		shell_error(cmd[0]);
}
void	close_pipes(int pip_count, int **fd)
{
	int	i;

	i = 0;
	while (i <= pip_count)
	{
		close(fd[i][0]);
		close(fd[i][1]);
		i++;
	}
}

void	close_pipes2(int pip_count, int **fd, char **herdoc)
{
	int	i;

	i = 0;
	while (i <= pip_count)
	{
		close(fd[i][0]);
		close(fd[i][1]);
		i++;
	}
	i = 0;
	if (herdoc[0] != NULL)
	{
		close(my_global->hrd[i][0]);
		close(my_global->hrd[i][1]);
	}
}

void	fct_error(char *str)
{
	perror(str);
	my_global->status = 1;
	printf("status = %d\n", my_global->status);
	exit(my_global->status);
}
