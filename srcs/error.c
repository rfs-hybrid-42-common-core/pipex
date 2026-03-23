/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 21:46:10 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/23 13:44:39 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

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
		perror("calloc failed");
	else if (error == OPEN)
		perror("open failed");
	else if (error == CLOSE)
		perror("close failed");
	else if (error == PIPE)
		perror("pipe failed");
	else if (error == FORK)
		perror("fork failed");
	else if (error == DUP2)
		perror("dup2 failed");
	else if (error == NOT_EXEC)
		perror("found but not executable");
	else if (error == NOT_FOUND)
		write(STDERR_FILENO, "command not found\n", 18);
	if (data)
		free_data(data);
	exit(status_code);
}
