/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 21:46:51 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/24 05:41:01 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

/**
 * @fn static void drain_gnl(char *line)
 * @brief Drains standard input to cleanly free get_next_line's static buffer.
 * @details Continuously reads and frees lines until EOF is reached, preventing 
 * memory leaks from abandoned static variables after the limiter is found.
 * @param line The current line string to be freed.
 */
static void	drain_gnl(char *line)
{
	while (line)
	{
		free(line);
		line = get_next_line(STDIN_FILENO);
	}
}

/**
 * @fn static void write_heredoc(t_data *data, int write_fd, const char *limiter)
 * @brief Helper function to read from STDIN and write to a pipe.
 * @details Isolates the continuous reading and writing loop for modularity. 
 * Reads input line by line from standard input, checking against the 
 * delimiter, and writes valid lines to the designated pipe. If a write 
 * error occurs, it flushes STDIN to prevent static buffer memory leaks.
 * @param data     Pointer to the master data structure.
 * @param write_fd The write-end file descriptor of the here_doc pipe.
 * @param limiter  The delimiter string that stops the reading process.
 */
static void	write_heredoc(t_data *data, int hdoc_fd, const char *limiter)
{
	char	*line;
	size_t	len;

	len = ft_strlen(limiter);
	while (1)
	{
		line = get_next_line(STDIN_FILENO);
		if (!line || (!ft_strncmp(line, limiter, len)
				&& (line[len] == '\n' || line[len] == '\0')))
			break ;
		if (write(hdoc_fd, line, ft_strlen(line)) < 0)
		{
			safe_close(data, &hdoc_fd);
			drain_gnl(line);
			error_handler(data, WRITE, 1);
		}
		free(line);
	}
	drain_gnl(line);
}

/**
 * @fn void heredoc(t_data *data, const char *limiter)
 * @brief Processes the here_doc input and routes it through an anonymous pipe.
 * @details Creates a pipe to handle here_doc input purely in memory, avoiding 
 * disk I/O and race conditions. Delegates the writing process to the pipe's 
 * write-end, then assigns the read-end to data->fd.in for the first command.
 * @param data    Pointer to the master data structure.
 * @param limiter The delimiter string that stops the reading process.
 */
void	heredoc(t_data *data, const char *limiter)
{
	int	hdoc_fd[2];

	if (pipe(hdoc_fd) < 0)
		error_handler(data, PIPE, 1);
	write_heredoc(data, hdoc_fd[1], limiter);
	safe_close(data, &hdoc_fd[1]);
	data->fd.in = hdoc_fd[0];
}
