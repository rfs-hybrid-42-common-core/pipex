/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 03:46:59 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/23 05:13:09 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

/**
 * @fn static void free_cmd_paths(char **cmd, char **paths)
 * @brief Frees dynamically allocated 2D arrays.
 * @details Safely iterates through command argument arrays and path string 
 * arrays, freeing individual elements and the main pointers.
 * @param cmd   The 2D array of command arguments to free (can be NULL).
 * @param paths The 2D array of environment paths to free (can be NULL).
 */
static void	free_cmd_paths(char **cmd, char **paths)
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
 * @fn static void try_exec_absolute_relative(t_data *data, char **cmd, 
 * char **envp)
 * @brief Attempts to execute a command specified by an absolute/relative path.
 * @details Directly calls execve. If it fails due to ENOENT, throws a 127 
 * (Not Found) error. Otherwise, throws a 126 (Permission/Execution) error.
 * @param data Pointer to the master data structure.
 * @param cmd  Array of command arguments where cmd[0] is the direct path.
 * @param envp The environment variables.
 */
static void	try_exec_absolute_relative(t_data *data, char **cmd, char **envp)
{
	errno = 0;
	execve(cmd[0], cmd, envp);
	free_cmd_paths(cmd, NULL);
	if (errno == ENOENT)
		error_handler(data, NOT_FOUND, 127);
	error_handler(data, NOT_EXEC, 126);
}

/**
 * @fn static char *get_cmd_path(t_data *data, char **cmd, char **paths)
 * @brief Searches the environment paths to find the valid executable.
 * @details Appends a '/' to each path, concatenates the command name, and 
 * checks its accessibility using access() with X_OK.
 * @param data  Pointer to the master data structure.
 * @param cmd   Array of command arguments where cmd[0] is the command name.
 * @param paths Array of available system PATH directories.
 * @return      A dynamically allocated string containing the valid execution 
 * path, or NULL if not found.
 */
static char	*get_cmd_path(t_data *data, char **cmd, char **paths)
{
	char	*full_path;
	char	*partial_path;
	int		i;

	i = -1;
	while (paths[++i])
	{
		partial_path = ft_strjoin(paths[i], "/");
		full_path = ft_strjoin(partial_path, cmd[0]);
		free(partial_path);
		if (!full_path)
		{
			free_cmd_paths(cmd, paths);
			error_handler(data, CALLOC, 1);
		}
		if (access(full_path, X_OK) == 0)
			return (full_path);
		free(full_path);
	}
	return (NULL);
}

/**
 * @fn static void run_cmd(t_data *data, char **cmd, char *cmd_path, 
 * char **envp)
 * @brief Executes the fully resolved command path.
 * @details Calls execve with the resolved path. If execve fails, it cleans up 
 * memory and returns the proper exit code (127 for missing, 126 for denied).
 * @param data     Pointer to the master data structure.
 * @param cmd      Array of command arguments.
 * @param cmd_path The resolved absolute path to the binary.
 * @param envp     The environment variables.
 */
static void	run_cmd(t_data *data, char **cmd, char *cmd_path, char **envp)
{
	if (!cmd_path)
	{
		free_cmd_paths(cmd, NULL);
		error_handler(data, NOT_FOUND, 127);
	}
	errno = 0;
	execve(cmd_path, cmd, envp);
	free_cmd_paths(cmd, NULL);
	free(cmd_path);
	cmd_path = NULL;
	if (errno == ENOENT)
		error_handler(data, NOT_FOUND, 127);
	error_handler(data, NOT_EXEC, 126);
}

/**
 * @fn void execute(t_data *data, const char *str, char **envp)
 * @brief Parses a command string and routes it for execution.
 * @details Splits the command string. If it contains a slash, routes to 
 * relative/absolute execution. Otherwise, fetches the PATH variable, resolves 
 * the binary path, and executes it.
 * @param data Pointer to the master data structure.
 * @param str  The raw command string provided via argv.
 * @param envp The environment variables.
 */
void	execute(t_data *data, const char *str, char **envp)
{
	char	**cmd;
	char	**paths;
	char	*cmd_path;

	cmd = ft_split(str, ' ');
	if (!cmd || !cmd[0] || !*cmd[0])
	{
		free_cmd_paths(cmd, NULL);
		error_handler(data, NOT_FOUND, 127);
	}
	if (ft_strchr(cmd[0], '/'))
		try_exec_absolute_relative(data, cmd, envp);
	paths = ft_get_path("PATH", envp);
	if (!paths)
	{
		free_cmd_paths(cmd, NULL);
		error_handler(data, NOT_FOUND, 127);
	}
	cmd_path = get_cmd_path(data, cmd, paths);
	free_cmd_paths(NULL, paths);
	run_cmd(data, cmd, cmd_path, envp);
}
