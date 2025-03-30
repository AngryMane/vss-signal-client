#include "signal-client-impl.h"
#include "common.h"
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <nlohmann/json.hpp>
#include <random>

using json = nlohmann::json;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

using Client = websocketpp::client<websocketpp::config::asio_client>;

SignalClientImpl::SignalClientImpl(const std::string &host, const std::string &port)
    : SignalClient(host, port)
    , host_(host)
    , port_(port)
    , endpoint_(std::make_unique<Client>())
    , connection_hdl_()
    , mutex_()
    , subscribers_()
{
    endpoint_->init_asio();
    endpoint_->set_open_handler(bind(&SignalClientImpl::on_open, this, _1));
    endpoint_->set_message_handler(bind(&SignalClientImpl::on_message, this, _1, _2));
}

SignalClientImpl::~SignalClientImpl() {
    disconnect();
}

bool SignalClientImpl::connect() {
    websocketpp::lib::error_code ec;
    std::string uri = "ws://" + host_ + ":" + port_;
    
    Client::connection_ptr con = endpoint_->get_connection(uri, ec);
    if (ec) {
        return false;
    }

    connection_hdl_ = con->get_handle();
    endpoint_->connect(con);

    endpoint_thread_ = std::thread([this]() {
        endpoint_->run();
    });

    return true;
}

bool SignalClientImpl::disconnect() {
    if (endpoint_) {
        endpoint_->close(connection_hdl_, websocketpp::close::status::normal, "");
        if (endpoint_thread_.joinable()) {
            endpoint_thread_.join();
        }
    }
    return true;
}

void SignalClientImpl::on_open(websocketpp::connection_hdl hdl) {
    connection_hdl_ = hdl;
}

void SignalClientImpl::on_message(websocketpp::connection_hdl hdl, Client::message_ptr msg) {
    try {
        json j = json::parse(msg->get_payload());
        
        if (j.contains(Constants::viss::SUBSCRIPTION_ID_TAG)) {
            std::string subscription_id = j[Constants::viss::SUBSCRIPTION_ID_TAG];
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = subscribers_.find(subscription_id);
            if (it != subscribers_.end()) {
                it->second(j.dump());
            }
        }
    } catch (const std::exception& e) {
        // Handle error
    }
}

std::string SignalClientImpl::generate_request_id() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 999999);
    return std::to_string(dis(gen));
}

std::optional<std::string> SignalClientImpl::read_signal_target(std::string path, int timeout_millisec) {
    json request = {
        {"action", "get"},
        {"path", path},
        {"requestId", generate_request_id()}
    };

    Subscriber subscriber;
    std::string request_id = request["requestId"];
    {
        std::lock_guard<std::mutex> lock(mutex_);
        subscribers_[request_id] = [&subscriber](const std::string& value) {
            subscriber.received_value_ = value;
            subscriber.is_received_ = true;
            subscriber.cond_.notify_one();
        };
    }

    endpoint_->send(connection_hdl_, request.dump(), websocketpp::frame::opcode::text);

    if (subscriber.wait(timeout_millisec)) {
        return subscriber.received_value_;
    }
    return std::nullopt;
}

std::optional<std::string> SignalClientImpl::read_signal_current(std::string path, int timeout_millisec) {
    json request = {
        {"action", "get"},
        {"path", path},
        {"requestId", generate_request_id()}
    };

    Subscriber subscriber;
    std::string request_id = request["requestId"];
    {
        std::lock_guard<std::mutex> lock(mutex_);
        subscribers_[request_id] = [&subscriber](const std::string& value) {
            subscriber.received_value_ = value;
            subscriber.is_received_ = true;
            subscriber.cond_.notify_one();
        };
    }

    endpoint_->send(connection_hdl_, request.dump(), websocketpp::frame::opcode::text);

    if (subscriber.wait(timeout_millisec)) {
        return subscriber.received_value_;
    }
    return std::nullopt;
}

std::optional<std::string> SignalClientImpl::write_signal_target(std::string path, std::string value, int timeout_millisec) {
    json request = {
        {"action", "set"},
        {"path", path},
        {"value", value},
        {"requestId", generate_request_id()}
    };

    Subscriber subscriber;
    std::string request_id = request["requestId"];
    {
        std::lock_guard<std::mutex> lock(mutex_);
        subscribers_[request_id] = [&subscriber](const std::string& value) {
            subscriber.received_value_ = value;
            subscriber.is_received_ = true;
            subscriber.cond_.notify_one();
        };
    }

    endpoint_->send(connection_hdl_, request.dump(), websocketpp::frame::opcode::text);

    if (subscriber.wait(timeout_millisec)) {
        return subscriber.received_value_;
    }
    return std::nullopt;
}

