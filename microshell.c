/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlow <marvin@42.fr>                        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/24 16:36:19 by mlow              #+#    #+#             */
/*   Updated: 2024/07/24 17:31:25 by mlow             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static int	write_err(char *msg)
{
	int	index;

	index = -1;
	while (msg[++index])
		write(2, &msg[index], 1);
	return (1);
}

static int	cd(char **argv, int array)//cd $HOME "|"(2)
{
	if (array != 2)
		return (write_err("error: cd: bad arguments\n"));
	if (chdir(argv[1]) == -1)
	{
		write_err("error: cd: cannot change directory to ");
		write_err(argv[1]);
		return (write_err("\n"));
	}
	return (1);
}

static int	ft_exec(char **argv, int array, char **envp)
{
	int	fd[2];
	int	status;
	int	has_pipe;
	int	pid;

	status = 0;
	has_pipe = 0;
	//printf("value of array = %d\n", array);
	if (argv[array] && strcmp(argv[array], "|") == 0)
		has_pipe = 1;
	if (has_pipe == 0 && strcmp(*argv, "cd") == 0)
		return (cd(argv, array));
	if ((has_pipe != 0) && pipe(fd) == -1)
		return (write_err("error: fatal\n"));
	pid = fork();
	if (pid == 0)
	{
		argv[array] = NULL;
		if (has_pipe != 0)
		{
			if (close(fd[0]) == -1 || dup2(fd[1], 1) == -1 || close(fd[1]) == -1)
				exit(write_err("error: fatal\n"));
		}
		if (*argv && strcmp(*argv, "cd") == 0)
			exit(cd(argv, array));
		if (*argv)
		{
			execve(*argv, argv, envp);
			write_err("error: cannot execute ");
			write_err(*argv);
			exit(write_err("\n"));
		}
		exit(1);
	}
	waitpid(pid, &status, 0);
	if (has_pipe != 0)
	{
		if (close(fd[1]) == -1 || dup2(fd[0], 0) == -1 || close(fd[0]) == -1)
			return (write_err("error: fatal\n"));
	}
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (0);
}

int	main(int argc, char **argv, char ** envp)
{
	int	array;
	int	status;

	array = 0;
	status = 0;
	if (argc > 1)
	{
		while (argv[array] && argv[++array])// /bin/echo(0) -n 123 "|"(3)
		{
			argv += array;
			array = 0;
			while (argv[array] && strcmp(argv[array], "|") != 0 
					&& strcmp(argv[array], ";") != 0)
				array++;
			status = ft_exec(argv, array, envp);
		}
	}
	return (status);
}

/*
old and kinda correct one?
static int	write_err(char *msg)
{
	int	index;

	index = -1;
	while (msg[++index])
		write(2, &msg[index], 1);
	return (1);
}

static int	cd(char **argv, int array)
{
	if (array != 2)
		return (write_err("error: cd: bad arguments\n"));
	if (chdir(argv[1]) == -1)
	{
		write_err("error: cd: cannot change directory to ");
		write_err(argv[1]);
		return (write_err("\n"));
	}
	return (1);
}

static int	ft_exec(char **argv, int array, char **envp)
{
	int	fd[2];
	int	status;
	int	has_pipe;
	int	pid;

	status = 0;
	has_pipe = 0;
	//printf("value of array = %d\n", array);
	if (argv[array] && strcmp(argv[array], "|") == 0)
		has_pipe = 1;
	if (has_pipe == 0 && strcmp(*argv, "cd") == 0)
		return (cd(argv, array));
	if ((has_pipe != 0) && pipe(fd) == -1)
		return (write_err("error: fatal\n"));
	pid = fork();
	if (pid == 0)
	{
		argv[array] = NULL;
		//printf("*argv = %s\n", *argv);
		//printf("argv[%d] = %s\n", array, argv[array]);
		//printf("argv[%d] = %s\n", array + 1, argv[array + 1]);
		if (has_pipe != 0)
		{
			if (close(fd[0]) == -1 || dup2(fd[1], 1) == -1 || close(fd[1]) == -1)
				return (write_err("error: fatal\n"));
		}
		//if (strcmp(*argv, "cd") == 0)//dont write (*argv && strcmp(*argv, "cd") == 0)
		if (strcmp(*argv, "cd") == 0)//dont write (*argv && strcmp(*argv, "cd") == 0)
			return (cd(argv, array));
		execve(*argv, argv, envp);
		write_err("error: cannot execute ");
		write_err(*argv);
		return (write_err("\n"));
	}
	waitpid(pid, &status, 0);
	if (has_pipe != 0)
	{
		if (close(fd[1]) == -1 || dup2(fd[0], 0) == -1 || close(fd[0]) == -1)
			return (write_err("error: fatal\n"));
	}
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (0);
}

int	main(int argc, char **argv, char ** envp)
{
	int	array;
	int	status;

	array = 0;
	status = 0;
	if (argc > 1)
	{
		while (argv[array] && argv[++array])
		{
			argv += array;
			array = 0;
			while (argv[array] && strcmp(argv[array], "|") != 0 
					&& strcmp(argv[array], ";") != 0)
				array++;
			status = ft_exec(argv, array, envp);
		}
	}
	return (status);
}
*/
// cc -g microshell
// gdb ./a.out
// (gdb) set follow-fork-mode child
// (gdb) r ";" ";" -r mean run command
// bt
