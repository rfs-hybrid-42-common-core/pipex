/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 22:11:18 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/24 01:51:36 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

/**
 * @fn static void replace_spaces_in_quotes(char *cmd)
 * @brief Replaces spaces with a non-printable character inside quotes.
 * @details Iterates through the command string. If a space is found inside 
 * unescaped quotes, it is temporarily replaced with ASCII 2 to prevent 
 * ft_split from breaking the argument apart.
 * @param cmd The newly duplicated command string to modify.
 */
static void	replace_spaces_in_quotes(char *cmd)
{
	char	quote;
	int		i;

	quote = 0;
	i = -1;
	while (cmd[++i])
	{
		if (cmd[i] == '\\' && quote != '\'' && cmd[i + 1])
		{
			i++;
			if (cmd[i] == ' ')
				cmd[i] = 2;
		}
		else if ((cmd[i] == '\'' || cmd[i] == '\"') && !quote)
			quote = cmd[i];
		else if (cmd[i] == quote)
			quote = 0;
		else if (cmd[i] == ' ' && quote)
			cmd[i] = 2;
	}
}

/**
 * @fn static void append_char(char *cmd, int i, int *j)
 * @brief Helper function to restore masked spaces and append characters.
 * @details Checks if the current character is the non-printable ASCII 2 
 * mask. If so, it reverts it to a standard space before appending it to 
 * the cleaned string. Otherwise, it appends the character normally.
 * @param cmd The string being cleaned.
 * @param i   The current read index of the string.
 * @param j   Pointer to the current write index of the string.
 */
static void	append_char(char *cmd, int i, int *j)
{
	if (cmd[i] == 2)
		cmd[(*j)++] = ' ';
	else
		cmd[(*j)++] = cmd[i];
}

/**
 * @fn static void clean_single_cmd(char *cmd)
 * @brief Restores spaces and removes quote/escape characters.
 * @details Converts ASCII 2 back to standard spaces. Removes the outer 
 * quotes and processes escape characters to produce the final, clean 
 * string ready for execve.
 * @param cmd A single parsed argument string.
 */
static void	clean_single_cmd(char *cmd)
{
	char	quote;
	int		i;
	int		j;

	quote = 0;
	i = -1;
	j = 0;
	while (cmd[++i])
	{
		if (quote != '\'' && cmd[i] == '\\' && cmd[i + 1])
			append_char(cmd, ++i, &j);
		else if (!quote && (cmd[i] == '\'' || cmd[i] == '\"'))
			quote = cmd[i];
		else if (quote == cmd[i])
			quote = 0;
		else
			append_char(cmd, i, &j);
	}
	cmd[j] = '\0';
}

/**
 * @fn char **parser(t_data *data, const char *str)
 * @brief Parses and cleans the command string into an execve-ready array.
 * @details Duplicates the raw string, masks spaces inside quotes, splits 
 * the string into an array, and then cleans each individual argument.
 * @param data Pointer to the master data structure (used for error handling).
 * @param str  The raw command string provided via argv.
 * @return     A 2D array representing the fully parsed command and its 
 * arguments.
 */
char	**parser(t_data *data, const char *str)
{
	char	**cmd;
	char	*str_copy;
	int		i;

	str_copy = ft_strdup(str);
	if (!str_copy)
		error_handler(data, CALLOC, 1);
	replace_spaces_in_quotes(str_copy);
	cmd = ft_split(str_copy, ' ');
	free(str_copy);
	if (!cmd || !cmd[0] || !*cmd[0])
	{
		print_cmd_error(str, ": command not found\n");
		free_cmd_paths(cmd, NULL);
		error_handler(data, NOT_FOUND, 127);
	}
	i = -1;
	while (cmd[++i])
		clean_single_cmd(cmd[i]);
	return (cmd);
}
