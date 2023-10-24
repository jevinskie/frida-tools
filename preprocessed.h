# 1 "./objc-bridge-attr-test.h"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 410 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "./objc-bridge-attr-test.h" 2


@class NSString;



typedef struct __attribute__((packed)) __attribute__((objc_bridge(NSString))) __SecKeyz *SecKeyRefz;

extern int i_take_a_key(SecKeyRefz my_key);
