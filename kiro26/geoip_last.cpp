// geoip_last.cpp — reads /var/log/wtmp, prints GeoIP city/country per login IP
// Build: g++ -std=c++26 -o geoip_last geoip_last.cpp -lcurl
// Usage: ./geoip_last [wtmp_file]

#include <curl/curl.h>
#include <utmp.h>

#include <arpa/inet.h>
#include <netdb.h>

#include <cstring>
#include <format>
#include <fstream>
#include <iostream>
#include <print>
#include <string>
#include <unordered_map>

// ── libcurl write callback ────────────────────────────────────────────────────
static size_t write_cb(char* ptr, size_t, size_t nmemb, std::string* out) {
    out->append(ptr, nmemb);
    return nmemb;
}

// ── Minimal JSON field extractor (no dependency) ──────────────────────────────
static std::string json_field(const std::string& json, const std::string& key) {
    auto pos = json.find(std::format("\"{}\"", key));
    if (pos == std::string::npos) return "?";
    pos = json.find(':', pos);
    if (pos == std::string::npos) return "?";
    ++pos;
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '"')) ++pos;
    auto end = json.find_first_of("\",}", pos);
    return end == std::string::npos ? "?" : json.substr(pos, end - pos);
}

// ── GeoIP lookup via ip-api.com ───────────────────────────────────────────────
struct GeoInfo { std::string city, country; };

static GeoInfo geoip(const std::string& ip) {
    CURL* curl = curl_easy_init();
    if (!curl) return {"?", "?"};

    std::string body;
    auto url = std::format("http://ip-api.com/json/{}?fields=status,city,country", ip);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (json_field(body, "status") != "success") return {"?", "?"};
    return {json_field(body, "city"), json_field(body, "country")};
}

// ── Resolve hostname → IP string ──────────────────────────────────────────────
static std::string resolve(const char* host) {
    // Already an IP?
    struct in_addr a{}; struct in6_addr a6{};
    if (inet_pton(AF_INET, host, &a) == 1 || inet_pton(AF_INET6, host, &a6) == 1)
        return host;

    addrinfo hints{}, *res{};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(host, nullptr, &hints, &res) != 0) return {};

    char buf[INET6_ADDRSTRLEN]{};
    if (res->ai_family == AF_INET)
        inet_ntop(AF_INET, &reinterpret_cast<sockaddr_in*>(res->ai_addr)->sin_addr, buf, sizeof buf);
    else
        inet_ntop(AF_INET6, &reinterpret_cast<sockaddr_in6*>(res->ai_addr)->sin6_addr, buf, sizeof buf);
    freeaddrinfo(res);
    return buf;
}

// ── main ──────────────────────────────────────────────────────────────────────
int main(int argc, char* argv[]) {
    const char* path = argc > 1 ? argv[1] : "/var/log/wtmp";

    std::ifstream f(path, std::ios::binary);
    if (!f) { std::println(stderr, "Cannot open {}", path); return 1; }

    curl_global_init(CURL_GLOBAL_DEFAULT);

    std::unordered_map<std::string, GeoInfo> cache;
    std::println("{:<45} {:<22} {}", "IP", "City", "Country");
    std::println("{}", std::string(75, '-'));

    utmp entry{};
    while (f.read(reinterpret_cast<char*>(&entry), sizeof entry)) {
        // Only USER_PROCESS entries with a remote host (like last.c)
        if (entry.ut_type != USER_PROCESS) continue;
        const char* host = entry.ut_host;
        if (host[0] == '\0') continue;

        std::string ip = resolve(host);
        if (ip.empty()) continue;

        // Skip private/loopback addresses
        if (ip.starts_with("127.") || ip.starts_with("10.") ||
            ip.starts_with("192.168.") || ip == "::1") continue;

        if (!cache.contains(ip)) {
            std::print("Looking up {}...\r", ip);
            std::cout.flush();
            cache[ip] = geoip(ip);
        }
        const auto& [city, country] = cache[ip];
        std::println("{:<45} {:<22} {}", ip, city, country);
    }

    curl_global_cleanup();
}
