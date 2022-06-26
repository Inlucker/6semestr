#include <dirent.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>


#define BUF_SIZE 10000

#define ACCESS_ERROR 1

typedef struct {
    uint64_t pfn : 55;
    unsigned int soft_dirty : 1;
    unsigned int file_page : 1;
    unsigned int swapped : 1;
    unsigned int present : 1;
} PagemapEntry;

const char* WITH_DESCR[] = {
  "(1) pid                    %20s (The process ID).\n",
  "(2) comm                   %20s (The filename of the executable, in "
  "parentheses).\n",
  "(3) state                  %20s (one of the characters indicating process "
  "state).\n",
  "(4) ppid                   %20s (The PID of the parent of this process).\n",
  "(5) pgrp                   %20s (The process group ID of the process).\n",
  "(6) session                %20s (The session ID of the process).\n",
  "(7) tty_nr                 %20s (The controlling terminal of the "
  "process).\n",
  "(8) tpgid                  %20s (The ID of the foreground process group of "
  "the controlling terminal of the process).\n",
  "(9) flags                  %20s (The kernel flags word of the process).\n",
  "(10) minflt                %20s (The number of minor faults the process has "
  "made which have not required loading a memory page from disk).\n",
  "(11) cminflt               %20s (The number of minor faults that the "
  "process's waited-for children have made).\n",
  "(12) majflt                %20s (The number of major faults the process has "
  "made which have required  loading  a  memory  page from disk).\n",
  "(13) cmajflt               %20s (The number of major faults that the "
  "process's waited-for children have made).\n",
  "(14) utime                 %20s (Amount of time that this process has been "
  "scheduled in user mode, measured in clock ticks).\n",
  "(15) stime                 %20s (Amount of time that this process has been "
  "scheduled in kernel mode, measured in  clock  ticks).\n",
  "(16) cutime                %20s (Amount of time that this process's "
  "waited-for children have been scheduled in user mode, measured in clock "
  "ticks).\n",
  "(17) cstime                %20s (Amount of time that this process's "
  "waited-for children have been scheduled in kernel mode, measured in clock "
  "ticks).\n",
  "(18) priority              %20s (For processes running a real-time "
  "scheduling policy).\n",
  "(19) nice                  %20s (The nice value (see setpriority(2)), a "
  "value in the range 19 (low priority) to -20 (high priority)).\n",
  "(20) num_threads           %20s (Number of threads in this process (since "
  "Linux 2.6). Before kernel 2.6, this field was hardcoded to 0 as a "
  "placeholder for an earlier removed field.\n",
  "(21) itrealvalue           %20s (The time in jiffies before the next "
  "SIGALRM is sent to the process due to an interval timer).\n",
  "(22) starttime             %20s (The time the process started after system "
  "boot).\n",
  "(23) vsize                 %20s (Virtual memory size in bytes).\n",
  "(24) rss                   %20s (Resident Set Size: number of pages the "
  "process has in real memory).\n",
  "(25) rsslim                %20s (Current soft limit in bytes on the rss of "
  "the process).\n",
  "(26) startcode             %20s (The address above which program text can "
  "run).\n",
  "(27) endcode               %20s (The address below which program text can "
  "run).\n",
  "(28) startstack            %20s (The address of the start (i).e., bottom) "
  "of the stack.\n",
  "(29) kstkesp               %20s (The current value of ESP (stack pointer), "
  "as found in the kernel stack page for the process).\n",
  "(30) kstkeip               %20s (The current EIP (instruction pointer)).\n",
  "(31) signal                %20s (The  bitmap of pending signals, displayed "
  "as a decimal number).\n",
  "(32) blocked               %20s (The bitmap of blocked signals, displayed "
  "as a decimal number).\n",
  "(33) sigignore             %20s (The  bitmap of ignored signals, displayed "
  "as a decimal number).\n",
  "(34) sigcatch              %20s (The bitmap of caught signals, displayed as "
  "a decimal number).\n",
  "(35) wchan                 %20s (This is the \"channel\" in which the "
  "process is waiting).\n",
  "(36) nswap                 %20s (Number of pages swapped (not "
  "maintained)).\n",
  "(37) cnswap                %20s (Cumulative nswap for child processes (not "
  "maintained)).\n",
  "(38) exit_signal           %20s (since Linux 2.1.22) (Signal to be sent to "
  "parent when we die).\n",
  "(39) processor             %20s (since Linux 2.2.8) (CPU number last "
  "executed on).\n",
  "(40) rt_priority           %20s (since Linux 2.5.19) (Real-time scheduling "
  "priority, a number in the range 1 to 99 for processes scheduled under a "
  "real-time policy, or 0, for non-real-time processes).\n",
  "(41) policy                %20s (since Linux 2.5.19) (Scheduling policy).\n",
  "(42) delayacct_blkio_ticks %20s (since Linux 2.6.18) (Aggregated block I/O "
  "delays, measured in clock ticks (centiseconds)).\n",
  "(43) guest_time            %20s (since Linux 2.6.24) (Guest time of the "
  "process (time spent running a virtual CPU for a guest operating system), "
  "measured in clock ticks).\n",
  "(44) cguest_time           %20s (since Linux 2.6.24) (Guest time of the "
  "process's children, measured in clock ticks).\n",
  "(45) start_data            %20s (since Linux 3.3) (Address above which "
  "program initialized and uninitialized (BSS) data are placed).\n",
  "(46) end_data              %20s (since Linux 3.3) (Address below which "
  "program initialized and uninitialized (BSS) data are placed).\n",
  "(47) start_brk             %20s (since Linux 3.3) (Address above which "
  "program heap can be expanded with brk(2)).\n",
  "(48) arg_start             %20s (since Linux 3.5) (Address above which "
  "program command-line arguments (argv) are placed).\n",
  "(49) arg_end               %20s (since Linux 3.5) (Address below program "
  "command-line arguments (argv) are placed).\n",
  "(50) env_start             %20s (since Linux 3.5) (Address above which "
  "program environment is placed).\n",
  "(51) env_end               %20s (since Linux 3.5) (Address below which "
  "program environment is placed).\n",
  "(52) exit_code             %20s (since Linux 3.5) (The thread's exit status "
  "in the form reported by waitpid(2)).\n"
};

