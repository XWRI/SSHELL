
all: sshell

sshell: sshell.c
	gcc -g -Wall -Wextra -Werror -o sshell sshell.c

clean:
	rm -f sshell