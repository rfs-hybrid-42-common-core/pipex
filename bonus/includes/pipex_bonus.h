/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 02:10:43 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/24 02:02:36 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_BONUS_H
# define PIPEX_BONUS_H

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- External Libraries --------------------------- */
# include "libft.h"

/* --------------------------- Internal Libraries --------------------------- */
# include <errno.h>
# include <fcntl.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <unistd.h>

/* ========================================================================== */
/* DATA STRUCTURES                                                            */
/* ========================================================================== */

/**
 * @enum e_error
 * @brief Defines custom error codes for the application.
 * @details Used to categorize the type of failure so the error handler can 
 * print the correct perror message before exiting or cleaning up.
 * @var CALLOC    Memory allocation failure.
 * @var OPEN      File opening failure.
 * @var CLOSE     File closing failure.
 * @var WRITE     File writing failure.
 * @var PIPE      Pipe creation failure.
 * @var FORK      Process forking failure.
 * @var WAIT      Process waiting failure.
 * @var DUP2      File descriptor duplication failure.
 * @var NOT_EXEC  Command was found but lacks execution permissions.
 * @var NOT_FOUND Command could not be found in the system PATH.
 */
typedef enum e_error
{
	CALLOC,
	OPEN,
	CLOSE,
	WRITE,
	PIPE,
	FORK,
	WAIT,
	DUP2,
	NOT_EXEC,
	NOT_FOUND
}	t_error;

/**
 * @struct s_fd
 * @brief Holds the file descriptors for the input and output files.
 * @details Keeps track of the static file descriptors opened from the given 
 * arguments, abstracting them away from the main data structure to keep 
 * the data grouping clean and logical.
 * @var in  The file descriptor for the input file (infile or heredoc pipe).
 * @var out The file descriptor for the output file (outfile).
 */
typedef struct s_fd
{
	int	in;
	int	out;
}	t_fd;

/**
 * @struct s_data
 * @brief Master data structure for the Pipex Bonus program.
 * @details Stores child PIDs, file descriptors, and dynamic pipe structures 
 * needed to handle an arbitrary number of commands.
 * @var pid     Dynamically allocated array of child process IDs.
 * @var fd      Struct containing the input and output file descriptors.
 * @var pipe_fd Array storing the read/write ends of multiple pipes.
 * @var prev_fd The read-end of the previous pipe, saved for the next command.
 * @var n_cmds  The total number of commands to execute.
 * @var n_pipes The total number of pipes required (n_cmds - 1).
 * @var is_hdoc Boolean flag indicating if a here_doc is being utilized.
 */
typedef struct s_data
{
	pid_t	*pid;
	t_fd	fd;
	int		pipe_fd[2];
	int		prev_fd;
	int		n_cmds;
	int		n_pipes;
	bool	is_hdoc;
}	t_data;

/* ========================================================================== */
/* FUNCTIONS                                                                  */
/* ========================================================================== */

/* ----------------------------- Initialization ----------------------------- */
void	init(t_data *data, int argc, char **argv);

/* ---------------------------- Execution Logic ----------------------------- */
void	execute(t_data *data, const char *str, char **envp);
void	child(t_data *data, const int index);
char	**parser(t_data *data, const char *str);

/* -------------------------- File & FD Utilities --------------------------- */
int		safe_open(const char *file, int flags, mode_t mode);
void	safe_close(t_data *data, int *fd);

/* -------------------------------- Heredoc --------------------------------- */
void	heredoc(t_data *data, const char *limiter);

/* ------------------------ Cleanup & Error Handling ------------------------ */
void	print_sys_error(const char *str);
void	print_cmd_error(const char *cmd, const char *msg);
void	error_handler(t_data *data, const t_error error, int status_code);
void	free_cmd_paths(char **cmd, char **paths);
void	free_data(t_data *data);

#endif
