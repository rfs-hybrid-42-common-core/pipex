/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 02:24:22 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/24 02:02:36 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

/**
 * @fn void free_cmd_paths(char **cmd, char **paths)
 * @brief Frees dynamically allocated 2D arrays.
 * @details Safely iterates through command argument arrays and path string 
 * arrays, freeing individual elements and the main pointers.
 * @param cmd   The 2D array of command arguments to free (can be NULL).
 * @param paths The 2D array of environment paths to free (can be NULL).
 */
void	free_cmd_paths(char **cmd, char **paths)
{
	int	i;

	if (cmd)
	{
		i = -1;
		while (cmd[++i])
			free(cmd[i]);
		free(cmd);
	}
	if (paths)
	{
		i = -1;
		while (paths[++i])
			free(paths[i]);
		free(paths);
	}
}

/**
 * @fn int safe_open(const char *file, int flags, mode_t mode)
 * @brief Safely opens a file with error reporting.
 * @details Wraps the open() syscall. If the file fails to open, it 
 * prints the system error without exiting, simulating bash behavior.
 * @param file  The path/name of the file to open.
 * @param flags The file access modes (e.g., O_RDONLY).
 * @param mode  The permissions to set if the file is created.
 * @return      The file descriptor, or -1 on failure.
 */
int	safe_open(const char *file, int flags, mode_t mode)
{
	int	fd;

	fd = open(file, flags, mode);
	if (fd < 0)
		print_sys_error(file);
	return (fd);
}

/**
 * @fn void safe_close(t_data *data, int *fd)
 * @brief Safely closes a file descriptor.
 * @details Checks if the descriptor is valid (>= 0) before closing. Sets the 
 * variable to -1 to prevent double-closing. Triggers an error on failure.
 * @param data Pointer to the master data structure.
 * @param fd   Pointer to the file descriptor integer.
 */
void	safe_close(t_data *data, int *fd)
{
	int	tmp;

	if (*fd >= 0)
	{
		tmp = *fd;
		*fd = -1;
		if (close(tmp) < 0)
			error_handler(data, CLOSE, 1);
	}
}

/**
 * @fn void free_data(t_data *data)
 * @brief Cleans up dynamically allocated memory and file descriptors.
 * @details Safely closes the static FDs, any active pipe ends, and the 
 * previous pipe's read-end from the sliding window. Finally, it frees the 
 * PID array.
 * @param data Pointer to the master data structure.
 */
void	free_data(t_data *data)
{
	safe_close(data, &data->fd.in);
	safe_close(data, &data->fd.out);
	safe_close(data, &data->pipe_fd[0]);
	safe_close(data, &data->pipe_fd[1]);
	safe_close(data, &data->prev_fd);
	if (data->pid)
	{
		free(data->pid);
		data->pid = NULL;
	}
	data->n_cmds = 0;
	data->n_pipes = 0;
}
