#include "collector/WebCollector.hpp"
#include <curl/curl.h>

WebCollector::WebCollector() {}

size_t WebCollector::throwAwayCallback(void*, size_t size, size_t nmemb, void*) {
    return size * nmemb;
}

bool WebCollector::isSiteUp(const std::string& url) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    long response_code = 0;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L); 
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, throwAwayCallback);

    CURLcode res = curl_easy_perform(curl);
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    }

    curl_easy_cleanup(curl);
    return (response_code >= 200 && response_code < 400);
}
