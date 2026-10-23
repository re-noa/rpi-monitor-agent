#include "collector/CertCollector.hpp"
#include <curl/curl.h>
#include <ctime>
#include <cstring>
#include <cmath>

CertCollector::CertCollector() {}

size_t CertCollector::throwAwayCallback(void*, size_t size, size_t nmemb, void*) {
    return size * nmemb;
}

std::optional<int> CertCollector::getDaysUntilExpiry(const std::string& url) {
    CURL* curl = curl_easy_init();
    if (!curl) return std::nullopt;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_CERTINFO, 1L);
    // Only the expiry date is read here: verification is disabled so that an
    // already expired certificate is still reported instead of failing silently
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, throwAwayCallback);

    std::optional<int> days;
    if (curl_easy_perform(curl) == CURLE_OK) {
        struct curl_certinfo* certinfo = nullptr;
        if (curl_easy_getinfo(curl, CURLINFO_CERTINFO, &certinfo) == CURLE_OK
            && certinfo && certinfo->num_of_certs > 0) {
            // certs[0] is the server's own certificate
            for (struct curl_slist* item = certinfo->certinfo[0]; item; item = item->next) {
                const char* prefix = "Expire date:";
                if (std::strncmp(item->data, prefix, std::strlen(prefix)) != 0) continue;

                struct tm expiry {};
                // Format: "Jan 21 13:17:46 2027 GMT"
                if (strptime(item->data + std::strlen(prefix), "%b %d %H:%M:%S %Y", &expiry)) {
                    double seconds = std::difftime(timegm(&expiry), std::time(nullptr));
                    days = static_cast<int>(std::floor(seconds / 86400));
                }
                break;
            }
        }
    }

    curl_easy_cleanup(curl);
    return days;
}
