*This project has been created as part of the 42 curriculum by maaugust.*

<div align="center">
  <img src="https://raw.githubusercontent.com/rfs-hybrid/42-Common-Core/main/assets/covers/cover-pipex-bonus.png" alt="Pipex Cover" width="100%" />
</div>

<div align="center">
  <h1>🚰 Pipex: UNIX Mechanisms and Inter-Process Communication</h1>
  <img src="https://img.shields.io/badge/Language-C-blue" />
  <img src="https://img.shields.io/badge/Status-Unevaluated-blue" />
  <img src="https://img.shields.io/badge/Norminette-Passing-success" />
</div>

---

> **A Note on This Repository:** > I built this project simultaneously with [`minitalk`](https://github.com/rfs-hybrid-42-common-core/minitalk), but due to 42's concurrent project limits, I could only submit one for an official grade. I chose to see Pipex through to completion anyway. Not only did it provide immense value in understanding IPC and process management, but it also served as the perfect architectural foundation for the highly complex [`minishell`](https://github.com/rfs-hybrid-42-common-core/minishell) project (which I have since completed). While this repository does not have an official Moulinette score, it was built to a 125% bonus standard. It successfully handles unlimited piped commands, in-memory `here_doc` routing without data races, exact bash-equivalent exit codes, and strict FD management.

## 💡 Description
**Pipex** is a project in the 42 curriculum that explores UNIX mechanisms in detail by recreating the behavior of shell pipes (`|`) and redirections (`<`, `>`, `<<`, `>>`).

The primary goal of this project is to learn how to manipulate file descriptors, handle process creation using `fork()`, and establish inter-process communication (IPC) using `pipe()`. The program intercepts commands, routes their standard input and output through anonymous pipes, and executes them concurrently while properly managing memory and zombie processes.

---

## 📑 Features

### 🔹 Mandatory Features
The mandatory program takes exactly four arguments (two files and two commands) and behaves exactly like the following bash command:
`$< infile cmd1 | cmd2 > outfile`
* Accurately resolves command paths using the environment (`envp`).
* Fully protects against execution failures, locked files, and missing permissions, returning precise bash-like exit codes (126, 127).

### 🚀 Bonus Features
The bonus implementation drastically expands the scope of the program to handle complex, dynamic routing:
* **Multiple Pipes:** Supports an arbitrary, unlimited number of piped commands:
  `$< infile cmd1 | cmd2 | cmd3 | ... | cmdn > outfile`
* **Here Document (`<<`):** Supports `here_doc` input, reading from the standard input until a specific `LIMITER` is found, and appending the final output `>>` to the target file:
  `$ cmd1 << LIMITER | cmd2 | ... | cmdn >> outfile`

---

## 🧠 Algorithm & Data Structure

*Per the core requirements, here is the justification for the architecture used in this project.*

### Data Structure: Centralized `t_data`
To comply with the strict 42 Norm (max 4 parameters per function), state is managed via a central `t_data` structure. It stores the input/output FDs, the dynamic process ID array, and tracks the active pipes, allowing seamless cleanup in case of a fatal error at any point during execution.

### Algorithm: Sliding Window Pipe Logic (Bonus)
Standard approaches to handling multiple pipes often involve allocating a massive 2D array of file descriptors (`int **pipes`). This scales poorly and creates nightmare cleanup scenarios. 
* **The Logic:** This project uses a **Sliding Window** approach. Only *one* active pipe is created per iteration. After the child process forks and duplicates the necessary FDs, the parent process closes the write-end and saves the read-end into `data->prev_fd`. The next command then reads from `prev_fd` instead of a new array index. This keeps the memory footprint infinitely small regardless of the number of commands.

### Architectural Trade-off: In-Memory `here_doc`
In the bonus part, to mimic `here_doc`, standard shells usually write the input to a hidden temporary file (e.g., in `/tmp/`). 
* **The Problem:** Without access to randomizing functions like `mktemp()`, hardcoding a temporary file (e.g., `.heredoc_tmp`) introduces a severe race condition if two instances of the program run simultaneously.
* **The Solution:** This project utilizes an entirely in-memory **anonymous pipe** to buffer the `here_doc` input. While this restricts the maximum input to the UNIX pipe buffer limit (usually 64KB), it strictly prioritizes data integrity and absolute process isolation, guaranteeing zero residual files if the program crashes.

---

## 🛠️ Instructions

### 📦 Installation
To compile the project, run the following commands in the root of the repository.

* To build the **mandatory** executable (`pipex`):
```bash
make
```

* To build the **bonus** executable (`pipex_bonus`):
```bash
make bonus
```

### 🧹 Cleaning
* `make clean`: Removes the compiled object files (`.o`).
* `make fclean`: Removes object files, the executables, and cleans the embedded `libft`.
* `make re`: Performs a clean re-build.

### 💻 Usage

**1. Mandatory Part:**
Pass the infile, two commands, and the outfile.
```bash
./pipex infile "ls -l" "wc -l" outfile
```
*(Equivalent to: `< infile ls -l | wc -l > outfile`)*

**2. Bonus Part (Multiple Pipes):**
Pass an arbitrary number of commands between the files.
```bash
./pipex_bonus infile "grep a" "sort -r" "awk '{print \$1}'" "wc -l" outfile
```
*(Equivalent to: `< infile grep a | sort -r | awk '{print $1}' | wc -l > outfile`)*

**3. Bonus Part (Here Document):**
Use `here_doc` as the first argument, followed by the limiter keyword.
```bash
./pipex_bonus here_doc EOF "grep error" "wc -l" outfile
```
*(Equivalent to: `grep error << EOF | wc -l >> outfile`)*

---

## 🧪 Testing & Edge Cases
Pipex has many hidden traps regarding process management, memory leaks, and exit codes. **Rigorous testing is essential.**

### 1. The Absolute Path & `errno` Trap
When testing absolute paths (e.g., `/usr/bin/invalid_cmd`), many implementations accidentally overwrite the `errno` variable during their error-printing phase (like calling `free()` or `write()`), resulting in a `126` (Permission denied) exit code instead of the expected `127` (Command not found). Ensure your `errno` is saved immediately after `execve` fails to return accurate, bash-like exit codes.

### 2. The 1024 Open FDs Limit
A true test of your bonus multiple-pipe implementation is to pipe hundreds of commands simultaneously (e.g., `cat` piped 500 times). If your architecture hoards file descriptors, it will crash. The sliding window algorithm ensures only the necessary FDs are open at any given time, preventing `EMFILE` (Too many open files) errors.

### 3. Scaling to Minishell: The Sliding Window Advantage
While Pipex handles a linear pipeline, the architectural choice to use a **Sliding Window** for file descriptors was a deliberate preparation for [`minishell`](https://github.com/rfs-hybrid-42-common-core/minishell). In a full shell environment with bonus features, complex user input is parsed via recursive descent into a binary Abstract Syntax Tree (AST). Because pipelines are dynamically generated and deeply nested, you cannot safely rely on fixed 2D arrays to store pipes. By ensuring that only one active pipe is created at a time and the read-end is passed forward via `data->prev_fd`, this exact execution logic can be ported directly into [`minishell`](https://github.com/rfs-hybrid-42-common-core/minishell)'s recursive AST executor. When traversing a `PIPE` node, it effortlessly routes the FDs through the left and right branches, guaranteeing zero FD leaks and preventing exhaustion regardless of pipeline complexity.

### 4. Memory Leak & FD Verification (Valgrind)
Because `pipex` utilizes dynamically allocated arrays for process IDs and environmental path matrices, it is critical to verify memory safety. Furthermore, tracking open File Descriptors is essential, as leaving pipes open will cause the program to hang. Use the `--track-fds=yes` flag in Valgrind to ensure absolute system safety across both mandatory and bonus executables:

**Scenario 1: Mandatory - Invalid Command Leak**

Forces the mandatory program to parse the environment paths, fail to find the binary, free the 2D arrays, and exit securely without executing.
```bash
valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./pipex infile "ls -l" "invalid_command" outfile
```

**Scenario 2: Bonus - Multiple Pipes & Invalid Command**

Checks the bonus logic to ensure the dynamic PID arrays and sliding window pipes don't leak when a middle command fails.
```bash
valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./pipex_bonus infile "ls -l" "invalid_command" "wc -l" outfile
```

**Scenario 3: Bonus - `here_doc` Static Buffer Drain**

Ensures the `get_next_line` static buffers are cleanly freed when the `LIMITER` is reached and that the anonymous memory pipe is closed correctly.
```bash
valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes ./pipex_bonus here_doc EOF "grep a" "wc -l" outfile
```

*Expected Valgrind Output for all scenarios:* `All heap blocks were freed -- no leaks are possible`
`FILE DESCRIPTORS: 3 open at exit. (These are standard 0, 1, 2)`

### 5. Third-Party Testers
To ensure your Pipex handles temporary zombie processes, exact exit codes, and memory leaks perfectly, I highly recommend running your code through these community testers:
* [michmos / 42_pipex_tester](https://github.com/michmos/42_pipex_tester) - Excellent for checking exact bash-equivalent exit codes.
* [bastienkody / pipex_tester](https://github.com/bastienkody/pipex_tester) - Great for catching temporary zombie processes and FD leaks.

> **⚠️ IMPORTANT NOTE FOR BONUS TESTING:** > Both of these community testers assume that running `make bonus` overrides the mandatory `./pipex` binary. Because this repository properly isolates the bonus into a separate `./pipex_bonus` executable, running the testers on the bonus part will initially fail (since they will pass 5+ arguments to the mandatory binary). 
> * **The Workaround:** Temporarily open the `Makefile` and change `B_NAME = pipex_bonus` to `B_NAME = pipex`. Run `make fclean` and `make bonus`. The testers will now correctly evaluate your multiple pipes and `here_doc` logic. **Do not forget to change it back before submitting to Moulinette!**

---

### 🚨 The Norm
Moulinette relies on a program called `norminette` to check if your files comply with the 42 Norm. Every single `.c` and `.h` file must pass this check. If there is a norm error, you will receive a 0.

**The 42 Header:**
Before writing any code, every file must start with the standard 42 header. `norminette` will automatically fail any file missing this specific signature.
```c
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/21 02:11:40 by maaugust          #+#    #+#             */
/*   Updated: 2025/11/25 03:30:20 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
```

Run the following command in the root of your repository to check all your files at once:
```bash
norminette -R CheckForbiddenSourceHeader srcs/ bonus/ includes/
```

---

## 📚 Resources & References

**System Manuals:**
* `man 2 pipe`, `man 2 fork`, `man 2 dup2`, `man 2 execve` - Essential Linux programmer's manuals for understanding process creation, file descriptor duplication, and command execution.

**Articles & Guides:**
* **[Pipex Tutorial - 42 Project (Medium)](https://csnotes.medium.com/pipex-tutorial-42-project-4469f5dd5901):** A comprehensive guide on how the pipex architecture works under the hood.
* **[Pipex: Understanding Pipelines in C (Medium)](https://medium.com/@omimouni33/pipex-the-42-project-understanding-pipelines-in-c-71984b3f2103):** A great article explaining the core concepts and file descriptor routing.
* **[Pipex 42 (Medium)](https://medium.com/@abkabex/pipex-42-009811947a59):** A detailed walkthrough explaining the transition from shell commands to C process management.

**Video Tutorials:**
* **[CodeVault - Unix Processes in C](https://www.youtube.com/playlist?list=PLfqABt5AS4FkW5mOn2Tn9ZZLLDwA3kZUY):** An absolute goldmine playlist for understanding forks, pipes, and wait functions.

**42 Standards:**
* **[42 Norm V4](https://cdn.intra.42.fr/pdf/pdf/96987/en.norm.pdf):** The strict coding standard for 42 C projects.
* **[Official 42 Norminette Repository](https://github.com/42School/norminette):** The open-source linter enforcing the strict 42 coding standard.

### 🤖 AI Usage & Transparency
In the spirit of transparency and the learning objectives of the 42 curriculum, here is how AI tools were utilized during this project:

* **Conceptual Validation:** Used as a sounding board to verify architectural trade-offs, such as analyzing the race conditions of temporary files versus the UNIX buffer limits of anonymous pipes for the `here_doc` implementation.
* **Documentation & Formatting:** Assisted in drafting comprehensive Doxygen comments for the source files, reviewing general code modularity, and structuring this `README.md` layout.
* **Zero Code Generation:** **No core logic was generated by AI**. All file descriptor management, process forking, inter-process communication, wait loops, and execution routing were 100% manually coded. This strict boundary was maintained to ensure a fundamental, hands-on mastery of UNIX system calls and IPC.
