/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 02:11:40 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/24 14:15:46 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

/**
 * @fn static int wait_processes(t_data *data)
 * @brief Waits for child processes and extracts the final exit code.
 * @details Loops through the stored child PIDs, waiting for each. 
 * Captures the exit status of the very last command in the pipeline.
 * @param data Pointer to the master data structure.
 * @return     The exit status of the last executed command.
 */
static int	wait_processes(t_data *data)
{
	pid_t	wpid;
	int		exit_code;
	int		status;
	int		i;

	i = -1;
	exit_code = 0;
	while (++i < 2)
	{
		wpid = waitpid(-1, &status, 0);
		if (wpid < 0)
			error_handler(data, WAIT, 1);
		if (wpid == data->pid[1])
		{
			if (WIFEXITED(status))
				exit_code = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				exit_code = 128 + WTERMSIG(status);
		}
	}
	return (exit_code);
}

/**
 * @fn static int pipex(t_data *data, char **argv, char **envp)
 * @brief Forks the processes and manages the execution flow.
 * @details Iterates to fork two child processes, assigning each a command 
 * from the pipeline to execute. Once spawned, the parent closes its file 
 * descriptors and halts execution until the children complete, subsequently 
 * returning the exit status of the final command.
 * @param data Pointer to the master data structure.
 * @param argv Command line arguments offset to the command strings.
 * @param envp The environment variables array.
 * @return     The exit status of the last executed command.
 */
static int	pipex(t_data *data, char **argv, char **envp)
{
	int	i;

	i = -1;
	while (++i < 2)
	{
		data->pid[i] = fork();
		if (data->pid[i] < 0)
			error_handler(data, FORK, 1);
		if (!data->pid[i])
		{
			child(data, i);
			execute(data, argv[i], envp);
		}
	}
	free_data(data);
	return (wait_processes(data));
}

/**
 * @fn int main(int argc, char **argv, char **envp)
 * @brief The main entry point for the pipex program.
 * @details Validates argument count, initializes file descriptors, and 
 * triggers the piping process.
 * @param argc The argument count.
 * @param argv The argument vector strings.
 * @param envp The environment variables.
 * @return     Returns the exit status of the pipex execution.
 */
int	main(int argc, char **argv, char **envp)
{
	t_data	data;
	int		exit_code;

	if (argc != 5)
	{
		write(STDERR_FILENO, "Wrong number of arguments!\n", 27);
		write(STDERR_FILENO, "Usage:\t./pipex file1 cmd1 cmd2 file2\n", 37);
		return (EXIT_FAILURE);
	}
	init(&data, argv);
	argv += 2;
	exit_code = pipex(&data, argv, envp);
	return (exit_code);
}
