#include "signal-client.h"
#include "signal-client-impl.h"

SignalClient::SignalClient(const std::string &host, const std::string &port) {
}

std::shared_ptr<SignalClient> SignalClient::getInstance(const std::string &host, const std::string &port){
    static std::shared_ptr<SignalClient> instance = nullptr;
    if (!instance) {
        instance = std::make_shared<SignalClientImpl>(host, port);
    }
    return instance;
}
