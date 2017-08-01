#ifndef __TIME_UTILS_H__
#define __TIME_UTILS_H__

#include <stdint.h>

namespace tu{
uint64_t micros_since_epoch();
int usleep(long value);
void sleep(int value);
}

#endif /* __TIME_UTILS_H__ */
