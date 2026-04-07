#ifndef DISCORD_BOT_NOTIFIER_HPP
#define DISCORD_BOT_NOTIFIER_HPP

#include <string>

class DiscordBotNotifier {
public:
    DiscordBotNotifier(const std::string& token, const std::string& userId);
    bool sendMessage(const std::string& message);

private:
    std::string m_token;
    std::string m_userId;
    std::string m_dmChannelId;

    bool createDmChannel();
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
};

#endif
