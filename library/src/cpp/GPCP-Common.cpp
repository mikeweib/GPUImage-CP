#include "GPCP-Common.h"

static const int MAX_LOG_LENGTH = 16 * 1024;

void _log(const char *format, va_list args) {
    int bufferSize = MAX_LOG_LENGTH;
    char *buf = nullptr;
    int nret = 0;
    do {
        buf = new(std::nothrow) char[bufferSize];
        if (buf == nullptr) return;

        nret = vsnprintf(buf, bufferSize - 3, format, args);
        if (nret >= 0) {
            if (nret <= bufferSize - 3) { // success, so don't need to realloc
                break;
            } else {
                bufferSize = nret + 3;
                delete[] buf;
            }
        } else {
            bufferSize *= 2;
            delete[] buf;
        }
    } while (true);
    buf[nret] = '\n';
    buf[++nret] = '\0';

    // Linux, Mac, iOS, etc
    fprintf(stdout, "GPCP_DEBUG : %s", buf);
    fflush(stdout);
    delete[] buf;
}

void log(const char *format, ...) {
    va_list args;
    va_start(args, format);
#ifdef __ANDROID__
    __android_log_vprint(ANDROID_LOG_DEBUG, "GPCP_DEBUG", format, args);
#else
    _log(format, args);
#endif
    va_end(args);
}