/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 05:00:25 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/23 13:53:51 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

/**
 * @fn static void redirect_in(t_data *data, const int index)
 * @brief Configures the standard input for the current command.
 * @details Evaluates the command index. If it is the first command, it 
 * redirects the STDIN from the infile. Otherwise, it redirects from the 
 * read-end of the previous pipe.
 * @param data Pointer to the master data structure.
 * @param index  The index representing which command is being executed.
 */
static void	redirect_in(t_data *data, const int index)
{
	if (index == 0)
	{
		if (data->fd.in < 0)
			exit(EXIT_FAILURE);
		if (dup2(data->fd.in, STDIN_FILENO) < 0)
			error_handler(data, DUP2, 1);
		safe_close(data, &data->fd.in);
	}
	else
	{
		if (dup2(data->p_fd[index - 1][0], STDIN_FILENO) < 0)
			error_handler(data, DUP2, 1);
		safe_close(data, &data->p_fd[index - 1][0]);
	}
}

/**
 * @fn static void redirect_out(t_data *data, const int index)
 * @brief Configures the standard output for the current command.
 * @details Evaluates the command index. If it is the last command, it 
 * redirects the STDOUT to the outfile. Otherwise, it redirects to the 
 * write-end of the current pipe.
 * @param data  Pointer to the master data structure.
 * @param index The index representing which command is being executed.
 */
static void	redirect_out(t_data *data, const int index)
{
	if (index < data->n_cmds - 1)
	{
		if (dup2(data->p_fd[index][1], STDOUT_FILENO) < 0)
			error_handler(data, DUP2, 1);
		safe_close(data, &data->p_fd[index][1]);
	}
	else
	{
		if (data->fd.out < 0)
			exit(EXIT_FAILURE);
		if (dup2(data->fd.out, STDOUT_FILENO) < 0)
			error_handler(data, DUP2, 1);
		safe_close(data, &data->fd.out);
	}
}

/**
 * @fn void child(t_data *data, const int index)
 * @brief Orchestrates the input and output redirection for a child process.
 * @details Calls modular helper functions to configure STDIN and STDOUT. 
 * Closes unused pipe file descriptors to prevent hanging, and prevents FD 
 * leaks by ensuring inherited static FDs (infile/outfile) not required by 
 * this specific command are closed before execution.
 * @param data  Pointer to the master data structure.
 * @param index The index representing which command is being executed.
 */
void	child(t_data *data, const int index)
{
	redirect_in(data, index);
	redirect_out(data, index);
	close_pipes(data, index);
	if (index != 0)
		safe_close(data, &data->fd.in);
	if (index != data->n_cmds - 1)
		safe_close(data, &data->fd.out);
}
