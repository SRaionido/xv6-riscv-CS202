#ifndef _PINFO_H_
#define _PINFO_H_

struct pinfo {
  int ppid;           // PID of parent process
  int syscall_count;  // Total number of syscalls made by the process
  int page_usage;     // Memory usage in pages
};

#endif
