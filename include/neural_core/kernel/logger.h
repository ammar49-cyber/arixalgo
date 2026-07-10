#ifndef SNEPPX_LOGGER_H
#define SNEPPX_LOGGER_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Severity levels */
typedef enum {
    SNEPPX_LOG_TRACE = 0,
    SNEPPX_LOG_DEBUG = 1,
    SNEPPX_LOG_INFO  = 2,
    SNEPPX_LOG_WARN  = 3,
    SNEPPX_LOG_ERROR = 4,
    SNEPPX_LOG_FATAL = 5
} SNEPPX_LogLevel;

/* Structured log entry */
typedef struct {
    int64_t  timestamp_ns;
    SNEPPX_LogLevel level;
    int      rank;
    char     file[64];
    int      line;
    char     message[1024];
} SNEPPX_LogEntry;

/* Logger configuration */
typedef struct {
    SNEPPX_LogLevel min_level;
    int             enable_stdout;
    int             enable_json;
    const char*     json_path;
    int             rank;
    int             enable_timestamp;
    int             enable_color;
} SNEPPX_LogConfig;

/* Global logger handle (opaque) */
typedef struct SNEPPX_Logger SNEPPX_Logger;

/* Lifecycle */
int  SNEPPX_logger_init(SNEPPX_Logger** logger, const SNEPPX_LogConfig* config);
void SNEPPX_logger_destroy(SNEPPX_Logger* logger);
int  SNEPPX_logger_set_level(SNEPPX_Logger* logger, SNEPPX_LogLevel level);
int  SNEPPX_logger_set_output(SNEPPX_Logger* logger, const char* json_path);

/* Core log function */
void SNEPPX_log_write(SNEPPX_Logger* logger, SNEPPX_LogLevel level,
                       const char* file, int line,
                       const char* fmt, ...);

/* Convenience macros */
#define SNEPPX_LOG_TRACE(logger, ...)  SNEPPX_log_write(logger, SNEPPX_LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define SNEPPX_LOG_DEBUG(logger, ...)  SNEPPX_log_write(logger, SNEPPX_LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define SNEPPX_LOG_INFO(logger, ...)   SNEPPX_log_write(logger, SNEPPX_LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define SNEPPX_LOG_WARN(logger, ...)   SNEPPX_log_write(logger, SNEPPX_LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define SNEPPX_LOG_ERROR(logger, ...)  SNEPPX_log_write(logger, SNEPPX_LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define SNEPPX_LOG_FATAL(logger, ...)  SNEPPX_log_write(logger, SNEPPX_LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

/* Default log level strings */
const char* SNEPPX_log_level_string(SNEPPX_LogLevel level);

#ifdef __cplusplus
}
#endif

#endif /* SNEPPX_LOGGER_H */
