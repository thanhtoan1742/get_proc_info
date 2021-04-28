#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define DEV_NAME "get_proc_info"

struct proc_info {
	pid_t pid;
	char name[16];
};

struct procinfos {
	long studentID;
	struct proc_info proc;
	struct proc_info parent_proc;
	struct proc_info oldest_child_proc;
};

void invoke_device() {
	struct procinfos* proc = malloc(sizeof(struct procinfos));

	size_t fd = open(DEV_NAME, O_RDONLY);
	size_t pid = 1;
	read(fd, (char*)(proc), pid);


	printf("process pid, name: %d, %s\n", proc->proc.pid, proc->proc.name);
	printf("process's parent pid, name: %d, %s\n", proc->parent_proc.pid, proc->parent_proc.name);
	printf("process's oldest child pid, name: %d, %s\n", proc->oldest_child_proc.pid, proc->oldest_child_proc.name);

	close(fd);
	free(proc);
}

int main(void) {
	pid_t pid = fork();
	if (pid)
		pid = fork();
	if (pid)
		pid = fork();
	if (pid)
		invoke_device();

	return 0;
}
