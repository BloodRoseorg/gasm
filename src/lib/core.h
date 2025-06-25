#define u8  unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned long long
#define i8  signed char
#define i16 signed short
#define i32 signed int
#define i64 signed long long
#define f32 float
#define f64 double
#define true 1
#define false 0
#define NULL 0

typedef u64 uint64_t;
typedef u32 uint32_t;
typedef u16 uint16_t;
typedef u64 uintptr_t;
typedef u32 size_t;
typedef u32 bool;

#define SYSCALL_READ    0
#define SYSCALL_WRITE   1
#define SYSCALL_OPEN    2
#define SYSCALL_CLOSE   3
#define SYSCALL_FSTAT   5
#define SYSCALL_EXECVE  59
#define SYSCALL_EXIT    60

#define STDIN           0
#define STDOUT          1
#define STDERR          2

extern void *   brk                     (void *address);
extern bool     memcheck                (void *ptr, size_t size);
extern i32      __syscall_read          (i32 fd, u8 *buf, i32 count);
extern i32      __syscall_write         (i32 fd, u8 *buf, i32 count);
extern i32      __syscall_open          (u8 *filename, i32 flags, i32 mode);
extern i32      __syscall_close         (i32 fd);
extern void *   __syscall_mmap          (u8 *addr, u32 length, u32 prot, u32 flags, i32 fd, u32 offset);
extern void *   __syscall_munmap        (u8 *addr, u32 length);
extern void     __syscall_execve        (u8 *filename, u8 **argv, u8 **envp);
extern void     __syscall_exit          (i32 code);
extern i32      __syscall_lseek         (i32 fd, i64 offset, u32 whence);

#define read    __syscall_read
#define write   __syscall_write
#define open    __syscall_open
#define close   __syscall_close
#define mmap    __syscall_mmap
#define munmap  __syscall_munmap
#define execve  __syscall_execve
#define exit    __syscall_exit
#define lseek   __syscall_lseek
