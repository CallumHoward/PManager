// CommsManager.cpp

#include <string>
#include <map>
#include <cstdlib>

#include "cinder/Log.h"
#include "cinder/osc/Osc.h"
#include "cinder/Timeline.h"

#include "CommsManager.hpp"


namespace ch {

void CommsManager::setup(const std::string& listenPath,
        const std::function<void(int, int)>& updateCallback) {
    addListener(listenPath, updateCallback);

    try {
        // Bind the receiver to the endpoint. This function may throw.
        mReceiver.bind();

    } catch (const osc::Exception &ex) {
        CI_LOG_E("Error binding: " << ex.what() << " val: " << ex.value());
        throw ex;
    }

    // UDP opens the socket and "listens" accepting any message from any
    // endpoint. The listen function takes an error handler for the underlying
    // socket. Any errors that would call this function are because of problems
    // with the socket or with the remote message.
    mReceiver.listen(
            [](asio::error_code error, protocol::endpoint endpoint) -> bool {
                if (error) {
                    CI_LOG_E("Error Listening: "
                            << error.message()
                            << " val: " << error.value()
                            << " endpoint: " << endpoint);
                    return false;
                } else {
                    return true;
                }
            });

    try {
        // Bind the sender to the endpoint. This function may throw. The
        // exception will contain asio::error_code information.
        mSender.bind();
    } catch (const osc::Exception &ex) {
        CI_LOG_E("Error binding: " << ex.what() << " val: " << ex.value());
        // quit();
    }

    // Udp doesn't "connect" the same way Tcp does. If bind doesn't throw, we
    // can consider ourselves connected.
    mIsConnected = true;
}

void CommsManager::addListener(const std::string& listenPath,
        const std::function<void(int, int)>& updateCallback) {

    mUpdateCallbacks[listenPath] = updateCallback;

    mReceiver.setListener(listenPath,
            [&](const osc::Message &msg) {
                const auto index = msg[0].int32();
                const auto value = msg[1].int32();
                mUpdateCallbacks.at(msg.getAddress())(index, value);
            });
}

// generate random events to test with MaxMSP
void CommsManager::generateEvent() {
    // Make sure you're connected before trying to send.
    if (!mIsConnected) { return; }

    const auto root = "/move";
    bool trigger = true;

    osc::Message msg(root);
    msg.append(static_cast<int>(trigger));

    // Send the msg and also provide an error handler. If the message is
    // important you could store it in the error callback to dispatch it again
    // if there was a problem.
    mSender.send(msg,
            std::bind(&CommsManager::onSendError, this, std::placeholders::_1));
}

// Unified error handler. Easiest to have a bound function in this situation,
// since we're sending from many different places.
void CommsManager::onSendError(asio::error_code error) {
    if (error) {
        CI_LOG_E("Error sending: " << error.message()
                                   << " val: " << error.value());

        // If you determine that this error is fatal, make sure to flip
        // mIsConnected. It's possible that the error isn't fatal.
        mIsConnected = false;
        try {
            // Close the socket on exit. This function could throw. The
            // exception will contain asio::error_code information.
            mSender.close();

        } catch (const osc::Exception &ex) {
            CI_LOG_EXCEPTION("Cleaning up socket: val -" << ex.value(), ex);
            throw ex;
        }
    }
}

} // namespace ch
