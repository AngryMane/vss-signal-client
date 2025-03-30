#include <iostream>
#include <thread>
#include <chrono>
#include "signal-client.h"

int main() {
    try {
        // VISSクライアントのインスタンスを取得
        auto client = SignalClient::getInstance("localhost", "8090");

        // 接続を確立
        if (!client->connect()) {
            std::cerr << "接続に失敗しました" << std::endl;
            return 1;
        }

        std::cout << "接続が確立されました" << std::endl;

        // サブスクリプションIDを保存する変数
        std::string subscription_id;

        // サブスクリプションコールバック
        auto callback = [](std::string result) {
            std::cout << "サブスクリプション通知: " << result << std::endl;
        };

        // サブスクリプションを設定
        auto sub_result = client->subscribe_signal_target("Vehicle.Speed", callback);
        if (sub_result) {
            subscription_id = *sub_result;
            std::cout << "サブスクリプションが設定されました。ID: " << subscription_id << std::endl;
        }

        // 現在の速度を取得
        auto speed_result = client->read_signal_current("Vehicle.Speed", 5000); // 5秒のタイムアウト
        if (speed_result) {
            std::cout << "現在の速度: " << *speed_result << std::endl;
        }

        // 目標速度を設定
        client->write_signal_target("Vehicle.Speed", "50", 5000); // 5秒のタイムアウト

        // 10秒間待機
        std::this_thread::sleep_for(std::chrono::seconds(10));

        // サブスクリプションを解除
        if (!subscription_id.empty()) {
            client->unsubscribe_signal_target(subscription_id);
            std::cout << "サブスクリプションを解除しました" << std::endl;
        }

        // 接続を切断
        client->disconnect();
        std::cout << "接続を切断しました" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "エラーが発生しました: " << e.what() << std::endl;
        return 1;
    }

    return 0;
} 