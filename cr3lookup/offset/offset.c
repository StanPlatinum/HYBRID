#include "linux/init.h"
#include "linux/module.h"
#include "linux/stddef.h"
#include <linux/rtc.h>

MODULE_AUTHOR("viktor");
MODULE_LICENSE("Dual BSD/GPL");
#define MODULE_NAME     "offset_finder"

#define size_of(_struct) printk("sizeof(" #_struct ") : %lu\n", sizeof(struct _struct))
#define offset_of(_struct, _member) printk(#_struct "->" #_member " : 0x%lx\n", offsetof(struct _struct, _member))

#include "linux/list.h"
#include "linux/sched.h"        /* task_struct */
#include "linux/cred.h"         /* cred */
#include "linux/mm.h"		/* mm_struct */
#include "linux/fdtable.h"  /* files_struct, fdtable*/
#include "linux/file.h"         /* files_struct */
#include "linux/fs.h"           /* file, inode, file_operations*/
#include "linux/path.h"         /*path*/
#include "linux/dcache.h"       /* dentry, qstr */
#include "linux/net.h"		/* socket */
#include "net/sock.h"		/* sock */
#include "net/inet_sock.h"	/* inet_sock */
//#include "asm/bitops.h"         /*test_bit*/
#include "linux/lockref.h" 
#include "linux/time.h"

#define _test_bit(nr, addr)                      \
         (__builtin_constant_p((nr))             \
          ? _constant_test_bit((nr), (addr))      \
          : _variable_test_bit((nr), (addr)))

static __always_inline int _constant_test_bit(long nr, const volatile unsigned long *addr)
{
         return ((1UL << (nr & (BITS_PER_LONG-1))) &
                 (addr[nr >> _BITOPS_LONG_SHIFT])) != 0;
}

static inline int _variable_test_bit(long nr, volatile const unsigned long *addr)
{
        int oldbit;

        asm volatile("bt %2,%1\n\t"
                     "sbb %0,%0"
                     : "=r" (oldbit)
                     : "m" (*(unsigned long *)addr), "Ir" (nr));

        return oldbit;
}

static void print(void)
{
    size_of(list_head);
    size_of(task_struct);
    size_of(mm_struct);
    size_of(files_struct);
    size_of(fdtable);
    size_of(file);
    size_of(path);
    size_of(dentry);
    size_of(qstr);
    size_of(inode);
    size_of(timespec);
    printk("sizeof(fd_set) : %lu\n", sizeof(fd_set));
    size_of(socket);
    size_of(sock);
    size_of(inet_sock);

    offset_of(list_head, next);
    offset_of(list_head, prev);

    offset_of(task_struct, tasks);
    offset_of(task_struct, pid);
    offset_of(task_struct, real_parent);
    offset_of(task_struct, parent);
    offset_of(task_struct, real_start_time);
/*    offset_of(task_struct, uid);  */
    offset_of(task_struct, real_cred);  /* 取进程客体的credential */
    offset_of(cred, uid);               /* 取实际用户的uid */
    offset_of(task_struct, comm);
    offset_of(task_struct, files);
    offset_of(task_struct, mm);
    offset_of(mm_struct, arg_start);
    offset_of(mm_struct, arg_end);
    offset_of(mm_struct, total_vm);
/*    offset_of(mm_struct, reserved_vm); */

    offset_of(files_struct, count);
    offset_of(files_struct, fdt);
    offset_of(files_struct, fdtab);
    offset_of(files_struct, fd_array);
    offset_of(fdtable, max_fds);
    offset_of(fdtable, fd);
    offset_of(fdtable, open_fds);

/*    offset_of(file, f_dentry);*/
    offset_of(file, f_path);
    offset_of(file, f_inode);
    offset_of(file, f_op);
    offset_of(file, f_flags);
    offset_of(file, f_version);
    offset_of(file, private_data);
    
    offset_of(path, mnt);
    offset_of(path, dentry);
    
    offset_of(dentry, d_inode);
    offset_of(dentry, d_name);
    offset_of(dentry, d_iname);
    offset_of(dentry, d_time);
    offset_of(dentry, d_lockref);
    offset_of(qstr, hash_len);
    offset_of(qstr, name);

    offset_of(lockref, count);
    
    offset_of(inode, i_ino);
    offset_of(inode, i_mode);
    offset_of(inode, i_atime);
    offset_of(inode, i_mtime);
    offset_of(inode, i_ctime);
    
    offset_of(timespec, tv_sec);
    offset_of(timespec, tv_nsec);
    
    offset_of(file_operations, read);
    offset_of(file_operations, write);
    offset_of(file_operations, open);
    offset_of(file_operations, release);
    //offset_of(file_operations, i_ctime);

    offset_of(socket, sk);
    offset_of(sock, sk_prot);
    offset_of(sock, sk_family);
    offset_of(sock, sk_stamp);
    offset_of(sock, sk_flags);
    offset_of(sock, sk_tsflags);
    offset_of(inet_sock, inet_daddr);
    offset_of(inet_sock, inet_rcv_saddr);
    offset_of(inet_sock, inet_dport);
    offset_of(inet_sock, inet_saddr);
    offset_of(inet_sock, inet_sport);
    
    offset_of(proto, name);
}

