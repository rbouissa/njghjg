#include "minishell.h"
void	list_free(t_list *cmd)
{
	t_list	*tmp;

	tmp = cmd;
	while (tmp)
	{
		free(tmp->data);
		tmp = tmp->next;
	}
	free(tmp);
}

void	freestack_last(t_list **stack)
{
	t_list	*temp;

	if (!*stack)
		return ;
	temp = *stack;
	while (*stack && (*stack)->next && (*stack)->token != 5)
	{
		(*stack) = (*stack)->next;
		free(temp->data);
		free(temp);
		temp = *stack;
	}
	*stack = (*stack)->next;
	if (temp->data)
		free(temp->data);
	free(temp);
}

int	file_numb(t_list *comm)
{
	t_list	*command;
	t_list	*cm;
	int		token;

	token = -2;
	cm = comm;
	command = comm;
	while (command->next)
	{
		if (command->next->token == 5 || command->next == NULL)
			break ;
		if (command->token == 2 || command->token == 9)
		{
			if (token != -2)
				close(token);
			if (command->token == 2)
			{
				token = open(command->next->data, O_CREAT | O_WRONLY, 0777);
				if (token == -1)
				{
					perror("open");
					my_global->status = -1;
					return (my_global->status);
				}
			}
			else if (command->token == 9)
			{
				token = open(command->next->data, O_CREAT | O_WRONLY | O_APPEND,
						0777);
				if (token == -1)
				{
					perror("open");
					my_global->status = -1;
					return (my_global->status);
				}
			}
		}
		command = command->next;
	}
	return (token);
}

int	her_pip(t_list *cmd)
{
	while (cmd)
	{
		if (cmd->token == 5)
			return (1);
		cmd = cmd->next;
	}
	return (0);
}

int	file_numb2(t_list *comm)
{
	t_list	*command;
	t_list	*cm;
	int		token;

	token = -2;
	cm = comm;
	command = comm;
	while (command->next)
	{
		if (command->next->token == 5 || command->next == NULL)
			break ;
		if (command->token == 3)
		{
			if (token != -2)
				close(token);
			token = open(command->next->data, O_RDONLY);
			if (token == -1)
			{
				printf("minishell: %s: No such file or directory\n",
						command->next->data);
				my_global->status = -1;
				return (my_global->status);
			}
		}
		command = command->next;
	}
	return (token);
}

void	initiate_cmm(t_split_com **vr)
{
	(*vr)->cmd = NULL;
	(*vr)->hrd = NULL;
	(*vr)->herdoc = NULL;
	(*vr)->full_cmd = NULL;
	(*vr)->check = 0;
	(*vr)->spliter = malloc(2);
	(*vr)->spliter[0] = 19;
	(*vr)->spliter[1] = '\0';
	(*vr)->token = -2;
	(*vr)->token2 = -2;
}