const char* NO_DESCR[] = {
  "(1) pid                    %20s\n", "(2) comm                   %20s\n",
  "(3) state                  %20s\n", "(4) ppid                   %20s\n",
  "(5) pgrp                   %20s\n", "(6) session                %20s\n",
  "(7) tty_nr                 %20s\n", "(8) tpgid                  %20s\n",
  "(9) flags                  %20s\n", "(10) minflt                %20s\n",
  "(11) cminflt               %20s\n", "(12) majflt                %20s\n",
  "(13) cmajflt               %20s\n", "(14) utime                 %20s\n",
  "(15) stime                 %20s\n", "(16) cutime                %20s\n",
  "(17) cstime                %20s\n", "(18) priority              %20s\n",
  "(19) nice                  %20s\n", "(20) num_threads           %20s\n",
  "(21) itrealvalue           %20s\n", "(22) starttime             %20s\n",
  "(23) vsize                 %20s\n", "(24) rss                   %20s\n",
  "(25) rsslim                %20s\n", "(26) startcode             %20s\n",
  "(27) endcode               %20s\n", "(28) startstack            %20s\n",
  "(29) kstkesp               %20s\n", "(30) kstkeip               %20s\n",
  "(31) signal                %20s\n", "(32) blocked               %20s\n",
  "(33) sigignore             %20s\n", "(34) sigcatch              %20s\n",
  "(35) wchan                 %20s\n", "(36) nswap                 %20s\n",
  "(37) cnswap                %20s\n", "(38) exit_signal           %20s\n",
  "(39) processor             %20s\n", "(40) rt_priority           %20s\n",
  "(41) policy                %20s\n", "(42) delayacct_blkio_ticks %20s\n",
  "(43) guest_time            %20s\n", "(44) cguest_time           %20s\n",
  "(45) start_data            %20s\n", "(46) end_data              %20s\n",
  "(47) start_brk             %20s\n", "(48) arg_start             %20s\n",
  "(49) arg_end               %20s\n", "(50) env_start             %20s\n",
  "(51) env_end               %20s\n", "(52) exit_code             %20s\n"
};

const char* STATM_PATTERNS[] = { "size     %8s  (1)\n", "resident %8s  (2)\n",
                                 "shared   %8s  (3)\n", "text     %8s  (4)\n",
                                 "lib      %8s  (5)\n", "data     %8s  (6)\n",
                                 "dt       %8s  (7)\n" };

void
print_cmdline(const char* proc_path, FILE* out)
{
  char path[PATH_MAX];
  snprintf(path, PATH_MAX, "%s/cmdline", proc_path);
  FILE* file = fopen(path, "r");

  char buf[BUF_SIZE];
  int len = fread(buf, 1, BUF_SIZE, file);
  buf[len - 1] = 0;
  fprintf(out, "\n%s:\n%s\n", path, buf);

  fclose(file);
}

void
print_cwd(const char* proc_path, FILE* out)
{
  char pathToOpen[PATH_MAX];
  snprintf(pathToOpen, PATH_MAX, "%s/cwd", proc_path);
  fprintf(out, "\n%s:\n", pathToOpen);
  char buf[BUF_SIZE] = { '\0' };
  readlink(pathToOpen, buf, BUF_SIZE);
  fprintf(out, "%s\n", buf);
}

