

#include "minishell.h"
int	is_allpha1(char s)
{
	if ((s >= 65 && s <= 90) || (s >= 97 && s <= 122))
		return (1);
	return (0);
}

int	is_digit1(char s)
{
	if ((s >= 48 && s <= 57))
		return (1);
	return (0);
}

int	find_spliter1(char s, char *sp)
{
	int	i;
	int	j;

	j = 0;
	i = 0;
	while (sp[i])
	{
		if (s == sp[i] || (!is_digit1(s) && !is_allpha1(s)))
			return (1);
		i++;
	}
	return (0);
}

char	*finnd_valeur(t_mini *env, char *str)
{
	t_mini	*tmp_env;

	tmp_env = env;
	if (str[0] == '?')
		return (ft_itoa(my_global->status));
	else
	{
		while (tmp_env)
		{
			if (!ft_strcmp(tmp_env->data_var, str))
				return (tmp_env->data_val);
			tmp_env = tmp_env->next;
		}
	}
	return (NULL);
}

void	initiate_expand(t_expand **vr)
{
    (*vr)->i = 0;
	(*vr)->j = 0;
	(*vr)->k = 0;
	(*vr)->quotes = 0;
	(*vr)->e = 0;
	(*vr)->h = 0;
	(*vr)->new_str = NULL;
	(*vr)->dollar = NULL;
	(*vr)->var = NULL;
	(*vr)->valeur = NULL;
	(*vr)->dollar = malloc(2);
	(*vr)->dollar[0] = '$';
	(*vr)->dollar[1] = '\0';
}

char	*new_expand(char *str, t_mini *env)
{
    t_expand *t;
    char *str2;

     t = malloc(sizeof(t_expand));
	initiate_expand(&t);
	while (str[t->i])
	{
		t->e = 0;
		if (str[t->i] == '\'' && t->quotes == no_quotes)
			t->quotes = isingle_quotes;
		else if (str[t->i] == '\"' && t->quotes == no_quotes)
			t->quotes = idouble_quotes;
		else if (str[t->i] == '\"' && t->quotes == idouble_quotes)
			t->quotes = no_quotes;
		else if (str[t->i] == '\'' && t->quotes == isingle_quotes)
			t->quotes = no_quotes;
		if (str[t->i] == '$' && t->quotes != isingle_quotes)
		{
			while (str[t->i] == '$')
			{
				t->i++;
				t->e++;
			}
			if (str[t->i] == '?')
			{
				t->var = malloc(2);
				t->var[0] = '?';
				t->var[1] = '\0';
				t->i++;
				t->h++;
			}
			else
			{
				t->k = t->i;
				while (!find_spliter1(str[t->k], "|>< \t\n\'\""))
				{
					t->k++;
					t->j++;
				}
				t->var = malloc(t->j + 1);
				t->k = t->i;
				t->j = 0;
				while (!find_spliter1(str[t->k], "|>< \t\n\'\""))
				{
					t->var[t->j] = str[t->k];
					t->k++;
					t->j++;
				}
				t->var[t->j] = '\0';
				t->k = 0;
			}
			if ((t->e % 2 != 0 && t->e > 2) || (t->e == 1 && !str[t->i] && t->h != 1))
				t->new_str = ft_strjoin(t->new_str, t->dollar);
			t->valeur = finnd_valeur(env, t->var);
			t->new_str = ft_strjoin(t->new_str, t->valeur);
			while (!find_spliter1(str[t->i], "|>< \t\n\'\""))
				t->i++;
		}
		else
		{
			t->new_str = ft_strjoin(t->new_str, (char[]){str[t->i], 0});
			t->i++;
		}
	}
	free(t->var);
	free(t->dollar);
    if (t->valeur != NULL)
        free(t->valeur);
    str2 = ft_strdup(t->new_str);
    free(t->new_str);
    free(t);
	if (t->quotes == idouble_quotes || t->quotes == isingle_quotes)
	{
		ft_write("syntax error exepected quote");
		return (NULL);
	}
	else
		return (str2);
}

// char	*handle_quotes(char *str, t_mini *env)
// {
// 	int quotes;
// 	char *buffer;
// 	int i;
// 	int k;

// 	quotes = no_quotes;
// 	buffer = NULL;
// 	(void)env;
// 	k = 0;
// 	i = 0;
// 	while (str[i])
// 	{
// 		if (str[i] == '\'' && quotes == no_quotes)
//         {
// 			quotes = isingle_quotes;
//         }
// 		else if (str[i] == '\"' && quotes == no_quotes)
// 		{
// 			quotes = idouble_quotes;
// 		}
// 		else if (str[i] == '\"' && quotes == idouble_quotes)
// 		{

// 			quotes = no_quotes;
// 		}
// 		else if (str[i] == '\'' && quotes == isingle_quotes)
// 		{

// 			quotes = no_quotes;
// 		}
// 		if (str[i] == '$' && quotes != isingle_quotes)
// 		{
// 			buffer = new_expand(str, env);
// 			k++;
// 		}
// 		i++;
// 	}
// 	if (quotes == idouble_quotes || quotes == isingle_quotes)
// 	{
// 		ft_write("syntax error exepected quote");
// 		exit(0);
// 		return (NULL);
// 	}
// 	else if (k)
// 	{
// 		return (buffer);
// 	}
// 	else
// 	{
// 		return (str);
// 	}
// }