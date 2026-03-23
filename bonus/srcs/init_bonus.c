/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 00:37:17 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/23 05:10:25 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

/**
 * @fn static void free_pipes(t_data *data, int n)
 * @brief Safely deallocates a specific number of initialized pipe arrays.
 * @details Designed to cleanly back out of partial pipe allocations if a 
 * dynamic allocation fails midway through initialization.
 * @param data Pointer to the master data structure.
 * @param n    The number of successfully allocated pipes to free.
 */
static void	free_pipes(t_data *data, int n)
{
	int	i;

	if (!data->p_fd || n <= 0)
		return ;
	i = -1;
	while (++i < n)
	{
		if (data->p_fd[i])
		{
			if (data->p_fd[i][0] >= 0)
				close(data->p_fd[i][0]);
			if (data->p_fd[i][1] >= 0)
				close(data->p_fd[i][1]);
			free(data->p_fd[i]);
		}
	}
	free(data->p_fd);
	data->p_fd = NULL;
}

/**
 * @fn static void init_pipes(t_data *data)
 * @brief Dynamically allocates and initializes the pipe file descriptors.
 * @details Creates a 2D array representing n_pipes. Handles memory allocation 
 * failures by freeing previously allocated pipes before triggering an error.
 * @param data Pointer to the master data structure.
 */
static void	init_pipes(t_data *data)
{
	int	i;

	if (data->n_pipes <= 0)
		return ;
	data->p_fd = ft_calloc(data->n_pipes, sizeof(int *));
	if (!data->p_fd)
		error_handler(data, CALLOC, 1);
	i = -1;
	while (++i < data->n_pipes)
	{
		data->p_fd[i] = ft_calloc(2, sizeof(int));
		if (!data->p_fd[i])
		{
			free_pipes(data, i);
			error_handler(data, CALLOC, 1);
		}
		data->p_fd[i][0] = -1;
		data->p_fd[i][1] = -1;
		if (pipe(data->p_fd[i]) < 0)
		{
			free_pipes(data, i + 1);
			error_handler(data, PIPE, 1);
		}
	}
}

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
		data->fd.out = safe_open(argv[argc - 1], O_CREAT | O_RDWR | O_APPEND,
				0644);
		data->n_cmds = argc - 4;
		return ;
	}
	data->fd.in = safe_open(argv[1], O_RDONLY, 0);
	data->fd.out = safe_open(argv[argc - 1], O_CREAT | O_RDWR | O_TRUNC, 0644);
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
	data->fd.in = -1;
	data->fd.out = -1;
	data->p_fd = NULL;
	data->pid = NULL;
	data->n_cmds = 0;
	data->n_pipes = 0;
	data->is_hdoc = !ft_strcmp(argv[1], "here_doc");
	init_fds(data, argc, argv);
	data->n_pipes = data->n_cmds - 1;
	data->pid = ft_calloc(data->n_cmds, sizeof(pid_t));
	if (!data->pid)
		error_handler(data, CALLOC, 1);
	init_pipes(data);
}
