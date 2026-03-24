/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 21:46:10 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/24 02:02:36 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

/**
 * @fn void print_sys_error(const char *msg)
 * @brief Prints a standard system error message with the program prefix.
 * @details Prepends the program name ("pipex: ") to standard error, then 
 * utilizes perror() to automatically append the system's errno description. 
 * This ensures standard system errors (like locked files or missing 
 * directories) perfectly match bash's output format.
 * @param msg The context string (usually a filename or command) to be 
 * printed before the system error description.
 */
void	print_sys_error(const char *msg)
{
	write(STDERR_FILENO, "pipex: ", 7);
	perror(msg);
}

/**
 * @fn void print_cmd_error(const char *cmd, const char *msg)
 * @brief Prints a formatted error message to standard error.
 * @details Prepends the program name, then concatenates the command name 
 * with the specific error message to perfectly mimic standard bash output.
 * @param cmd The command name that triggered the error.
 * @param msg The error description string.
 */
void	print_cmd_error(const char *cmd, const char *msg)
{
	write(STDERR_FILENO, "pipex: ", 7);
	write(STDERR_FILENO, cmd, ft_strlen(cmd));
	write(STDERR_FILENO, msg, ft_strlen(msg));
}

/**
 * @fn void error_handler(t_data *data, const t_error error, int status_code)
 * @brief Main centralized error reporting and cleanup function.
 * @details Prints the appropriate system error message based on the provided 
 * error enum. Cleans up file descriptors via free_data before exiting the 
 * program with the specified status code.
 * @param data        Pointer to the master data structure for cleanup.
 * @param error       The enum specifying which error occurred.
 * @param status_code The integer exit code to terminate the process with.
 */
void	error_handler(t_data *data, const t_error error, int status_code)
{
	if (error == CALLOC)
		print_sys_error("calloc");
	else if (error == OPEN)
		print_sys_error("open");
	else if (error == CLOSE)
		print_sys_error("close");
	else if (error == WRITE)
		print_sys_error("write");
	else if (error == PIPE)
		print_sys_error("pipe");
	else if (error == FORK)
		print_sys_error("fork");
	else if (error == DUP2)
		print_sys_error("dup2");
	if (data)
		free_data(data);
	exit(status_code);
}
