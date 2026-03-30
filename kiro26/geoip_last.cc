// geoip_last.cc — reads /var/log/wtmp, prints GeoIP city/country per login IP
// Build: g++ -std=c++23 -o geoip_last geoip_last.cc -lcurl
// Usage: ./geoip_last [wtmp_file]

// ── platform detection ────────────────────────────────────────────────────────
#if defined(__APPLE__)
#  include <AvailabilityMacros.h>
#  include <utmpx.h>          // macOS uses utmpx, not utmp
#  define UTMP_STRUCT utmpx
#  define UT_HOST     ut_host
#  define USER_PROC   USER_PROCESS
#elif defined(__linux__)
#  include <utmp.h>
#  define UTMP_STRUCT utmp
#  define UT_HOST     ut_host
#  define USER_PROC   USER_PROCESS
#else
#  error "Unsupported platform"
#endif

#include <curl/curl.h>

#include <arpa/inet.h>
#include <netdb.h>

#include <format>
#include <fstream>
#include <iostream>
#include <print>
#include <string>
#include <unordered_map>

// ── libcurl write callback ────────────────────────────────────────────────────
static size_t write_cb(char* ptr, size_t size, size_t nmemb, std::string* out) {
    out->append(ptr, size * nmemb);
    return size * nmemb;
}

// ── Minimal JSON field extractor ──────────────────────────────────────────────
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
    struct in_addr a{}; struct in6_addr a6{};
    if (inet_pton(AF_INET, host, &a) == 1 || inet_pton(AF_INET6, host, &a6) == 1)
        return host;

    addrinfo hints{}, *res{};
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(host, nullptr, &hints, &res) != 0) return {};

    char buf[INET6_ADDRSTRLEN]{};
    if (res->ai_family == AF_INET)
        inet_ntop(AF_INET,  &reinterpret_cast<sockaddr_in* >(res->ai_addr)->sin_addr,  buf, sizeof buf);
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

    UTMP_STRUCT entry{};
    while (f.read(reinterpret_cast<char*>(&entry), sizeof entry)) {
        if (entry.ut_type != USER_PROC) continue;
        const char* host = entry.UT_HOST;
        if (host[0] == '\0') continue;

        std::string ip = resolve(host);
        if (ip.empty()) continue;

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
    return 0;
}
