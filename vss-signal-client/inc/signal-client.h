#pragma once
#include <string>
#include <memory>
#include <optional>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>
#include <string>
#include <map>
#include <functional>
#include <thread>
#include <memory>

class SignalClient
{
public:
    SignalClient(const std::string &host, const std::string &port);
    virtual ~SignalClient() {};

    static std::shared_ptr<SignalClient> getInstance(const std::string &host, const std::string &port);

    virtual
    bool connect() = 0;

    virtual
    bool disconnect() = 0;

public:

    virtual
    std::optional<std::string>
    read_signal_target(std::string path, int timeout_millisec) = 0;

    virtual
    std::optional<std::string>
    read_signal_current(std::string path, int timeout_millisec) = 0;

    virtual
    std::optional<std::string>
    write_signal_target(std::string path, std::string value, int timeout_millisec) = 0;

    virtual
    std::optional<std::string>
    write_signal_current(std::string path, std::string value, int timeout_millisec) = 0;

    virtual
    std::optional<std::string>
    subscribe_signal_target(std::string path, std::function<void(std::string result)> callback, int timeout_millisec = -1) = 0;

    virtual
    std::optional<std::string>
    subscribe_signal_current(std::string path, std::function<void(std::string result)> callback, int timeout_millisec = -1) = 0;

    virtual
    std::optional<std::string>
    unsubscribe_signal_target(std::string subscribe_id, int timeout_millisec = -1) = 0;

    virtual
    std::optional<std::string>
    unsubscribe_signal_current(std::string subscribe_id, int timeout_millisec = -1) = 0;

};
