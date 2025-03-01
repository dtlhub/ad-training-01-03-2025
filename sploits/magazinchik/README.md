- Buffer overflow в структуре User, точнее в функции addUser

```angular2html
User newUser;
strcpy(newUser.username, username);
strcpy(newUser.password, password);
```

можно вписать какую угодно длину пароля или username-a, что делает возможным перезапись переменной balance 

```angular2html
struct User{
    char username[32];
    char password[32];
    int balance = 100;
};
```

на какое-то др число 

от bof спасет либо + '\0' в конец , либо перепись на python 
по другому не спасутся


- ошибка логики в поле "Забыл пароль"

Есть возможность перезаписать пароль от любого пользователя

Варианты фикса
поменять директорию с /forgot на /aaa_fig_tebe не заработает 
