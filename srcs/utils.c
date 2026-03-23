/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 02:24:22 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/23 04:10:17 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

/**
 * @fn char **ft_get_path(const char *var, char **envp)
 * @brief Extracts a specific variable from the environment array.
 * @details Searches for 'var' (e.g., "PATH"). Once found, it skips the 
 * variable name and the '=' sign, splitting the rest of the string by ':'.
 * @param var  The environment variable to search for.
 * @param envp The full array of environment variables.
 * @return     A 2D array of the separated paths, or NULL if not found.
 */
char	**ft_get_path(const char *var, char **envp)
{
	size_t	len;
	int		i;

	if (!envp)
		return (NULL);
	len = ft_strlen(var);
	i = -1;
	while (envp[++i])
	{
		if (!ft_strncmp(envp[i], var, len) && envp[i][len] == '=')
			return (ft_split(envp[i] + len + 1, ':'));
	}
	return (NULL);
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
		perror(file);
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
	if (*fd >= 0)
	{
		if (close(*fd) < 0)
			error_handler(data, CLOSE, 1);
		*fd = -1;
	}
}

/**
 * @fn void free_data(t_data *data)
 * @brief Cleans up remaining open file descriptors.
 * @details Safely closes the input, output, and both pipe file descriptors 
 * to ensure no resource leaks before the program terminates.
 * @param data Pointer to the master data structure.
 */
void	free_data(t_data *data)
{
	safe_close(data, &data->fd.in);
	safe_close(data, &data->fd.out);
	safe_close(data, &data->p_fd[0]);
	safe_close(data, &data->p_fd[1]);
}
