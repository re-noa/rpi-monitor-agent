#include "notifier/DiscordBotNotifier.hpp"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

DiscordBotNotifier::DiscordBotNotifier(const std::string& token, const std::string& userId)
    : m_token(token), m_userId(userId) {}

size_t DiscordBotNotifier::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

bool DiscordBotNotifier::createDmChannel() {
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    std::string responseBuffer;
    std::string url = "https://discord.com/api/v10/users/@me/channels";
    std::string authHeader = "Authorization: Bot " + m_token;
    
    json body;
    body["recipient_id"] = m_userId;
    std::string payload = body.dump();

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, authHeader.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBuffer);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK) {
        try {
            auto j = json::parse(responseBuffer);
            m_dmChannelId = j["id"];
            return true;
        } catch (...) {
            return false;
        }
    }
    return false;
}

bool DiscordBotNotifier::sendMessage(const std::string& message) {
    if (m_dmChannelId.empty() && !createDmChannel()) {
        return false;
    }

    CURL* curl = curl_easy_init();
    if (!curl) return false;

    std::string url = "https://discord.com/api/v10/channels/" + m_dmChannelId + "/messages";
    std::string authHeader = "Authorization: Bot " + m_token;
    
    json body;
    body["content"] = message;
    std::string payload = body.dump();

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, authHeader.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return (res == CURLE_OK);
}
