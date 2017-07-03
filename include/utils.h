#ifndef __UTILS_H_
#define __UTILS_H_

char *strjoin(const char *str1, const char *str2);

char *convert_int(int i);

void write_log(int debug_level, const char *str, ...);

#endif // __UTILS_H_