void
print_comm(const char* proc_path, FILE* out)
{
  char path[PATH_MAX];
  snprintf(path, PATH_MAX, "%s/comm", proc_path);
  FILE* file = fopen(path, "r");

  char buf[BUF_SIZE];
  int len = fread(buf, 1, BUF_SIZE, file);
  buf[len - 1] = 0;
  fprintf(out, "\n%s:\n%s\n", path, buf);

  fclose(file);
}

void
print_environ(const char* proc_path, FILE* out)
{
  char pathToOpen[PATH_MAX];
  snprintf(pathToOpen, PATH_MAX, "%s/environ", proc_path);
  fprintf(out, "\n%s:\n", pathToOpen);
  FILE* file = fopen(pathToOpen, "r");

  int len;
  char buf[BUF_SIZE];

  while ((len = fread(buf, 1, BUF_SIZE, file)) > 0) {
    for (int i = 0; i < len; i++)
      if (!buf[i])
        buf[i] = '\n';
    buf[len - 1] = '\n';
    fprintf(out, "%s", buf);
  }
  fclose(file);
}

void
print_exe(const char* proc_path, FILE* out)
{
  char pathToOpen[PATH_MAX];
  snprintf(pathToOpen, PATH_MAX, "%s/exe", proc_path);
  fprintf(out, "\n%s:\n", pathToOpen);

  char buf[BUF_SIZE] = { '\0' };
  readlink(pathToOpen, buf, BUF_SIZE);
  fprintf(out, "%s\n", buf);
}

void
print_fd(const char* proc_path, FILE* out)
{
  char pathToOpen[PATH_MAX];
  snprintf(pathToOpen, PATH_MAX, "%s/fd/", proc_path);
  fprintf(out, "\n%s:\n", pathToOpen);

  DIR* dir = opendir(pathToOpen);
  struct dirent* readDir;
  char string[PATH_MAX];
  char path[BUF_SIZE] = { '\0' };
  while ((readDir = readdir(dir)) != NULL) {
    if ((strcmp(readDir->d_name, ".") != 0) &&
        (strcmp(readDir->d_name, "..") != 0)) {
      sprintf(path, "%s%s", pathToOpen, readDir->d_name);
      readlink(path, string, PATH_MAX);
      fprintf(out, "%s -> %s\n", readDir->d_name, string);
    }
  }

  closedir(dir);
}

void
print_root(const char* proc_path, FILE* out)
{
  char pathToOpen[PATH_MAX] = { '\0' };
  snprintf(pathToOpen, PATH_MAX, "%s/root", proc_path);
  fprintf(out, "\n%s:\n", pathToOpen);
  char buf[BUF_SIZE] = { '\0' };
  readlink(pathToOpen, buf, BUF_SIZE);
  fprintf(out, "%s\n", buf);
}

void
print_stat(const char* proc_path, FILE* out)
{
  char pathToOpen[PATH_MAX] = { '\0' };
  snprintf(pathToOpen, PATH_MAX, "%s/stat", proc_path);
  fprintf(out, "\n%s:\n", pathToOpen);

  char buf[BUF_SIZE];
  FILE* file = fopen(pathToOpen, "r");
  fread(buf, 1, BUF_SIZE, file);

  char* token = strtok(buf, " ");
  for (int i = 0; token != NULL; i++) {
    fprintf(out, WITH_DESCR[i], token);
    token = strtok(NULL, " \n");
  }

  fclose(file);
}

void
print_statm(const char* proc_path, FILE* out)
{
  char pathToOpen[PATH_MAX];
  snprintf(pathToOpen, PATH_MAX, "%s/statm", proc_path);
  fprintf(out, "\n%s:\n", pathToOpen);

  FILE* file = fopen(pathToOpen, "r");
  char buf[BUF_SIZE];
  fread(buf, 1, BUF_SIZE, file);

  char* token = strtok(buf, " ");
  for (int i = 0; token != NULL; i++) {
    fprintf(out, STATM_PATTERNS[i], token);
    token = strtok(NULL, " \n");
  }
  fclose(file);
}

void
print_maps(const char* proc_path, FILE* out)
{
  char pathToOpen[PATH_MAX];
  snprintf(pathToOpen, PATH_MAX, "%s/maps", proc_path);
  fprintf(out, "\n%s:\n", pathToOpen);

  char buf[BUF_SIZE] = { '\0' };
  FILE* file = fopen(pathToOpen, "r");
  int lengthOfRead;
  while ((lengthOfRead = fread(buf, 1, BUF_SIZE, file))) {
    buf[lengthOfRead] = '\0';
    fprintf(out, "%s\n", buf);
  }
  fclose(file);
}

