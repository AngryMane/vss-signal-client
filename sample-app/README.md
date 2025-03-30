# VSS Signal Client サンプルアプリケーション

このサンプルアプリケーションは、vss-signal-clientライブラリを使用して、Kuksa Data Brokerと通信する方法を示します。

## 機能

- Kuksa Data Brokerへの接続
- 車両速度の現在値の読み取り
- 車両速度の目標値の設定
- 車両速度の変更のサブスクリプション

## ビルド方法

```bash
# ビルドディレクトリの作成
mkdir build
cd build

# CMakeの設定
cmake ..

# ビルド
make
```

## 実行方法

1. まず、Kuksa Data Brokerを起動します：
```bash
docker run -it --rm --name Server --network kuksa ghcr.io/eclipse-kuksa/kuksa-databroker:0.5.0 --insecure enable-viss &
socat TCP-LISTEN:8090,fork TCP:$(docker inspect -f '{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' Server):8090 &
```

2. サンプルアプリケーションを実行します：
```bash
./sample-app
```

## 動作の説明

1. アプリケーションは、Kuksa Data Brokerに接続を試みます
2. 接続が成功すると、以下の操作を実行します：
   - 車両速度の現在値を読み取ります
   - 車両速度の目標値を50に設定します
   - 車両速度の変更をサブスクライブします
3. 10秒間待機した後、サブスクリプションを解除し、接続を切断します

## 注意事項

- このサンプルを実行する前に、Kuksa Data Brokerが起動していることを確認してください
- デフォルトでは、localhost:8090に接続を試みます
- エラーが発生した場合は、適切なエラーメッセージが表示されます 