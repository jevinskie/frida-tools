typedef int integer;

extern integer square(short i);

@interface Foo
@property int bar;
@end

typedef int __attribute__((vector_size(16))) int_v4;

#define CF_BRIDGED_TYPE(T) __attribute__((objc_bridge(T)))

typedef struct __attribute__((packed)) CF_BRIDGED_TYPE(id) __SecKey *SecKeyRef;

typedef struct __SecKey OpaqueSecKeyRef;

typedef struct __attribute__((packed)) CF_BRIDGED_TYPE(Foo) __Foo *FooRef;

typedef struct __Foo OpaqueFooRef;

extern void i_take_id(id muh_id);

extern void i_take_id_ptr(id *muh_id_ptr);

extern int i_take_keyref(SecKeyRef muh_key_ref);

extern int i_take_fooref(FooRef muh_foo_ref);

extern int i_take_opaque_foo(OpaqueFooRef muh_opaque_foo_ref);

extern int i_take_int_v4(int_v4 muh_iv4);

extern int i_take_foo_ptr(Foo *muh_foo_ptr);
