#include "notifier/NtfyNotifier.hpp"
#include <curl/curl.h>
#include <nlohmann/json.hpp>

NtfyNotifier::NtfyNotifier(const std::string& serverUrl, const std::string& topic, const std::string& token)
    : m_serverUrl(serverUrl), m_topic(topic), m_token(token) {}

size_t NtfyNotifier::throwAwayCallback(void*, size_t size, size_t nmemb, void*) {
    return size * nmemb;
}

bool NtfyNotifier::sendMessage(const std::string& title, const std::string& message, const std::string& priority) {
    if (m_serverUrl.empty() || m_topic.empty()) return false;

    CURL* curl = curl_easy_init();
    if (!curl) return false;

    // JSON publishing keeps UTF-8 intact (headers would not)
    nlohmann::json body = {
        {"topic", m_topic},
        {"title", title},
        {"message", message},
        {"priority", priority == "min" ? 1 : priority == "low" ? 2 : priority == "high" ? 4 : priority == "urgent" ? 5 : 3},
    };
    std::string payload = body.dump();

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    std::string authHeader = "Authorization: Bearer " + m_token;
    if (!m_token.empty()) headers = curl_slist_append(headers, authHeader.c_str());

    long response_code = 0;
    curl_easy_setopt(curl, CURLOPT_URL, m_serverUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, throwAwayCallback);

    CURLcode res = curl_easy_perform(curl);
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return response_code >= 200 && response_code < 300;
}