t_cmd	*split_to_commands(t_list *comm)
{
	t_split_com *t;
	t_list *command;
	t_cmd *list;
	t = malloc(sizeof(t_expand));
	initiate_cmm(&t);
	command = comm;

	if (!her_pip(command))
	{
		while (command)
		{
			if (command->token == 2 || command->token == 3
				|| command->token == 9)
			{
				if (command->token == 2 || command->token == 9)
					t->token = file_numb(comm);
				else if (command->token == 3)
					t->token2 = file_numb2(comm);
				command = command->next->next;
			}
			else if (command->token == 4)
			{
				command = command->next;
				t->hrd = ft_strjoin(t->hrd, t->spliter);
				t->hrd = ft_strjoin(t->hrd, command->data);
				command = command->next;
			}
			else
			{
				t->cmd = ft_strjoin(t->cmd, t->spliter);
				t->cmd = ft_strjoin(t->cmd, command->data);
				command = command->next;
			}
		}
		if (t->hrd != NULL)
		{
			if (t->cmd == NULL)
			{
				t->full_cmd = malloc(sizeof(char *));
				t->full_cmd[0] = 0;
				list = ft_lstnew_new(t->full_cmd, t->token, t->token2,
						ft_split(t->hrd, 19), 1);
				t->token = -2;
				t->token2 = -2;
			}
			else
			{
				list = ft_lstnew_new(ft_split(t->cmd, 19), t->token, t->token2,
						ft_split(t->hrd, 19), 1);
				t->token = -2;
				t->token2 = -2;
			}
		}
		else
		{
			t->herdoc = malloc(sizeof(char *));
			t->herdoc[0] = 0;
			list = ft_lstnew_new(ft_split(t->cmd, 19), t->token, t->token2,
					t->herdoc, -2);
			t->token = -2;
			t->token2 = -2;
		}
	}
	else
	{
		while (command != NULL)
		{
			if (command->token == 5)
			{
				command = command->next;
				if (t->check == 0)
				{
					t->check = 1;
					if (command->token == 2 || command->token == 9)
						t->token = file_numb(comm);
					else if (command->token == 3)
						t->token2 = file_numb2(comm);
					freestack_last(&comm);
					if (t->hrd != NULL)
					{
						if (t->cmd == NULL)
						{
							t->full_cmd = malloc(sizeof(char *));
							t->full_cmd[0] = 0;
							list = ft_lstnew_new(t->full_cmd, t->token,
									t->token2, ft_split(t->hrd, 19), 1);
						}
						else
						{
							list = ft_lstnew_new(ft_split(t->cmd, 19),
													t->token,
													t->token2,
													ft_split(t->hrd, 19),
													1);
						}
					}
					else
					{
						t->herdoc = malloc(sizeof(char *));
						t->herdoc[0] = 0;
						list = ft_lstnew_new(ft_split(t->cmd, 19), t->token,
								t->token2, t->herdoc, -2);
					}
					t->token = -2;
					t->token2 = -2;
				}
				else
				{
					if (command->token == 2 || command->token == 9)
						t->token = file_numb(comm);
					else if (command->token == 3)
						t->token2 = file_numb2(comm);
					freestack_last(&comm);
					if (t->hrd != NULL)
					{
						if (t->cmd != NULL)
							ft_lstadd_back_new(&list,
												ft_lstnew_new(ft_split(t->cmd,
																		19),
																t->token,
																t->token2,
																ft_split(t->hrd,
																		19),
																1));
						else
						{
							t->full_cmd = malloc(sizeof(char *));
							t->full_cmd[0] = 0;
							ft_lstadd_back_new(&list,
												ft_lstnew_new(t->full_cmd,
														t->token, t->token2,
														ft_split(t->hrd, 19),
														1));
						}
					}
					else
					{
						t->herdoc = malloc(sizeof(char *));
						t->herdoc[0] = 0;
						ft_lstadd_back_new(&list,
											ft_lstnew_new(ft_split(t->cmd, 19),
													t->token, t->token2,
													t->herdoc, 1));
					}
					t->token = -2;
					t->token2 = -2;
				}
				t->cmd = NULL;
				t->hrd = NULL;
			}
			else if (command->token == 2 || command->token == 3
					|| command->token == 9)
			{
				if (command->token == 2 || command->token == 9)
					t->token = file_numb(comm);
				else if (command->token == 3)
					t->token2 = file_numb2(comm);
				command = command->next->next;
			}
			else if (command->token == 4)
			{
				command = command->next;
				t->hrd = ft_strjoin(t->hrd, t->spliter);
				t->hrd = ft_strjoin(t->hrd, command->data);
				command = command->next;
			}
			//this part make a segfaul in the function of get token file
			else
			{
				t->cmd = ft_strjoin(t->cmd, t->spliter);
				t->cmd = ft_strjoin(t->cmd, command->data);
				command = command->next;
			}
		}
		if (t->hrd != NULL)
		{
			if (t->cmd != NULL)
				ft_lstadd_back_new(&list, ft_lstnew_new(ft_split(t->cmd, 19),
							t->token, t->token2, ft_split(t->hrd, 19), 1));
			else
			{
				t->full_cmd = malloc(sizeof(char *));
				t->full_cmd[0] = 0;
				ft_lstadd_back_new(&list, ft_lstnew_new(t->full_cmd, t->token,
							t->token2, ft_split(t->hrd, 19), 1));
			}
			t->token = -2;
			t->token2 = -2;
		}
		else
		{
			t->herdoc = malloc(sizeof(char *));
			t->herdoc[0] = 0;
			ft_lstadd_back_new(&list, ft_lstnew_new(ft_split(t->cmd, 19),
						t->token, t->token2, t->herdoc, 1));
			t->token = -2;
			t->token2 = -2;
		}
	}
	free(t->spliter);
	list_free(comm);
	return (list);
}