#define printf(fmt, ...) __printf(fmt, (void *[]){__VA_ARGS__, NULL})
extern void     strcpy(char *dest, char *src, i32 max);
extern void     strcat(char *dest, char *src, i32 max);
extern void     print(char *);
extern void     print_i32(i32 number);
extern void     print_u32(u32 number);
extern void     print_float(float number);
extern void     print_h32(u32 number);
extern void     __printf(const char *fmt, void *args[]);

#define strfmt(buffer, max, fmt, ...) __strfmt(buffer, max, fmt, (void *[]){__VA_ARGS__, NULL})
extern char *   __strfmt(char *buffer, u32 max, char *fmt, void *args[]);
extern i32      strlen (char *string, i32 max);
extern void     strcpy (char *dest, char *src, i32 max);
extern void     strcat (char *dest, char *src, i32 max);
extern i32      strcmp (char *dest, char *src, i32 max);
extern void     memcpy (char *dest, char *src, i32 max);
extern bool     memcmp (char *dest, char *src, i32 max);

extern i64      file_size(char *filename);
extern i64      file_read(char *filename, char *buffer, i64 max);
extern i64      file_write(char *filename, char *content, i64 size);