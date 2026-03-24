/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 05:06:52 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/23 21:56:02 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

/**
 * @fn static void redirect_in(t_data *data, const int index)
 * @brief Configures the standard input for the current command.
 * @details Evaluates the command index. If it is the first command, it 
 * redirects the STDIN from the infile and closes the unused pipe read-end. 
 * Otherwise, it redirects from the pipe.
 * @param data  Pointer to the master data structure.
 * @param index The index representing which command is being executed.
 */
static void	redirect_in(t_data *data, const int index)
{
	if (index == 0)
	{
		safe_close(data, &data->pipe_fd[0]);
		if (data->fd.in < 0)
		{
			free_data(data);
			exit(EXIT_FAILURE);
		}
		if (dup2(data->fd.in, STDIN_FILENO) < 0)
			error_handler(data, DUP2, 1);
		safe_close(data, &data->fd.in);
	}
	else
	{
		if (dup2(data->pipe_fd[0], STDIN_FILENO) < 0)
			error_handler(data, DUP2, 1);
		safe_close(data, &data->pipe_fd[0]);
	}
}

/**
 * @fn static void redirect_out(t_data *data, const int index)
 * @brief Configures the standard output for the current command.
 * @details Evaluates the command index. If it is the first command, it 
 * redirects the STDOUT to the write-end of the pipe. Otherwise, it closes 
 * the unused pipe write-end and redirects to the outfile.
 * @param data  Pointer to the master data structure.
 * @param index The index representing which command is being executed.
 */
static void	redirect_out(t_data *data, const int index)
{
	if (index == 0)
	{
		if (dup2(data->pipe_fd[1], STDOUT_FILENO) < 0)
			error_handler(data, DUP2, 1);
		safe_close(data, &data->pipe_fd[1]);
	}
	else
	{
		safe_close(data, &data->pipe_fd[1]);
		if (data->fd.out < 0)
		{
			free_data(data);
			exit(EXIT_FAILURE);
		}
		if (dup2(data->fd.out, STDOUT_FILENO) < 0)
			error_handler(data, DUP2, 1);
		safe_close(data, &data->fd.out);
	}
}

/**
 * @fn void child(t_data *data, const int index)
 * @brief Orchestrates the input and output redirection for a child process.
 * @details Calls modular helper functions to configure STDIN and STDOUT. 
 * Prevents FD leaks by selectively closing the original static file 
 * descriptors (infile/outfile) if they are not actively required by the 
 * specific command being executed.
 * @param data  Pointer to the master data structure.
 * @param index The index representing which command is being executed.
 */
void	child(t_data *data, const int index)
{
	redirect_in(data, index);
	redirect_out(data, index);
	if (index != 0)
		safe_close(data, &data->fd.in);
	if (index != 1)
		safe_close(data, &data->fd.out);
}