static void print_process(void)
{
    struct task_struct *proc;
    for_each_process(proc) {
        if (!(proc->files))
        {
              printk("no files!\n");  
        }
        else
        {
                printk("[%d] %s, [%d] %s, [%llu] %llu\n", proc->pid, proc->comm, proc->real_parent->pid, proc->real_parent->comm, proc->start_time, proc->real_start_time);
                struct fdtable *fdt = proc->files->fdt;
                if (!fdt)
                {
                        printk("no fdtable!\n"); 
                }
                else
                {
                
                        int i, count = fdt->max_fds;
                        int size = 8 * sizeof(unsigned long);
                        printk("fds = %d\n", count);
                        //printk("open_fds = %d\n", fdt->open_fds);
                        //printk("fdarray = %d\n", fdt->fd);
                        for (i=0; i<count; i++)
                        {
                                //printk("result %d = %d\n", i, fdt->open_fds[i]);
                                if (_test_bit(i, fdt->open_fds))
                                {
                                        printk("[%d] f_inode = %d\n", i, fdt->fd[i]->f_inode);
                                        struct rtc_time tm;
                                        rtc_time_to_tm(fdt->fd[i]->f_inode->i_atime.tv_sec, &tm);
                                        printk("atime_UTC time :%d-%d-%d %d:%d:%d \n",tm.tm_year+1900,tm.tm_mon+1, tm.tm_mday,tm.tm_hour+8,tm.tm_min,tm.tm_sec);
                                        //printk("atime_sec = %ld, atime_tv_nsec = %ld\n", fdt->fd[i]->f_inode->i_atime.tv_sec, fdt->fd[i]->f_inode->i_atime.tv_nsec);
                                        //printk("f_flags = %d\n", fdt->fd[i]->f_flags);
                                        //printk("f_version = %lu\n", fdt->fd[i]->f_version);
                                        printk("%d, %d, %d, %d\n", fdt->fd[i]->f_op->read, fdt->fd[i]->f_op->write, fdt->fd[i]->f_op->open, fdt->fd[i]->f_op->release);
                                        struct dentry *dentry = fdt->fd[i]->f_path.dentry;
                                        //printk("dentry = %d\n", dentry);
				        if (dentry)
				        {
				                //printk("d_inode = %d\n", dentry->d_inode);
						printk("id = %d; mode = %d\n", dentry->d_inode->i_ino, dentry->d_inode->i_mode);
						//printk("d_flags = %d\n", dentry->d_flags);
        	                                printk("[%d] small names = %s\n", i, dentry->d_iname);
        	                                unsigned char *name = dentry->d_name.name;
        	                                printk("d_name = [%d] %s\n", dentry->d_name.hash_len, name);
        	                                printk("d_time = %lu\n", dentry->d_time);
						printk("dentry_count = %d\n", dentry->d_lockref.count);
				        }
				        struct file_operations *f_op = fdt->fd[i]->f_op;
				        long int *readresult = f_op->read;
				        long int *writeresult = f_op->write;
				        printk("f_read: %ld || f_write: %ld\n", readresult, writeresult);
				        
	                       }
                        }
                        
                
                }
    }
    }
}

static int mod_init(void)
{
    printk(MODULE_NAME " loaded.\n");
    print();
    //print_process();

    return 0;
}

static void mod_exit(void)
{
    printk(MODULE_NAME " unloaded.\n");
}

module_init(mod_init);
module_exit(mod_exit);

