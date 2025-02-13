to compile use:
```
gcc -fPIC -c vtable_lib.c -o vtable_lib.o
gcc -shared -o libvtable.so vtable_lib.o
gcc main.c -L. -lvtable -lpthread -lcrypto -Wl,-rpath=. -o chall
```
