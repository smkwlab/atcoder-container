# AtCoder 用コンテナイメージ

[AtCoder 参加用環境](https://github.com/smkwlab/atcoder-env) で利用することを想定したコンテナイメージ。

## 対応プログラミング言語

この docker イメージでは、以下の言語に対応している。

- Java (JDK 17)
- Ruby (3.2.2)
- Elixir (1.15.2)
- Python3 (3.11.4)
- JavaScript (node.js 18.16.1)
- C++ (g++ 12.3.0)
- Rust (1.17.0)
- Erlang (26.0.2)

イメージを作り直したい場合、[Dockerfile](Dockerfile) を編集後、
`docker build -t atcoder-container:latest .` を実行する。
