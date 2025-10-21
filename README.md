# AtCoder 用コンテナイメージ

[AtCoder 参加用環境](https://github.com/smkwlab/atcoder-env) で利用することを想定したコンテナイメージ。

## イメージバリエーション

用途に応じて選択できる2種類のイメージを提供しています：

### 🎯 ライト版 (`atcoder-lite:2025`)
**推奨：一般的な競技プログラミング用**
- サイズ: ~3-4GB
- ビルド時間: 30-60分
- 競技プログラミングの必須ライブラリのみ搭載
- PyTorch、LibTorch、重い機械学習ライブラリを除外

### 🚀 フル版 (`atcoder-full:2025`)
**機械学習・数値計算が必要な場合**
- サイズ: ~8GB  
- ビルド時間: 1-2時間
- 機械学習、深層学習ライブラリを含む全ライブラリ搭載
- PyTorch、LibTorch、torch-rb、scikit-learn等を含む

## 対応プログラミング言語

両イメージとも以下の言語に対応しています：

- **Java** (JDK 23.0.1)
- **Ruby** (3.4.4 with GC patch)
- **Elixir** (1.18.4)
- **Python3** (3.13.5 with LTO/BOLT optimizations)
- **JavaScript** (Node.js 22.16.0)
- **C++** (g++ 12.3.0, C++23 support)
- **Rust** (1.70.0)
- **Erlang** (28.0)

## 主要な違い

| ライブラリ | ライト版 | フル版 |
|------------|----------|--------|
| 基本科学計算 (NumPy, SciPy等) | ✅ | ✅ |
| AC Library (各言語) | ✅ | ✅ |
| online-judge-tools | ✅ | ✅ |
| atcoder-cli | ✅ | ✅ |
| PyTorch | ❌ | ✅ |
| LibTorch (C++) | ❌ | ✅ |
| torch-rb (Ruby) | ❌ | ✅ |
| scikit-learn | ❌ | ✅ |
| EXLA/Nx (Elixir) | ❌ | ✅ |

## ビルド方法

### 両方のイメージをビルド
```bash
./build-both.sh
```

### 個別にビルド
```bash
# ライト版のみ
./build-both.sh lite

# フル版のみ  
./build-both.sh full
```

### 手動ビルド
```bash
# ライト版
docker build -f Dockerfile.lite -t atcoder-lite:2025 .

# フル版
docker build -f Dockerfile -t atcoder-full:2025 .
```

## 使用方法

ビルドしたイメージは [AtCoder 参加用環境](https://github.com/smkwlab/atcoder-env) で利用できます。通常の競技プログラミングにはライト版を、機械学習系の問題にはフル版をお使いください。
