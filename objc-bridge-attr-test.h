// #import <Foundation/Foundation.h>

@class NSString;

#define CF_BRIDGED_TYPE(T) __attribute__((objc_bridge(T)))

typedef struct __attribute__((packed)) CF_BRIDGED_TYPE(NSString) __SecKeyz *SecKeyRefz;

extern int i_take_a_key(SecKeyRefz my_key);

void func(unsigned short n) asm ("MYFUNC");

static int x (void) __attribute__ ((alias ("y")));