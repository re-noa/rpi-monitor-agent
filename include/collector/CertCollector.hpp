#ifndef CERT_COLLECTOR_HPP
#define CERT_COLLECTOR_HPP

#include <optional>
#include <string>

class CertCollector {
public:
    CertCollector();
    // Days before the site's TLS certificate expires (negative once expired),
    // or nothing if the certificate cannot be read
    std::optional<int> getDaysUntilExpiry(const std::string& url);
private:
    static size_t throwAwayCallback(void* contents, size_t size, size_t nmemb, void* userp);
};

#endif