std::optional<std::string> SignalClientImpl::write_signal_current(std::string path, std::string value, int timeout_millisec) {
    json request = {
        {"action", "set"},
        {"path", path},
        {"value", value},
        {"requestId", generate_request_id()}
    };

    Subscriber subscriber;
    std::string request_id = request["requestId"];
    {
        std::lock_guard<std::mutex> lock(mutex_);
        subscribers_[request_id] = [&subscriber](const std::string& value) {
            subscriber.received_value_ = value;
            subscriber.is_received_ = true;
            subscriber.cond_.notify_one();
        };
    }

    endpoint_->send(connection_hdl_, request.dump(), websocketpp::frame::opcode::text);

    if (subscriber.wait(timeout_millisec)) {
        return subscriber.received_value_;
    }
    return std::nullopt;
}

std::optional<std::string> SignalClientImpl::subscribe_signal_target(std::string path, std::function<void(std::string result)> callback, int timeout_millisec) {
    json request = {
        {"action", "subscribe"},
        {"path", path},
        {"requestId", generate_request_id()}
    };

    Subscriber subscriber;
    std::string request_id = request["requestId"];
    {
        std::lock_guard<std::mutex> lock(mutex_);
        subscribers_[request_id] = [&subscriber](const std::string& value) {
            subscriber.received_value_ = value;
            subscriber.is_received_ = true;
            subscriber.cond_.notify_one();
        };
    }

    endpoint_->send(connection_hdl_, request.dump(), websocketpp::frame::opcode::text);

    if (subscriber.wait(timeout_millisec)) {
        json response = json::parse(subscriber.received_value_);
        if (response.contains("subscriptionId")) {
            std::string subscription_id = response["subscriptionId"];
            {
                std::lock_guard<std::mutex> lock(mutex_);
                subscribers_[subscription_id] = callback;
            }
            return subscription_id;
        }
    }
    return std::nullopt;
}

std::optional<std::string> SignalClientImpl::subscribe_signal_current(std::string path, std::function<void(std::string result)> callback, int timeout_millisec) {
    json request = {
        {"action", "subscribe"},
        {"path", path},
        {"requestId", generate_request_id()}
    };

    Subscriber subscriber;
    std::string request_id = request["requestId"];
    {
        std::lock_guard<std::mutex> lock(mutex_);
        subscribers_[request_id] = [&subscriber](const std::string& value) {
            subscriber.received_value_ = value;
            subscriber.is_received_ = true;
            subscriber.cond_.notify_one();
        };
    }

    endpoint_->send(connection_hdl_, request.dump(), websocketpp::frame::opcode::text);

    if (subscriber.wait(timeout_millisec)) {
        json response = json::parse(subscriber.received_value_);
        if (response.contains("subscriptionId")) {
            std::string subscription_id = response["subscriptionId"];
            {
                std::lock_guard<std::mutex> lock(mutex_);
                subscribers_[subscription_id] = callback;
            }
            return subscription_id;
        }
    }
    return std::nullopt;
}

std::optional<std::string> SignalClientImpl::unsubscribe_signal_target(std::string subscribe_id, int timeout_millisec) {
    json request = {
        {"action", "unsubscribe"},
        {"subscriptionId", subscribe_id},
        {"requestId", generate_request_id()}
    };

    Subscriber subscriber;
    std::string request_id = request["requestId"];
    {
        std::lock_guard<std::mutex> lock(mutex_);
        subscribers_[request_id] = [&subscriber](const std::string& value) {
            subscriber.received_value_ = value;
            subscriber.is_received_ = true;
            subscriber.cond_.notify_one();
        };
    }

    endpoint_->send(connection_hdl_, request.dump(), websocketpp::frame::opcode::text);

    if (subscriber.wait(timeout_millisec)) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            subscribers_.erase(subscribe_id);
        }
        return subscriber.received_value_;
    }
    return std::nullopt;
}

std::optional<std::string> SignalClientImpl::unsubscribe_signal_current(std::string subscribe_id, int timeout_millisec) {
    json request = {
        {"action", "unsubscribe"},
        {"subscriptionId", subscribe_id},
        {"requestId", generate_request_id()}
    };

    Subscriber subscriber;
    std::string request_id = request["requestId"];
    {
        std::lock_guard<std::mutex> lock(mutex_);
        subscribers_[request_id] = [&subscriber](const std::string& value) {
            subscriber.received_value_ = value;
            subscriber.is_received_ = true;
            subscriber.cond_.notify_one();
        };
    }

    endpoint_->send(connection_hdl_, request.dump(), websocketpp::frame::opcode::text);

    if (subscriber.wait(timeout_millisec)) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            subscribers_.erase(subscribe_id);
        }
        return subscriber.received_value_;
    }
    return std::nullopt;
}
