#include "notifier/HeartbeatNotifier.hpp"
#include <curl/curl.h>

HeartbeatNotifier::HeartbeatNotifier(const std::string& url) : m_url(url) {}

size_t HeartbeatNotifier::throwAwayCallback(void*, size_t size, size_t nmemb, void*) {
    return size * nmemb;
}

bool HeartbeatNotifier::isEnabled() const {
    return !m_url.empty();
}

bool HeartbeatNotifier::ping() {
    if (!isEnabled()) return false;

    CURL* curl = curl_easy_init();
    if (!curl) return false;

    long response_code = 0;
    curl_easy_setopt(curl, CURLOPT_URL, m_url.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, throwAwayCallback);

    CURLcode res = curl_easy_perform(curl);
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    }

    curl_easy_cleanup(curl);
    return response_code >= 200 && response_code < 300;
}
