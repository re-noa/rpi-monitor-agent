#ifndef HEARTBEAT_NOTIFIER_HPP
#define HEARTBEAT_NOTIFIER_HPP

#include <string>

// Pings an external dead man's switch (e.g. healthchecks.io).
// If the pings stop (Pi down, agent crashed, network lost), the external
// service sends the alert, since the agent itself no longer can.
class HeartbeatNotifier {
public:
    HeartbeatNotifier(const std::string& url);
    bool isEnabled() const;
    bool ping();

private:
    std::string m_url;
    static size_t throwAwayCallback(void* contents, size_t size, size_t nmemb, void* userp);
};

#endif