void
print_io(const char* proc_path, FILE* out)
{
  char pathToOpen[PATH_MAX];
  snprintf(pathToOpen, PATH_MAX, "%s/io", proc_path);
  fprintf(out, "\n%s:\n", pathToOpen);

  char buf[BUF_SIZE] = { '\0' };
  FILE* file = fopen(pathToOpen, "r");
  int lengthOfRead;
  while ((lengthOfRead = fread(buf, 1, BUF_SIZE, file))) {
    buf[lengthOfRead] = '\0';
    fprintf(out, "%s\n", buf);
  }
  fclose(file);
}

char*
get_proc_path(int argc, char* argv[])
{
  static char pathToOpen[PATH_MAX];
  if (argc < 2) {
    snprintf(pathToOpen, PATH_MAX, "/proc/self");
  } else {
    int pid = atoi(argv[1]);
    snprintf(pathToOpen, PATH_MAX, "/proc/%d", pid);
  }

  printf("%s\n", pathToOpen);
  if (access(pathToOpen, F_OK)) {
    printf("Директория для введенного pid не доступна!");
    exit(ACCESS_ERROR);
  }
  return pathToOpen;
}

void print_page(uint64_t address, uint64_t data, FILE *fres)
{
    fprintf(fres, "%#08lx <- ", address);

    if ((data >> 62) & 1)
        fprintf(fres, "[%#1lx : %#08lx]", data & 0xf, (data >> 4) & 0x7ffffffffffff);
    else
        fprintf(fres, "%#08lx ", data & 0x7fffffffffffff);

    // ex - exclusively mapped; sd - soft-dirty PTE; fs - file/shared; s -swappped; p - present
    fprintf(fres, "sd %ld; em %ld; fs %ld; s %ld; p %ld\n",
        (data >> 55) & 1,
        (data >> 56) & 1,
        (data >> 61) & 1,
        (data >> 62) & 1,
        (data >> 63) & 1);
}

int print_pagemap(pid_t pid, FILE *fres, int print_not_present)
{
    const char *filename = "pagemap";
    fprintf(fres, "======================= %s ======================\n", filename);

    char path_buf[BUF_SIZE];
    snprintf(path_buf, sizeof path_buf, "/proc/%d/%s", pid, filename);
    int fd = open(path_buf, O_RDONLY);
    if (fd == -1)
	{
        return -1;
	}

    uint64_t start_address = 0x00000000;
    uint64_t end_address = 0xc0000000;
    for(uint64_t i = start_address; i < end_address; i += PAGE_SIZE)
    {
        uint64_t data;
        uint64_t index = (i / PAGE_SIZE) * sizeof(data);
        if(pread(fd, &data, sizeof(data), index) != sizeof(data))
        {
            perror("pread");
            break;
        }

        if (print_not_present || ((data >> 63) & 1))
            print_page(i, data, fres);
    }
    close(fd);
    return 0;
}

int print_directory_info(const char *filename, pid_t pid, FILE *fres)
{
    fprintf(fres, "======================== %s ========================\n", filename);

    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    char path_buf[BUF_SIZE];
    snprintf(path_buf, sizeof path_buf, "/proc/%d/%s", pid, filename);
    DIR *dp = opendir(path_buf);
    if (dp == NULL)
        return -1;

    char dirp_buf[BUF_SIZE];
    struct dirent *dirp;
    while ((dirp = readdir(dp)) != NULL)
    {
        if ((strcmp(dirp->d_name, ".") != 0) &&
            (strcmp(dirp->d_name, "..") != 0)) 
        {
            sprintf(dirp_buf, "%s/%s", path_buf, dirp->d_name);

            readlink(dirp_buf, buf, BUF_SIZE);
            fprintf(fres, "%s\t%s\n", dirp->d_name, buf);
        }
    }
    fprintf(fres, "\n\n");
    closedir(dp);
    return 0;
}

int
main(int argc, char* argv[])
{
  char* proc_path = get_proc_path(argc, argv);

  FILE* out = fopen("result4.txt", "a");

  print_cmdline(proc_path, out);
  print_cwd(proc_path, out);
  print_comm(proc_path, out);
  print_environ(proc_path, out);
  print_exe(proc_path, out);
  print_fd(proc_path, out);
  print_root(proc_path, out);
  print_stat(proc_path, out);
  print_statm(proc_path, out);
  print_io(proc_path, out);
  print_maps(proc_path, out);
  
  char *end = NULL;
  pid_t pid = strtoul(argv[1], &end, 10);
  print_directory_info("task", pid, out);
  
  FILE* out2 = fopen("pagemap4.txt", "a");
  
  print_pagemap(pid, out2, 1);

  fclose(out);
  fclose(out2);

  return 0;
}
