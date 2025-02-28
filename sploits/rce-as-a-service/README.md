## Rce as a Service

Уязвимость в этом сервисе заключается в том, что wasmtime поддерживает апи wasm preview 2, в котором есть возможность работать с сетью прямо из песочницы wasm-а:

```rust
let wasi = WasiCtxBuilder::new()
    .stdin(pipe::ClosedInputStream {})
    .stdout(stdout.clone())
    .stderr(stderr.clone())
    .args(real_args.leak())
    .inherit_env()
    .inherit_network()  // <--- This permits to open network connections
    .preopened_dir(host_mount, "/", DirPerms::all(), FilePerms::all())?
    .build();
```

Таким образом, можно собрать модуль, который вычитывает пароль из редиса, после чего залогиниться за юзера и прочитать флаги из его файлов. Username для эксплуатации можно было получить из attack_data.

Для устранения уязвимости достаточно отключить сеть в песочнице, убрав флаг `.inherit_network()`.

---

Собрать сплойт можно в контейнере, нужные рецепты есть в `Makefile`:

```bash
make all
```
