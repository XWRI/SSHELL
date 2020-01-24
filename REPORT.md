<h3> ECS 150 Programming Assignment 1</h3>
<h4>Xiaowei Min & Wenbai Zhang</h4>
<h4>1/23/2020</h4>

<p> For this assignment, we used a self-defined struct and a self-defined data
structure to help accomplish the task.
</p>

<strong><em>Single Command Function</em></strong>

<p>We first parse the command line. We stores the command(s) in an array. While
parsing the line, we identity the symbols in the command line to differentiate
which category does the current command fits in. After we are done with parsing,
we then process the command based on their category.
</p>

<p> For a single command, we further categorize the commands into two different
types: builtin and not builtin. For the three builtin functions, we mostly
utilized the function getcwd() and chdir() to achieve the desired result.
However, for the rest of the commands, we first forked a new process, then we
execute the external executable in the child process, and the parent process
would wait for the child process to finish.
</p>

<strong><em>Command with symbols</em></strong>

<p> For command line that include symbols, we also further categorize them into two
different types: one is output redirection, the other is piping. For output
redirection, we first check the validity of the input command. For the presence
of necessary arguments, we checked when parsing the line. However, we still
need to check the validity of the output file. Once we have validated the
command, we also fork a new process to write the contents in the command to the
file. The child process writes the content and the parent would wait for it to
finish.
</p>

<p> For the piping, the central idea is that the previous command should output
to the original process's file descriptor STDOUT_FILENO, and the next command
should read from the original process's file descriptor STDIN_FILENO. We also
used a variable to keep track of the correct place to for the next command to
read from. Since there could be multiple commands, we used a for loop to execute
each command. However, we left the last command out of the loop since for the
last command, we only need make sure that it read the correct input and otuput
to the STDOUT_FILENO of the original file descriptor.
</p>

<p> For the error redirection, it is similar to the implementation of output
redirection. We added extra condition when parsing the command line to identity
if the input needs error redirection. Similarly in piping, we first need to be
sure if the error needs to be redirected. If so, we write the error message to
the output of the current command, then the next command will read in the error
message as its input.
</p>

<p> For the feature of directory stack, we used a self-defined data structure
that is implemented based on linked list. The detailed implementation when
changing the directory is similar to cd, except that there is the extra step of
adding the directory to the stack. Displaying the directories stored in the
stack simply needs us to iterate through the stack. However, one thing that we
note is that in the reference program, even without calling pushd, the current
directory is stored in the stack upon calling dirs when the stack is empty.
Thus, we add the current directory to the stack when calling dirs and the stack
is empty. For popd, we first changed to the top directory, then remove the top
directory in the stack.
</p>

<strong><em>Testing</em></strong>

<p> We first test our program with a single command. For the builtin functions,
pwd is pretty straightforward. For cd, we tested with the next argument being
".",  "..", invalid input and valid input. For the rest of the commands, we
tested with ls, date, echo, and other common commands used in terminal.
</p>

<p> For the testing of output redirection, we tried with incomplete command,
including missing output file, missing writing instructions, invalid output
file and multiple commands after the symbol '>'. We tested for the possible
errors and then, for the correct input to see if the correct output is reached.
</p>

<p> For the testing of piping, we first tested our code with only two commands.
After it is working correctly, then we tested for more commands in the input up
to four commands. But we first tested with all correct commands. Therefore, we
tested with incorrect commands afterwards. Then we also tested for a mix of
correct and incorrect inputs and compared the result to the reference program.
Lastly, we tested if the return value for each command is correct.
</p>

<p> For the error redirection, we compared the output when no '&' is there to
that when '&' is there. So the first file would be empty and the error message
should be displayed in the shell. The second file would contain the error
message and nothing would be displayed in the shell. Similarly for the
redirection in piping, we used command such as 'wc' to track the differences
with and without '&'.
</p>

<p> For the directory stack, we tested all three functions when the stack is
empty. Then, we tested to see if pushd actually changed the directory and if the
new directoy has been added to the stack. We also tested if popd actually
changed the directory and removed the top directory in the current stack. Then,
we compared the results to the reference program.
</p>

<p> Overall, this assignment is a bit difficult to understand at first. We feel
that the understanding of the shell is essential to the completion of it.
</p>
