#pragma once

#include <mutex>
#include <condition_variable>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>
#include <string>

#include "signal-client.h"


class SignalClientImpl : public SignalClient, public std::enable_shared_from_this<SignalClientImpl>
{
private:
    class Subscriber {
    public:
        Subscriber()
        : mutex_()
        , is_received_(false)
        , received_value_() 
        , cond_()
        {}

        bool wait(int milli_seconds = -1) {
            std::unique_lock<std::mutex> lock(mutex_);
            if (milli_seconds < 0) {
                cond_.wait(lock, [this]() { return is_received_; });
                return true;
            }

            bool ret = cond_.wait_for(lock, std::chrono::milliseconds(milli_seconds), [this]() { return is_received_; });
            return ret;
        }

        std::mutex mutex_;
        bool is_received_;
        std::string received_value_;
        std::condition_variable cond_;
    };

public:

    SignalClientImpl(const std::string &host, const std::string &port);

    ~SignalClientImpl();

    bool
    connect() override;

    bool
    disconnect() override;

    std::optional<std::string>
    read_signal_target(std::string path, int timeout_millisec) override;

    std::optional<std::string>
    read_signal_current(std::string path, int timeout_millisec) override;
    
    std::optional<std::string>
    write_signal_target(std::string path, std::string value, int timeout_millisec) override;

    std::optional<std::string>
    write_signal_current(std::string path, std::string value, int timeout_millisec) override;

    std::optional<std::string>
    subscribe_signal_target(std::string path, std::function<void(std::string result)> callback, int timeout_millisec = -1) override;

    std::optional<std::string>
    subscribe_signal_current(std::string path, std::function<void(std::string result)> callback, int timeout_millisec = -1) override;

    std::optional<std::string>
    unsubscribe_signal_target(std::string subscribe_id, int timeout_millisec = -1) override;

    std::optional<std::string>
    unsubscribe_signal_current(std::string subscribe_id, int timeout_millisec = -1) override;

private:

    std::string host_;

    std::string port_;

    std::unique_ptr<websocketpp::client<websocketpp::config::asio_client>> endpoint_;
    websocketpp::connection_hdl connection_hdl_;
    std::mutex mutex_;
    std::map<std::string, std::function<void(const std::string&)>> subscribers_;
    std::thread endpoint_thread_;

    void on_open(websocketpp::connection_hdl hdl);
    void on_message(websocketpp::connection_hdl hdl, websocketpp::client<websocketpp::config::asio_client>::message_ptr msg);
    std::string generate_request_id();
};