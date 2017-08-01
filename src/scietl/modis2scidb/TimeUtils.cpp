#include "TimeUtils.hpp"
#include <stdlib.h>
#include <stdint.h>

#ifdef _WINDOWS
       #include <windows.h>
       #include <winsock2.h>
#else
    #include <sys/time.h>
    #include <unistd.h>
#endif
int tu::usleep(long usec){
     #ifdef _WINDOWS
            struct timeval tv;
            fd_set dummy;
            SOCKET s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
            FD_ZERO(&dummy);
            FD_SET(s, &dummy);
            tv.tv_sec = usec/1000000L;
            tv.tv_usec = usec%1000000L;
            return select(0, 0, 0, &dummy, &tv);
     #else
          return usleep(usec);
     #endif
}

void tu::sleep(int value) {
      #ifdef _WINDOWS
          Sleep(value*1000);
     #else
          sleep(value);
     #endif
}

#ifdef _WINDOWS
       struct timezone {};
       int gettimeofday(struct timeval *tp, struct timezone *tzp) {
           static const uint64_t EPOCH = ((uint64_t) 116444736000000000ULL);

           SYSTEMTIME system_time;
           FILETIME file_time;
           uint64_t time;
           GetSystemTime(&system_time);
           SystemTimeToFileTime(&system_time, &file_time);

           time = ((uint64_t)file_time.dwLowDateTime);
           time+= ((uint64_t)file_time.dwHighDateTime)<<32;

           tp->tv_sec = (long) ((time - EPOCH)/10000000L);
           tp->tv_usec = (long) (system_time.wMilliseconds*1000);
           return 0;
       }
#endif

uint64_t tu::micros_since_epoch(){
    struct timeval tv;
    uint64_t micros = 0;
    gettimeofday(&tv, NULL);
    micros =  ((uint64_t)tv.tv_sec) * 1000000 + tv.tv_usec;
    return micros;
}
