/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 00:37:17 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/23 16:27:35 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

/**
 * @fn void init(t_data *data, char **argv)
 * @brief Initializes the data structure and opens input/output files.
 * @details Opens the files without exiting on failure, simulating standard 
 * bash behavior. It also initializes the pipe.
 * @param data Pointer to the master data structure.
 * @param argv Command line arguments containing the filenames.
 */
void	init(t_data *data, char **argv)
{
	data->fd.in = safe_open(argv[1], O_RDONLY, 0);
	data->fd.out = safe_open(argv[4], O_CREAT | O_WRONLY | O_TRUNC, 0644);
	data->pipe_fd[0] = -1;
	data->pipe_fd[1] = -1;
	if (pipe(data->pipe_fd) < 0)
		error_handler(data, PIPE, 1);
}
