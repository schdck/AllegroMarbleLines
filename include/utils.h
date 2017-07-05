#ifndef __UTILS_H_
#define __UTILS_H_

char *strjoin(const char *str1, const char *str2);

char *convert_int(int i);

double distance_between_points(double x1, double y1, double x2, double y2);

void write_log(int debug_level, bool print_prefix, const char *str, ...);

#endif // __UTILS_H_