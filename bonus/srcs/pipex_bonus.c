/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 02:11:40 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/24 14:16:44 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

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
	while (++i < data->n_cmds)
	{
		wpid = waitpid(-1, &status, 0);
		if (wpid < 0)
			error_handler(data, WAIT, 1);
		if (wpid == data->pid[data->n_cmds - 1])
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
 * @brief Forks the processes and manages the dynamic execution flow.
 * @details Iterates to fork a child process for every command requested. 
 * It closes all unused resources in the parent process and waits for 
 * execution to finish before returning the pipeline's exit code.
 * @param data Pointer to the master data structure.
 * @param argv Command line arguments offset to the command strings.
 * @param envp The environment variables array.
 * @return     The exit status of the pipeline execution.
 */
static int	pipex(t_data *data, char **argv, char **envp)
{
	int	i;

	i = -1;
	while (++i < data->n_cmds)
	{
		if (i < data->n_cmds - 1 && pipe(data->pipe_fd) < 0)
			error_handler(data, PIPE, 1);
		data->pid[i] = fork();
		if (data->pid[i] < 0)
			error_handler(data, FORK, 1);
		if (!data->pid[i])
		{
			child(data, i);
			execute(data, argv[i], envp);
		}
		safe_close(data, &data->prev_fd);
		if (i < data->n_cmds - 1)
		{
			safe_close(data, &data->pipe_fd[1]);
			data->prev_fd = data->pipe_fd[0];
			data->pipe_fd[0] = -1;
		}
	}
	return (wait_processes(data));
}

/**
 * @fn int main(int argc, char **argv, char **envp)
 * @brief The main entry point for the pipex bonus program.
 * @details Validates argument count based on whether here_doc is used, 
 * parses input routing, initiates dynamic execution, and captures the code.
 * @param argc The argument count.
 * @param argv The argument vector strings.
 * @param envp The environment variables.
 * @return     Returns the exit status of the final executed command.
 */
int	main(int argc, char **argv, char **envp)
{
	t_data	data;
	int		exit_code;

	if (argc < 5 || (argc < 6 && argv[1] && !ft_strcmp(argv[1], "here_doc")))
	{
		write(STDERR_FILENO, "Wrong number of arguments!\n", 27);
		if (argc < 5)
			write(STDERR_FILENO,
				"Usage: ./pipex file1 cmd1 cmd2 ... cmdn file2\n", 46);
		else
			write(STDERR_FILENO,
				"Usage: ./pipex here_doc LIMIT cmd1 cmd2 ... cmdn file2\n", 55);
		return (EXIT_FAILURE);
	}
	init(&data, argc, argv);
	argv += 2;
	if (data.is_hdoc)
		heredoc(&data, *argv++);
	exit_code = pipex(&data, argv, envp);
	free_data(&data);
	return (exit_code);
}
