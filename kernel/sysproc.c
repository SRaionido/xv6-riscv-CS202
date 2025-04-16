#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "pinfo.h" // New header

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64 sys_sysinfo(void)
{
  int n;
  argint(0, &n);
  int out = sys_info(n);
  return out;
}

//sys_procinfo Kernel Function
uint64
sys_procinfo(void)
{
  struct proc *p = myproc();
  uint64 pinfo_user_addr; // Address of user struct pinfo
  struct pinfo kinfo;     // Kernel-space copy of the struct

  // Get the pointer argument from user space (argument 0)
  // argaddr fetches a pointer-sized argument.
  if (argaddr(0, &pinfo_user_addr) < 0) {
    return -1; // Error fetching argument
  }

  // Basic validation: check for null pointer passed by user
  if (pinfo_user_addr == 0) {
      return -1;
  }

  // Fill the kernel-space struct kinfo
  // Parent PID: Handle initproc case where p->parent might be 0
  if (p->parent) {
      kinfo.ppid = p->parent->pid;
  } else {
      // Typically only initproc has no parent pointer set after setup.
      // We can return its own PID or a special value like 0 or 1.
      // Returning its own PID is a reasonable convention.
      kinfo.ppid = p->pid;
  }

  // Syscall count: read the current value (before it's incremented for this call)
  kinfo.syscall_count = p->syscall_count;

  // Page usage: Calculate pages from process size (sz)
  // PGSIZE is defined in memlayout.h
  // Use ceiling division: (numerator + denominator - 1) / denominator
  kinfo.page_usage = (p->sz + PGSIZE - 1) / PGSIZE;

  // Copy the filled kernel struct to the user-space address.
  // copyout(pagetable, user_dest_addr, kernel_src_addr, size)
  if (copyout(p->pagetable, pinfo_user_addr, (char *)&kinfo, sizeof(kinfo)) < 0) {
    return -1; // Error copying data to user space
  }

  return 0; // Success
}
