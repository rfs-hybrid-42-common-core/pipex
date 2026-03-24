/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 00:37:17 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/23 16:33:53 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

/**
 * @fn static void init_fds(t_data *data, int argc, char **argv)
 * @brief Sets the input and output file descriptors depending on here_doc.
 * @details Evaluates the boolean is_hdoc flag to dictate whether it opens 
 * standard files or configures the output to open in append mode.
 * @param data Pointer to the master data structure.
 * @param argc The command line argument count.
 * @param argv The command line argument strings.
 */
static void	init_fds(t_data *data, int argc, char **argv)
{
	if (data->is_hdoc)
	{
		data->fd.in = -1;
		data->fd.out = safe_open(argv[argc - 1], O_CREAT | O_WRONLY | O_APPEND,
				0644);
		data->n_cmds = argc - 4;
		return ;
	}
	data->fd.in = safe_open(argv[1], O_RDONLY, 0);
	data->fd.out = safe_open(argv[argc - 1], O_CREAT | O_WRONLY | O_TRUNC,
			0644);
	data->n_cmds = argc - 3;
}

/**
 * @fn void init(t_data *data, int argc, char **argv)
 * @brief Orchestrates the initialization of all required data parameters.
 * @details Evaluates the here_doc flag, allocates memory for process IDs, 
 * computes the number of commands and pipes, and delegates file opening.
 * @param data Pointer to the master data structure.
 * @param argc The command line argument count.
 * @param argv The command line argument strings.
 */
void	init(t_data *data, int argc, char **argv)
{
	data->pid = NULL;
	data->fd.in = -1;
	data->fd.out = -1;
	data->pipe_fd[0] = -1;
	data->pipe_fd[1] = -1;
	data->prev_fd = -1;
	data->n_cmds = 0;
	data->n_pipes = 0;
	data->is_hdoc = !ft_strcmp(argv[1], "here_doc");
	init_fds(data, argc, argv);
	data->n_pipes = data->n_cmds - 1;
	data->pid = ft_calloc(data->n_cmds, sizeof(pid_t));
	if (!data->pid)
		error_handler(data, CALLOC, 1);
}
