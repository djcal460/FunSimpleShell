# Simple-Shell

### UNIX Shell and History Feature

Simple-Shell was developed in c language to demonstrate how processes are forked from the parent and executed in a shell terminal. This cli allows for children to be forked and executed either concurrently by using the '&' at the tail of the command, or the default is to have the parent wait for the child to finish executing. The tricky part was realizing that for concurrent runs with the ampersand specified, I'd have to throw all the noise from stdout and stderr to the null device, so that the parent could get back to the prompt. Afterall, we send that child to the background because we want it to shut up while we continue working. So, in this little nubbin, that child be quiet. 

This little boo is a homework assignment from Operating System Concepts. They don't provide solutions, so this is my own OG code. An additional nub for this project was to implement a history feature much like the one UNIX systems provide (!n runs the Nth command and !! runs the most recent command). And it is implemented as such.  

## The Skinny
This was all accomplished by tokenizing input
* If a shell command, then a child process is forked and executes while the parent waits. 
* If shell command + '&', then the parent loops back to the prompt and child becomes background proc. 
* If 'history', then a list containing historical commands is displayed. 
* If '!n' or '!!', then child processes are forked to executing that particular historical command. 

# Example Input
```sh
osh > cat /etc/hosts
 127.0.0.1			localhost
osh > history
 1  uname -a
 2  clear
 3  ssh root@192.168.1.50
 4  exit
 5  ls
 6  echo "Hello"
 7  history
osh > !6
 Hello
osh > !!
 Hello
osh > ls -la &
osh >
```

### Tech

Tech uses the very very very advanced terminal shell and gcc compiler.

### Installation

You don't need a Makefile for this Boo, it's only one file. Don't be lazy and give it a run from gcc.

```sh
$ gcc -o fun-simple-shell fun-simple-shell.c
```
