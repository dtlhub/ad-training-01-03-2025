# AD Training 01.03.2025

[![check-services](https://github.com/dtlhub/ad-training-01-03-2025/actions/workflows/check-services.yml/badge.svg?branch=master&event=push)](https://github.com/dtlhub/ad-training-01-03-2025/actions/workflows/check-services.yml)

## Services

| Service                                        | Language | Vulns                                                                                                                                     | Authors                                                                  |
| ---------------------------------------------- | -------- | ----------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------ |
| [bimba](services/bimba/)                       | Java     | JWT key leak via XXE in docx                                                                                                              | @Demura                                                                  |
| [FileStorage](services/FileStorage/)           | Python   | sql injection in change() in db.py; LFI to rce in /upload; idor in /getfile?id={any id}; logic vuln in /register (you can overwrite user) | [Sonya_nyaaa](https://t.me/Sonya_nyaaa), [SEVA](https://t.me/CollapSeva) |
| [magazinchik](services/magazinchik/)           | C++      | Buffer overflow in add_user, password reset in /forgot                                                                                    | [@ash](https://t.me/andrei_shpak_1523)                                   |
| [rce-as-a-service](services/rce-as-a-service/) | Rust     | Password leak via wasip2 socket api                                                                                                       | [@LeKSuS](https://github.com/LeKSuS-04)                                  |
| [shpagodrach](services/shpagodrach/)           | C        | leak flags over format string, RCE via null-termination error and vtable overwrite                                                        | @azod002                                                                 |

## Infrastructure

- DevOps: [@LeKSuS](https://github.com/LeKSuS-04)
- Checksystem: [ForcAD](https://github.com/pomo-mondreganto/ForcAD)

## Writeups

- [bimba](/sploits/bimba/)
- [FileStorage](/sploits/FileStorage/)
- [magazinchik](/sploits/magazinchik/)
- [rce-as-a-service](/sploits/rce-as-a-service/)
- [shpagodrach](/sploits/shpagodrach/)
