/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 03:46:59 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/25 16:41:35 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

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
	int	saved_errno;

	execve(cmd[0], cmd, envp);
	saved_errno = errno;
	print_sys_error(cmd[0]);
	free_cmd_paths(cmd, NULL);
	if (saved_errno == ENOENT)
		error_handler(data, NOT_FOUND, 127);
	error_handler(data, NOT_EXEC, 126);
}

/**
 * @fn static char **get_all_paths(const char *var, char **envp)
 * @brief Extracts a specific variable from the environment array.
 * @details Searches for 'var' (e.g., "PATH"). Once found, it skips the 
 * variable name and the '=' sign, splitting the rest of the string by ':'.
 * @param var  The environment variable to search for.
 * @param envp The full array of environment variables.
 * @return     A 2D array of the separated paths, or NULL if not found.
 */
static char	**get_all_paths(const char *var, char **envp)
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
 * @fn static char *get_cmd_path(t_data *data, char **cmd, char **paths)
 * @brief Searches the environment paths to find the valid executable.
 * @details Appends a '/' to each path, concatenates the command name, and 
 * checks its accessibility using access() with F_OK.
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
		if (access(full_path, F_OK) == 0)
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
	int	saved_errno;

	if (!cmd_path)
	{
		print_cmd_error(cmd[0], ": command not found\n");
		free_cmd_paths(cmd, NULL);
		error_handler(data, NOT_FOUND, 127);
	}
	execve(cmd_path, cmd, envp);
	saved_errno = errno;
	print_sys_error(cmd[0]);
	free_cmd_paths(cmd, NULL);
	free(cmd_path);
	if (saved_errno == ENOENT)
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

	cmd = parser(data, str);
	if (ft_strchr(cmd[0], '/'))
		try_exec_absolute_relative(data, cmd, envp);
	paths = get_all_paths("PATH", envp);
	if (!paths)
	{
		print_cmd_error(cmd[0], ": command not found\n");
		free_cmd_paths(cmd, NULL);
		error_handler(data, NOT_FOUND, 127);
	}
	cmd_path = get_cmd_path(data, cmd, paths);
	free_cmd_paths(NULL, paths);
	run_cmd(data, cmd, cmd_path, envp);
}
