#ifndef NTFY_NOTIFIER_HPP
#define NTFY_NOTIFIER_HPP

#include <string>

// Sends push notifications through a ntfy server (https://ntfy.sh, self-hostable).
class NtfyNotifier {
public:
    NtfyNotifier(const std::string& serverUrl, const std::string& topic, const std::string& token);

    // priority: "min", "low", "default", "high" or "urgent"
    bool sendMessage(const std::string& title,
                     const std::string& message,
                     const std::string& priority = "high");

private:
    std::string m_serverUrl;
    std::string m_topic;
    std::string m_token;

    static size_t throwAwayCallback(void* contents, size_t size, size_t nmemb, void* userp);
};

#endif
