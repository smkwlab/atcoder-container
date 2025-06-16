# AtCoder 用コンテナイメージ

[AtCoder 参加用環境](https://github.com/smkwlab/atcoder-env) で利用することを想定したコンテナイメージ。

## 対応プログラミング言語

この docker イメージでは、以下の言語に対応している。

- Java (JDK 23.0.1)
- Ruby (3.4.4)
- Elixir (1.18.4)
- Python3 (3.13.5)
- JavaScript (node.js 22.16.0)
- C++ (g++ 12.3.0)
- Rust (1.70.0)
- Erlang (28.0)

イメージを作り直したい場合、[Dockerfile](Dockerfile) を編集後、
`docker build -t atcoder-container:latest .` を実行する。
