1. libvtable_exp

эта уязвимость строится на этом куске из updateCurrentGladiator() -->
```
    printf("Enter new value: ");    
    char *base = (char *)&current;
    char *dest = base + 40 * (idx - 1);
    
    int bytes_read = read(0, dest, 40);
    if (bytes_read > 0) {
        for (int i = 0; i < bytes_read; i++) {
            if (dest[i] == '\n') {
                dest[i] = '\0';
                break;
            }
        }
    }
```
здесь у нас сразу 2 уязы --> 
1) ошибка null-терминирования
2) outbound write

с помощью них мы можем leak'нуть сначала PIE потом адрес из libvtable.so(так же найти "one gadget" в этой библиотеке) и переписать адрес который хранится в vtable на one_gadget и вызвать его...

2. format_exp

вот этот кусок кода из fight() очень плохой... -->
```
    char enemcomm[40];
    char mycomm[40];
    strncpy(enemcomm, enemy.comment, sizeof(enemcomm));
    strncpy(mycomm, current.comment, sizeof(mycomm));

    int res = get_random_range(MAX_RANDOM_VALUE);
    if (res != 0) {
        printf("Winner -> %s\n", enemy.name);
        printf("You lost!\n");
        printf(mycomm);
    } else {
        printf("Winner -> %s\n", current.name);
        printf("You won!\n");
        printf(enemcomm);
    }
```
мы сначала кладем на стек флаг нашего противника, а потом у нас еще и форматка появляется
значит мы можем просто вывести этот флаг подобрав правильный offset