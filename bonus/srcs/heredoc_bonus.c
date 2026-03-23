/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_bonus.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 21:46:51 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/23 05:08:46 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

/**
 * @fn void heredoc(t_data *data, const char *limiter)
 * @brief Processes the here_doc input and writes it to a pipe.
 * @details Reads continuously from standard input until the specified 
 * limiter string is encountered on a line by itself. Writes the accumulated 
 * lines into a pipe, which is then assigned as the primary input file.
 * @param data    Pointer to the master data structure.
 * @param limiter The delimiter string that stops the reading process.
 */
void	heredoc(t_data *data, const char *limiter)
{
	char	*line;
	int		hdoc_fd[2];
	size_t	limiter_len;

	if (pipe(hdoc_fd) == -1)
		error_handler(data, PIPE, 1);
	limiter_len = ft_strlen(limiter);
	line = get_next_line(STDIN_FILENO);
	while (line)
	{
		if (!ft_strncmp(line, limiter, limiter_len)
			&& (line[limiter_len] == '\n' || line[limiter_len] == '\0'))
			break ;
		if (write(hdoc_fd[1], line, ft_strlen(line)) < 0)
		{
			free(line);
			safe_close(data, &hdoc_fd[1]);
			error_handler(data, WRITE, 1);
		}
		free(line);
		line = get_next_line(STDIN_FILENO);
	}
	free(line);
	safe_close(data, &hdoc_fd[1]);
	data->fd.in = hdoc_fd[0];
}
