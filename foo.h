typedef int integer;

extern integer square(short i);

@interface Foo
@property int bar;
@end

#define CF_BRIDGED_TYPE(T)		__attribute__((objc_bridge(T)))

typedef struct __attribute__((packed)) CF_BRIDGED_TYPE(id) __SecKey *SecKeyRef;

typedef struct __SecKey OpaqueSecKeyRef;

extern void i_take_id(id foo);

extern int i_take_key(SecKeyRef muh_key);
