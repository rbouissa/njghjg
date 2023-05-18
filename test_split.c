

#include "minishell.h"

int	find_spliter(char s, char *sp)
{
	int	i;
	int	j;

	j = 0;
	i = 0;
	while (sp[i])
	{
		if (s == sp[i])
			return (1);
		i++;
	}
	return (0);
}

int	my_token(char *s)
{
	int	token;

	token = 0;
	if (!ft_strcmp(s, "|"))
		token = 5;
	else if (!ft_strcmp(s, ">"))
		token = 2;
	else if (!ft_strcmp(s, "<"))
		token = 3;
	else if (!ft_strcmp(s, "<<"))
		token = 4;
	else if (!ft_strcmp(s, ">>"))
		token = 9;
	else if (!ft_strcmp(s, "||"))
		token = 6;
	return (token);
}

void	initiate_variables(t_var_them **vr)
{
	(*vr)->check = 0;
	(*vr)->check_q = 0;
	(*vr)->quotes = 0;
	(*vr)->node = NULL;
	(*vr)->st = NULL;
	(*vr)->j = 0;
	(*vr)->i = 0;
	(*vr)->e = 0;
	(*vr)->token = 0;
	(*vr)->k = 0;
}

t_list	*ft_split_them(char *str)
{
	t_list		*s;
	t_var_them	*t;

	
    t = malloc(sizeof(t_var_them));
	initiate_variables(&t);
	while (str[t->i])
	{
		t->k = t->i;
		while (str[t->i] && !find_spliter(str[t->i], "|<> \t"))
		{
			if (str[t->i] == 34 || str[t->i] == 39)
			{
				while (str[t->i] == 34 || str[t->i] == 39)
				{
					t->quotes = str[t->i];
					t->i++;
					while (str[t->i] && (str[t->i] != t->quotes))
						t->i++;
				}
			}
			else
				t->i++;
		}
		if (t->i - t->k != 0)
		{
			t->node = malloc(t->i - t->k + 1);
			t->i = t->k;
			t->j = 0;
			while (str[t->i] && !find_spliter(str[t->i], "|<> \t"))
			{
				if (str[t->i] == 34 || str[t->i] == 39)
				{
					while (str[t->i] == 34 || str[t->i] == 39)
					{
						t->node[t->j] = str[t->i];
						t->quotes = str[t->i];
						t->i++;
						t->j++;
						while (str[t->i] && (str[t->i] != t->quotes))
						{
							t->node[t->j] = str[t->i];
							t->j++;
							t->i++;
						}
						t->node[t->j] = str[t->i];
						t->j++;
						t->i++;
					}
				}
				else
				{
						t->node[t->j] = str[t->i];
						t->j++;
						t->i++;	
				}
                t->node[t->j]=0;
			}
			if (t->check == 0)
			{
				s = ft_lstnew(string_no_quotes(t->node), 0);
				t->check = 1;
			}
			else
				ft_lstadd_back(&s, ft_lstnew(string_no_quotes(t->node), 0));
		}
		else
		{
			t->k = t->i;
			t->j = 0;
			while (str[t->i] && find_spliter(str[t->i], "|<> \t"))
			{
				if (find_spliter(str[t->i], "|<>"))
					t->j++;
				t->i++;
			}
			if (t->j != 0)
			{
				t->st = malloc(t->j + 1);
				t->j = 0;
				t->i = t->k;
				while (str[t->i] && find_spliter(str[t->i], "|<> \t"))
				{
					if (find_spliter(str[t->i], "|<>"))
					{
						t->st[t->j] = str[t->i];
						t->j++;
						t->i++;
						if (str[t->i] == str[t->i - 1])
						{
							t->st[t->j] = str[t->i];
							t->i++;
							t->j++;
						}
						break ;
					}
					t->i++;
				}
				t->st[t->j] = 0;
				if (t->check == 0)
				{
					t->token = my_token(t->st);
					s = ft_lstnew(string_no_quotes(t->st), t->token);
					t->check = 1;
				}
				else
				{
					t->token = my_token(t->st);
					ft_lstadd_back(&s, ft_lstnew(string_no_quotes(t->st),
								t->token));
				}
			}
		}
	}
	free(t->node);
	free(t->st);
    free(t);
	return (s);
}
