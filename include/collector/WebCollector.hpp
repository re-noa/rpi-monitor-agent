#ifndef WEB_COLLECTOR_HPP
#define WEB_COLLECTOR_HPP

#include <string>

class WebCollector {
public:
    WebCollector();
    bool isSiteUp(const std::string& url);
private:
    static size_t throwAwayCallback(void* contents, size_t size, size_t nmemb, void* userp);
};

#endif
