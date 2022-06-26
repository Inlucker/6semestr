#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>

#include "defines.h"

#define COMMON_INFO_FILENAME "common_info.txt"
#define PROC_INFO_FILENAME "proc_info.txt" 
#define BUF_SIZE 4096

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
        return -1;

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

int print_link(const char *filename, pid_t pid, FILE *f_res)
{
    fprintf(f_res, "======================= %s ========================\n", filename);

    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    char path_buf[BUF_SIZE];
    snprintf(path_buf, sizeof path_buf, "/proc/%d/%s", pid, filename);
    readlink(path_buf, buf, BUF_SIZE);
    fprintf(f_res, "%s\n\n", buf);
    return 0;
}

void replace_char(char *bytes, size_t len)
{
    for (int i = 0; i < len; i++)
    {
        if (bytes[i] == 0)
            bytes[i] = '\n';
    }
    bytes[len - 1] = 0;
}

int print_file(const char *filename, pid_t pid, FILE *fres, void (*strfn)(char *bytes, size_t len))
{
    fprintf(fres, "======================= %s ======================\n", filename);

    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    char path_buf[BUF_SIZE];
    snprintf(path_buf, sizeof path_buf, "/proc/%d/%s", pid, filename);
    int len;
    FILE *f = fopen(path_buf, "r");
    if (f == NULL)
        return -1;

    if (strfn == NULL)
    {
        while ((len = fread(buf, 1, BUF_SIZE, f)) > 0)
            fprintf(fres, "%s", buf);
    }
    else
    {
        while ((len = fread(buf, 1, BUF_SIZE, f)) > 0)
        {
            strfn(buf, len);
            fprintf(fres, "%s", buf);
        }
    }
    fprintf(fres, "\n\n");
    fclose(f);
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

int print_map_file(const char *filename, pid_t pid, FILE *fres, const char *map[])
{
    fprintf(fres, "======================= %s ======================\n", filename);

    char path_buf[BUF_SIZE];
    snprintf(path_buf, sizeof path_buf, "/proc/%d/%s", pid, filename);
    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    int i = 0;
    FILE *f = fopen(path_buf, "r");
    if (f == NULL)
        return -1;

    fread(buf, 1, BUF_SIZE, f);
    char *temp = strtok(buf, " ");
    while (temp != NULL)
    {
        fprintf(fres, "%s:    %s\n", map[i], temp);
        i++;
        temp = strtok(NULL, " ");
    }
    fprintf(fres, "\n");
    fclose(f);
    return 0;
}

int print_common_file(const char *filename, FILE *fres, const char *map[])
{
    fprintf(fres, "======================= %s ======================\n", filename);

    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    int len;
    FILE *f = fopen(filename, "r");
    if (f == NULL)
        return -1;

    while ((len = fread(buf, 1, BUF_SIZE, f)) > 0)
        fprintf(fres, "%s", buf);

    fprintf(fres, "\n\n");
    fclose(f);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Использование: app.exe <pid>\n");
        return 1;
    }

    char *end = NULL;
    pid_t pid = strtoul(argv[1], &end, 10);
    if (end == argv[1])
    {
        printf("Ожидалось целое цисло\n");
        return 1;
    }

    FILE *f_res = fopen(PROC_INFO_FILENAME, "w");
    if (f_res == NULL)
    {
        perror(argv[0]);
        return 1;
    }
    
    // add meminfo, modules

    print_link("root", pid, f_res);
    print_link("exe", pid, f_res);
    print_link("cwd", pid, f_res);

    print_file("cmdline", pid, f_res, NULL);
    print_file("environ", pid, f_res, replace_char);
    print_file("maps", pid, f_res, NULL);
    print_file("io", pid, f_res, NULL);
    print_file("status", pid, f_res, NULL);
    print_file("comm", pid, f_res, NULL);

    print_map_file("stat", pid, f_res, (const char**)stat_info);
    print_map_file("statm", pid, f_res, (const char**)statm_info);

    print_directory_info("fd", pid, f_res);
    print_directory_info("task", pid, f_res);

    print_pagemap(pid, f_res, 0);

    //print_directory_info("map_files", pid, f_res);    // Дублирует maps
    //print_file("mountinfo", pid, f_res, NULL);        // Еще рано
    //print_file("mounts", pid, f_res, NULL);           // 
    //print_file("mountstats", pid, f_res, NULL);       // 
    //print_file("smaps", pid, f_res, NULL);            // Слишком большой вывод
    //print_file("limits", pid, f_res, NULL);           // Исследуемый процесс не изменяет лимиты ресурсов

    fclose(f_res);

    f_res = fopen("system_info.txt", "w");
    if (f_res == NULL)
    {
        perror(argv[0]);
        return 1;
    }

    print_common_file("/proc/cpuinfo", f_res, NULL);
    print_common_file("/proc/loadavg", f_res, NULL);
    print_common_file("/proc/meminfo", f_res, NULL);
    print_common_file("/proc/modules", f_res, NULL);
    print_common_file("/proc/mounts", f_res, NULL);
    print_common_file("/proc/sys/vm/user_reserve_kbytes", f_res, NULL);
    print_common_file("/proc/stat", f_res, NULL);
    print_common_file("/proc/sys/kernel/sem", f_res, NULL);

    fclose(f_res);

    return 0;
}
