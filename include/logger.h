#pragma once

enum LogLevel {
    INFO,
    WARNING,
    ERROR,
    DEBUG
};

void klog(LogLevel level, const char* message);