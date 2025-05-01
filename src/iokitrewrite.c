#include <stdio.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/serial/IOSerialKeys.h>

// Define the interposition structure for macOS
#define DYLD_INTERPOSE(_replacement, _replacee)            \
    __attribute__((used)) static struct                    \
    {                                                      \
        const void *replacement;                           \
        const void *replacee;                              \
    } _interpose_##_replacee                               \
        __attribute__((section("__DATA,__interpose"))) = { \
            (const void *)(unsigned long)&_replacement,    \
            (const void *)(unsigned long)&_replacee};

// Intercepted IOObjectGetClass function
kern_return_t my_IOObjectGetClass(io_object_t object, io_name_t className)
{
    kern_return_t result = IOObjectGetClass(object, className);
    fprintf(stderr, "[INTERCEPT] IOObjectGetClass called, class: %s\n", className);
    if (result == KERN_SUCCESS)
    {
        if (strcmp(className, "IOUSBHostDevice") == 0)
        {
            fprintf(stderr, "[INTERCEPT] Modifying class name from %s to IOUSBDevice\n", className);
            snprintf(className, sizeof(io_name_t), "IOUSBDevice");
        }
    }
    return result;
}

// Set up the interposing
DYLD_INTERPOSE(my_IOObjectGetClass, IOObjectGetClass)
