

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

t_list	*ft_split_them(char *str)
{
	t_list	*s;
	int		i;
	int		e;
	char	*node;
	char	*st;
	int		j;
	int		k;
	int		check;
	int		token;
	int		quotes;
	int		check_q;

	check = 0;
	check_q = 0;
	quotes = 0;
	node = NULL;
	st = NULL;
	s = NULL;
	j = 0;
	i = 0;
	e = 0;
	token = 0;
	k = 0;
	while (str[i])
	{
		k = i;
		while (str[i] && !find_spliter(str[i], "|<> \t"))
		{
			if (str[i] == 34 || str[i] == 39)
			{
				while (str[i] == 34 || str[i] == 39)
				{
					quotes = str[i];
					i++;
					while (str[i] && (str[i] != quotes))
						i++;
				}
			}
			else
			{
				//for alocation
				i++;
			}
		}
		//if don't find the spliter in the front
		if (i - k != 0)
		{
			//the alocate
			node = malloc(i - k + 1);
			i = k;
			j = 0;
			//fill our node
			while (str[i] && !find_spliter(str[i], "|<> \t"))
			{
				if (str[i] == 34 || str[i] == 39)
				{
					while (str[i] == 34 || str[i] == 39)
					{
						node[j] = str[i];
						quotes = str[i];
						i++;
						j++;
						while (str[i] && (str[i] != quotes))
						{
							node[j] = str[i];
							j++;
							i++;
						}
						printf("->>>>%c\n", str[i]);
						node[j] = str[i];
						j++;
						i++;
						node[j] = 0;
					}
				}
				else
				{
					{
						node[j] = str[i];
						j++;
						i++;
					}
					node[j] = 0;
				}
			}
			printf("------->%s\n", node);
			//to be here just on time for creat the frist node
			if (check == 0)
			{
				s = ft_lstnew(string_no_quotes(node), 0);
				check = 1;
			}
			//her for the rest
			else
				ft_lstadd_back(&s, ft_lstnew(string_no_quotes(node), 0));
		}
		else
		{
			k = i;
			j = 0;
			while (str[i] && find_spliter(str[i], "|<> \t"))
			{
				if (find_spliter(str[i], "|<>"))
					j++;
				i++;
			}
			if (j != 0)
			{
				st = malloc(j + 1);
				j = 0;
				i = k;
				while (str[i] && find_spliter(str[i], "|<> \t"))
				{
					if (find_spliter(str[i], "|<>"))
					{
						st[j] = str[i];
						j++;
						i++;
						if (str[i] == str[i - 1])
						{
							st[j] = str[i];
							i++;
							j++;
						}
						break ;
					}
					i++;
				}
				st[j] = 0;
				if (check == 0)
				{
					token = my_token(st);
					s = ft_lstnew(string_no_quotes(st), token);
					check = 1;
				}
				else
				{
					token = my_token(st);
					ft_lstadd_back(&s, ft_lstnew(string_no_quotes(st), token));
				}
			}
		}
	}
	free(node);
	free(st);
	return (s);
}
