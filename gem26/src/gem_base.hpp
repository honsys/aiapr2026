#ifndef GEM_BASE_HPP
#define GEM_BASE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <variant>
#include <thread>
#include <mutex>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstdlib>
#include <fstream>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <csignal>
#include <atomic>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef HAS_IMAGEMAGICK
#include <Magick++.h>
#endif

#ifdef HAS_QUANTLIB
#include <ql/quantlib.hpp>
#endif

// Cython Bridge Header
#include "gem_mistral_bridge.h"

// PDF-Writer Headers
#ifdef HAS_PDFWRITER
#include "PDFWriter.h"
#include "PDFPage.h"
#include "PDFRectangle.h"
#include "PageContentContext.h"
#include "PDFUsedFont.h"
#include "PDFParser.h"
#include "PDFObject.h"
#include "PDFDictionary.h"
#include "PDFObjectCast.h"
#include "PDFArray.h"
#include "PDFStreamInput.h"
#include "PDFLiteralString.h"
#include "PDFHexString.h"
#include "PDFIndirectObjectReference.h"
#endif

// WWW and GIS Headers
#include "httplib.h"
#ifdef HAS_MAPNIK
#include <mapnik/map.hpp>
#include <mapnik/layer.hpp>
#include <mapnik/rule.hpp>
#include <mapnik/feature_type_style.hpp>
#include <mapnik/symbolizer.hpp>
#include <mapnik/datasource_cache.hpp>
#include <mapnik/font_engine_freetype.hpp>
#include <mapnik/agg_renderer.hpp>
#include <mapnik/image.hpp>
#include <mapnik/image_util.hpp>
#include <mapnik/config_error.hpp>
#include <mapnik/load_map.hpp>
#endif

// Fix Bevington naming conflicts
#define pause bev_pause
// Bevington Headers
#include "FitUtil.h"
#include "GenUtil.h"
#include "MonteLib.h"
#include "QuikHist.h"
#undef pause

// Astrophysics module (forward-included after base types)
// gem_astro.hpp is included at the bottom of this file

// Forward declarations
class GemObject;
class GemValue;

// Value types supported by Gem
using GemValueType = std::variant<
    std::monostate, // null/void
    double,
    std::string,
    bool,
    std::shared_ptr<GemObject>,
    std::vector<std::shared_ptr<GemObject>>, // For generic lists
    std::vector<double>, // For numeric arrays (efficient)
    std::vector<std::string>, // For string lists (CLI args etc)
    std::map<std::string, std::shared_ptr<GemValue>>, // For dictionaries/maps
    std::vector<std::shared_ptr<GemValue>> // For list of mixed values
>;

class GemValue {
public:
    GemValueType value;

    GemValue() : value(std::monostate{}) {}
    GemValue(double d) : value(d) {}
    GemValue(const std::string& s) : value(s) {}
    GemValue(bool b) : value(b) {}
    GemValue(std::shared_ptr<GemObject> obj) : value(obj) {}
    GemValue(const std::vector<double>& v) : value(v) {}
    GemValue(const std::vector<std::shared_ptr<GemObject>>& v) : value(v) {}
    GemValue(const std::vector<std::string>& v) : value(v) {}
    GemValue(const std::map<std::string, std::shared_ptr<GemValue>>& m) : value(m) {}
    GemValue(const std::vector<std::shared_ptr<GemValue>>& v) : value(v) {}

    std::string toString() const;
};

// Base class for all Gem objects
class GemObject : public std::enable_shared_from_this<GemObject> {
public:
    std::string name;
    std::shared_ptr<GemObject> parent;
    std::map<std::string, std::shared_ptr<GemValue>> properties;
    
    // Support for both native and user-defined methods
    struct GemMethod {
        std::variant<
            std::function<std::shared_ptr<GemValue>(std::vector<std::shared_ptr<GemValue>>)>,
            std::string // Name of user function or body? Actually better to store the GemFunction name or ptr.
        > func;
        bool isNative = true;

        GemMethod() = default;
        GemMethod(std::function<std::shared_ptr<GemValue>(std::vector<std::shared_ptr<GemValue>>)> f, bool n = true)
            : func(f), isNative(n) {}
        GemMethod(const std::string& f, bool n = false)
            : func(f), isNative(n) {}
    };
    std::map<std::string, GemMethod> methods;

    GemObject(const std::string& n = "Object", std::shared_ptr<GemObject> p = nullptr);
    virtual ~GemObject() = default;
    virtual std::shared_ptr<GemValue> get(const std::string& name) {
        if (properties.count(name)) return properties[name];
        if (methods.count(name)) return std::make_shared<GemValue>("NativeMethod");
        if (parent) return parent->get(name);
        return std::make_shared<GemValue>();
    }

    virtual void set(const std::string& key, std::shared_ptr<GemValue> val) {
        properties[key] = val;
    }

    // Forward declaration of call signature with interpreter context
    virtual std::shared_ptr<GemValue> call(const std::string& methodName, std::vector<std::shared_ptr<GemValue>> args);
    
    static std::shared_ptr<GemObject> sysInstance;
};

inline std::string GemValue::toString() const {
    if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::shared_ptr<GemObject>>(value)) {
        return "Object(" + std::get<std::shared_ptr<GemObject>>(value)->name + ")"; 
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        const auto& v = std::get<std::vector<double>>(value);
        std::string s = "[";
        for (size_t i = 0; i < v.size(); ++i) {
            s += std::to_string(v[i]);
            if (i < v.size() - 1) s += ", ";
        }
        s += "]";
        return s;
    } else if (std::holds_alternative<std::vector<std::shared_ptr<GemObject>>>(value)) {
        const auto& v = std::get<std::vector<std::shared_ptr<GemObject>>>(value);
        std::string s = "[";
        for (size_t i = 0; i < v.size(); ++i) {
            s += "Object(" + v[i]->name + ")";
            if (i < v.size() - 1) s += ", ";
        }
        s += "]";
        return s;
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        const auto& v = std::get<std::vector<std::string>>(value);
        std::string s = "[";
        for (size_t i = 0; i < v.size(); ++i) {
            s += "\"" + v[i] + "\"";
            if (i < v.size() - 1) s += ", ";
        }
        s += "]";
        return s;
    } else if (std::holds_alternative<std::map<std::string, std::shared_ptr<GemValue>>>(value)) {
        const auto& m = std::get<std::map<std::string, std::shared_ptr<GemValue>>>(value);
        std::string s = "{";
        for (auto it = m.begin(); it != m.end(); ++it) {
            s += it->first + ": " + (it->second ? it->second->toString() : "nil");
            if (std::next(it) != m.end()) s += ", ";
        }
        s += "}";
        return s;
    } else if (std::holds_alternative<std::vector<std::shared_ptr<GemValue>>>(value)) {
        const auto& v = std::get<std::vector<std::shared_ptr<GemValue>>>(value);
        std::string s = "[";
        for (size_t i = 0; i < v.size(); ++i) {
            s += (v[i] ? v[i]->toString() : "nil");
            if (i < v.size() - 1) s += ", ";
        }
        s += "]";
        return s;
    }
    return "nil";
}

// Builtin Objects

struct SignalState {
    static std::map<int, std::string> handlers;
    static std::atomic<int> lastSignal;
    static void handle(int sig) {
        lastSignal = sig;
    }
};

inline std::map<int, std::string> SignalState::handlers;
inline std::atomic<int> SignalState::lastSignal{0};

class GemThread : public GemObject {
public:
    std::shared_ptr<std::thread> t;
    std::shared_ptr<GemValue> result;
    std::atomic<bool> finished{false};
    std::atomic<bool> timed_out{false};
    double timeout_seconds = 60.0;
    std::function<std::shared_ptr<GemValue>(const std::string&, std::vector<std::shared_ptr<GemValue>>)> callGemFunc;

    GemThread() : GemObject("thread") {}
    
    void start(const std::string& funcName, 
               std::vector<std::shared_ptr<GemValue>> params, 
               double timeout,
               std::function<std::shared_ptr<GemValue>(const std::string&, std::vector<std::shared_ptr<GemValue>>)> caller) {
        
        timeout_seconds = timeout;
        callGemFunc = caller;
        name = "ThreadInstance(" + funcName + ")";
        
        methods["wait"] = { [this](std::vector<std::shared_ptr<GemValue>> args) {
            if (t && t->joinable()) t->join();
            if (timed_out) return std::make_shared<GemValue>("Error: Timeout (" + std::to_string(timeout_seconds) + "s)");
            return result ? result : std::make_shared<GemValue>();
        }, true };

        methods["is_finished"] = { [this](std::vector<std::shared_ptr<GemValue>> args) {
            return std::make_shared<GemValue>((bool)finished);
        }, true };

        t = std::make_shared<std::thread>([this, funcName, params, caller, timeout]() {
            std::atomic<bool> completed{false};
            std::thread watchdog([this, timeout, &completed]() {
                auto start_time = std::chrono::steady_clock::now();
                while (!completed && !timed_out) {
                    if (std::chrono::duration<double>(std::chrono::steady_clock::now() - start_time).count() > timeout) {
                        timed_out = true;
                        finished = true;
                        break;
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
            });

            if (caller) {
                try {
                    result = caller(funcName, params);
                } catch (...) {
                    result = std::make_shared<GemValue>("Error: Exception in Gem thread");
                }
            }
            completed = true;
            finished = true;
            if (watchdog.joinable()) watchdog.join();
        });
    }

    ~GemThread() {
        if (t && t->joinable()) t->detach();
    }
};

// Sys Object
class GemSys : public GemObject {
public:
    std::vector<std::string> cli_args;
    std::function<std::shared_ptr<GemValue>(const std::string&, std::vector<std::shared_ptr<GemValue>>)> callGemFunc;
    std::function<std::string(const std::string&, const std::string&)> translateFunc;

    GemSys() : GemObject("sys", nullptr) { 
        // Register default signal handlers
#ifdef _WIN32
        for (int sig : {SIGINT, SIGTERM, SIGABRT, SIGFPE, SIGILL, SIGSEGV}) {
#else
        for (int sig : {SIGINT, SIGTERM, SIGHUP, SIGQUIT, SIGABRT, SIGFPE, SIGILL, SIGSEGV, SIGBUS, SIGPIPE, SIGUSR1, SIGUSR2}) {
#endif
            std::signal(sig, SignalState::handle);
        }

        properties["SIGINT"] = std::make_shared<GemValue>((double)SIGINT);
        properties["SIGTERM"] = std::make_shared<GemValue>((double)SIGTERM);
#ifndef _WIN32
        properties["SIGHUP"] = std::make_shared<GemValue>((double)SIGHUP);
        properties["SIGQUIT"] = std::make_shared<GemValue>((double)SIGQUIT);
        properties["SIGUSR1"] = std::make_shared<GemValue>((double)SIGUSR1);
        properties["SIGUSR2"] = std::make_shared<GemValue>((double)SIGUSR2);
#endif

        methods["args"] = { [this](std::vector<std::shared_ptr<GemValue>> args) {
            return std::make_shared<GemValue>(this->cli_args);
        }, true };

        methods["async"] = { [this](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty()) return std::make_shared<GemValue>();
            std::string funcName = args[0]->toString();
            std::vector<std::shared_ptr<GemValue>> params;
            double timeout = 60.0;
            
            if (args.size() > 1) {
                if (std::holds_alternative<std::vector<std::shared_ptr<GemValue>>>(args[1]->value)) {
                    params = std::get<std::vector<std::shared_ptr<GemValue>>>(args[1]->value);
                } else if (std::holds_alternative<std::vector<double>>(args[1]->value)) {
                    for (double d : std::get<std::vector<double>>(args[1]->value)) params.push_back(std::make_shared<GemValue>(d));
                } else if (std::holds_alternative<std::vector<std::string>>(args[1]->value)) {
                    for (const auto& s : std::get<std::vector<std::string>>(args[1]->value)) params.push_back(std::make_shared<GemValue>(s));
                } else if (std::holds_alternative<std::vector<std::shared_ptr<GemObject>>>(args[1]->value)) {
                    for (auto o : std::get<std::vector<std::shared_ptr<GemObject>>>(args[1]->value)) params.push_back(std::make_shared<GemValue>(o));
                } else if (std::holds_alternative<double>(args[1]->value)) {
                    timeout = std::get<double>(args[1]->value);
                }

                if (args.size() > 2 && std::holds_alternative<double>(args[2]->value)) {
                    timeout = std::get<double>(args[2]->value);
                }
            }

            auto threadObj = std::make_shared<GemThread>();
            threadObj->start(funcName, params, timeout, this->callGemFunc);
            return std::make_shared<GemValue>(threadObj);
        }, true };

        methods["sethandler"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.size() < 2) return std::make_shared<GemValue>(false);
            std::string handlerFunc = args[1]->toString();
            std::vector<int> signals;
            
            if (std::holds_alternative<std::vector<double>>(args[0]->value)) {
                for (double d : std::get<std::vector<double>>(args[0]->value)) signals.push_back((int)d);
            } else if (std::holds_alternative<double>(args[0]->value)) {
                signals.push_back((int)std::get<double>(args[0]->value));
            } else if (std::holds_alternative<std::vector<std::shared_ptr<GemValue>>>(args[0]->value)) {
                for (auto& v : std::get<std::vector<std::shared_ptr<GemValue>>>(args[0]->value)) {
                    if (std::holds_alternative<double>(v->value)) signals.push_back((int)std::get<double>(v->value));
                }
            }

            for (int sig : signals) {
                SignalState::handlers[sig] = handlerFunc;
                std::signal(sig, SignalState::handle);
            }
            return std::make_shared<GemValue>(true);
        }, true };

        methods["print"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            for (auto& arg : args) {
                std::cout << arg->toString() << " ";
            }
            std::cout << std::endl;
            return std::make_shared<GemValue>();
        }, true };
        
        methods["host"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
             const char* os = std::getenv("OS");
             if (!os) os = "Unknown (Unix-like?)";
             return std::make_shared<GemValue>(std::string(os));
        }, true };
        
        methods["exec"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty() || !std::holds_alternative<std::string>(args[0]->value)) return std::make_shared<GemValue>(-1.0);
            std::string cmd = std::get<std::string>(args[0]->value);
            int ret = std::system(cmd.c_str());
            return std::make_shared<GemValue>((double)ret);
        }, true };

        methods["langport"] = { [this](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty() || !translateFunc) return std::make_shared<GemValue>(false);
            std::string pattern = args[0]->toString();
            std::string output = (args.size() > 1) ? args[1]->toString() : "";
            
            std::vector<std::string> files;
            if (pattern.find('*') != std::string::npos) {
                std::string ext = "";
                if (pattern.find_last_of('.') != std::string::npos) ext = pattern.substr(pattern.find_last_of('.'));
                for (const auto& entry : std::filesystem::directory_iterator(".")) {
                    if (entry.path().extension() == ext) {
                        files.push_back(entry.path().string());
                    }
                }
            } else {
                files.push_back(pattern);
            }

            if (files.empty()) return std::make_shared<GemValue>(false);
            std::sort(files.begin(), files.end());

            std::string combinedGem = "";
            for (const auto& f : files) {
                std::ifstream ifs(f);
                if (!ifs.is_open()) continue;
                std::string source((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
                std::string ext = std::filesystem::path(f).extension().string();
                std::string lang = "Python";
                if (ext == ".r") lang = "R";
                else if (ext == ".jl") lang = "Julia";
                else if (ext == ".for") lang = "Fortran";
                else if (ext == ".cpp") lang = "C++";

                std::cout << "Porting " << f << " (" << lang << ")..." << std::endl;
                std::string translated = translateFunc(source, lang);
                
                if (output.empty()) {
                    std::string outPath = std::filesystem::path(f).stem().string() + ".g";
                    std::ofstream ofs(outPath);
                    ofs << translated;
                    std::cout << "Saved to " << outPath << std::endl;
                } else {
                    combinedGem += "# --- Ported from " + f + " ---\n" + translated + "\n\n";
                }
            }

            if (!output.empty()) {
                std::ofstream ofs(output);
                ofs << combinedGem;
                std::cout << "Saved combined port to " << output << std::endl;
            }

            return std::make_shared<GemValue>(true);
        }, true };

        methods["help"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty()) {
                std::cout << "\n--- Gem Language Help ---" << std::endl;
                std::cout << "Gem is a high-performance STEM language implemented in C++26." << std::endl;
                std::cout << "Usage Tips:" << std::endl;
                std::cout << "  - All variables must be initialized: 'int x = 0'" << std::endl;
                std::cout << "  - Global variables start with underscore: '_config = 1'" << std::endl;
                std::cout << "  - Use 'use \"file.py\"' to translate and run foreign code." << std::endl;
                std::cout << "  - Use 'langport(\"*.py\", \"out.gm\")' to port and save code." << std::endl;
                std::cout << "  - Use 'lib' to see all loaded modules." << std::endl;
                std::cout << "  - Use 'his' to view today's session history." << std::endl;
                std::cout << "\nCLI Options:" << std::endl;
                std::cout << "  gem <file.g>             - Run a script." << std::endl;
                std::cout << "  gem -c <main.g> [mod...] - Compile multiple files into a binary." << std::endl;
                std::cout << "  gem -o <name>            - Specify output name for compiler." << std::endl;
                std::cout << "  gem -h                   - Print history and exit." << std::endl;
                std::cout << "  gem -t <file> [-o output]- AI-assisted translation to Gem." << std::endl;
                std::cout << "\nAvailable Builtin Modules:" << std::endl;
                std::cout << "  sys, math, ai, text, algo, bev, file, zip, nlp, img, geo, mobl, cpp, tcp, itr, data, container, vm, go, ruby, node, rust, fin, bsm, chart, astro, drvr" << std::endl;
                std::cout << "\nKeywords for Documentation:" << std::endl;
                std::cout << "  fun, obj, use, alias, his, lib, end, if, while, int, double, string, bool, exit" << std::endl;
                std::cout << "\nMobile & Cross-Platform:" << std::endl;
                std::cout << "  use \"mobl.gm\" then: mobl phone(\"name\")  phone.dictate(text)  phone.make_feature(lat,lon,text)" << std::endl;
                std::cout << "  ./gem travel_log.g  →  http://localhost:8080  (Android/iPhone/macOS/Linux/Win11)" << std::endl;
                std::cout << "\nDetailed help: help \"topic\" or help(topic)" << std::endl;
                } else {
                std::string topic = args[0]->toString();
                std::cout << "\n--- Help: " << topic << " ---" << std::endl;
                if (topic == "fin") {
                    std::cout << "Description: Financial engineering and QuantLib integration." << std::endl;
                    std::cout << "Functions:" << std::endl;
                    std::cout << "  - ticker(symbol): Real-time market data via yfinance (returns Ticker object)." << std::endl;
                    std::cout << "  - high_yield_bonds(): Top 50 high yield bonds from TradingView." << std::endl;
                    std::cout << "  - high_yield_etfs(): Top 50 high yield ETFs from TradingView." << std::endl;
                    std::cout << "  - high_yield_equities(): Top 50 high yield equities from TradingView." << std::endl;
                    std::cout << "  - bs_price(type, strike, spot, rate, vol, t): European option pricing." << std::endl;
                    std::cout << "  - greeks(type, strike, spot, rate, vol, t): NPV, Delta, Gamma, Theta, Vega (returns Greeks object)." << std::endl;
                    std::cout << "  - date(d, m, y): Create a QuantLib date object." << std::endl;
                    std::cout << "  - calendar(name): Get a market calendar (e.g., 'USA', 'UK', 'TARGET')." << std::endl;
                    std::cout << "  - is_holiday(cal_name, d, m, y): Check if a date is a market holiday." << std::endl;
                    std::cout << "  - add_days(d, m, y, n): Date arithmetic (returns Date object)." << std::endl;
                    std::cout << "  - diff_days(d1, m1, y1, d2, m2, y2): Difference in days between dates." << std::endl;
                } else if (topic == "bsm") {
                    std::cout << "Description: Black-Scholes-Merton (BSM) Builtin Module." << std::endl;
                    std::cout << "Inherits: all 'fin' methods (ticker, bs_price, greeks, etc.)" << std::endl;
                    std::cout << "Functions:" << std::endl;
                    std::cout << "  - price_american(symbol, type, duration): PDE-enhanced American pricing." << std::endl;
                    std::cout << "    - duration can be 'weekly', 'monthly', or 'quarterly' (default)." << std::endl;
                } else if (topic == "img") {
                    std::cout << "Description: Image processing via ImageMagick." << std::endl;
                    std::cout << "Functions:" << std::endl;
                    std::cout << "  - resize(src_path, width, height, dest_path): Resizes an image." << std::endl;
                } else if (topic == "container") {
                    std::cout << "Description: Docker and Kubernetes (K3s) management." << std::endl;
                    std::cout << "Functions:" << std::endl;
                    std::cout << "  - docker_run(image, cmd): Runs a Docker container (--rm)." << std::endl;
                    std::cout << "  - docker_ps(): Lists running containers." << std::endl;
                    std::cout << "  - docker_build(path, tag): Builds a Docker image." << std::endl;
                    std::cout << "  - docker_stop(id): Stops a Docker container." << std::endl;
                    std::cout << "  - k3s_apply(yaml): Applies a Kubernetes manifest (inline string)." << std::endl;
                    std::cout << "  - k3s_get(resource): Gets Kubernetes resources (e.g., 'pods', 'svc')." << std::endl;
                    std::cout << "  - k3s_pods(): Lists all pods across all namespaces." << std::endl;
                    std::cout << "  - k3s_nodes(): Lists cluster nodes." << std::endl;
                    std::cout << "  - k3s_logs(pod): Returns logs for a specified pod." << std::endl;
                } else if (topic == "vm") {
                    std::cout << "Description: Vagrant Virtual Machine management." << std::endl;
                    std::cout << "Functions:" << std::endl;
                    std::cout << "  - init(box): Initializes a Vagrant environment with the specified box." << std::endl;
                    std::cout << "  - up(): Starts the VM (vagrant up)." << std::endl;
                    std::cout << "  - ssh(cmd): Runs a command via SSH on the VM." << std::endl;
                    std::cout << "  - status(): Checks VM status." << std::endl;
                    std::cout << "  - halt(): Powers down the VM." << std::endl;
                    std::cout << "  - destroy(): Destroys the VM environment." << std::endl;
                } else if (topic == "go" || topic == "ruby" || topic == "node" || topic == "rust") {
                    std::cout << "Description: Foreign language interop and execution (" << topic << ")." << std::endl;
                    std::cout << "Functions:" << std::endl;
                    std::cout << "  - run(input): Runs a source file or inline code string." << std::endl;
                    if (topic == "go") std::cout << "  - build(file): Builds a Go binary." << std::endl;
                    if (topic == "node") std::cout << "  - npm_install(package): Installs an NPM package." << std::endl;
                    if (topic == "rust") std::cout << "  - cargo_new(name): Creates a new Cargo project." << std::endl;
                } else if (topic == "data") {
                    std::cout << "Description: Data science, statistics, and CSV processing." << std::endl;
                    std::cout << "Functions:" << std::endl;
                    std::cout << "  - read_csv(path): Reads CSV into a vector of GemObjects (rows)." << std::endl;
                    std::cout << "  - mean(vector): Calculates arithmetic mean of a numeric vector." << std::endl;
                    std::cout << "  - std(vector): Calculates sample standard deviation." << std::endl;
                } else if (topic == "sys") {
                    std::cout << "Description: System root module (inherited by all objects)." << std::endl;
                    std::cout << "Functions:" << std::endl;
                    std::cout << "  - print(...): Prints arguments to stdout." << std::endl;
                    std::cout << "  - args(): Returns command-line arguments as a vector." << std::endl;
                    std::cout << "  - async(func, [params], [timeout]): Executes function in a background thread." << std::endl;
                    std::cout << "  - sethandler(signals, func): Sets custom signal handler." << std::endl;
                    std::cout << "  - host(): Returns OS information." << std::endl;
                    std::cout << "  - exec(cmd): Executes a shell command." << std::endl;
                    std::cout << "  - doc([path]): Reads file comments or current script comments." << std::endl;
                    std::cout << "  - help([topic]): Displays this interactive help." << std::endl;
                    std::cout << "  - exit(): Exits the REPL or script." << std::endl;
                    std::cout << "  - langport(pattern, [output]): Ports foreign code (e.g. *.py) to Gem." << std::endl;
                    std::cout << "  - redirect(url, [port]): HTTP redirect or background redirect server." << std::endl;
                    std::cout << "  - app(port, [routes]): Background web app server. Routes map paths to strings, files, or handler fns." << std::endl;
                } else if (topic == "drvr") {
                    std::cout << "Description: Device driver development module (Linux, Windows 11, macOS, Android)." << std::endl;
                    std::cout << "Functions:" << std::endl;
                    std::cout << "  - linux(name): Generates a Linux kernel module template." << std::endl;
                    std::cout << "  - win11(name): Generates a Windows Driver Framework (WDF) template." << std::endl;
                    std::cout << "  - macos(name): Generates a macOS IOKit/System Extension template." << std::endl;
                    std::cout << "  - android(name): Generates an Android HAL template." << std::endl;
                    std::cout << "  - build(target): Triggers cross-platform build for specified target." << std::endl;
                    std::cout << "  - deploy(target): Deploys driver to the target device." << std::endl;
                } else if (topic == "math") {
                    std::cout << "Description: Mathematical constants, functions, and symbolic math." << std::endl;
                    std::cout << "Functions:" << std::endl;
                    std::cout << "  - sin(x), cos(x), sqrt(x): Standard math functions." << std::endl;
                    std::cout << "  - useSymPy(), useSage(): Switch symbolic provider (default: SymPy)." << std::endl;
                    std::cout << "  - simplify(expr): Symbolic simplification via SymPy/Sage." << std::endl;
                    std::cout << "  - diff(expr, [var]): Symbolic differentiation." << std::endl;
                    std::cout << "  - integrate(expr, [var]): Symbolic integration." << std::endl;
                    std::cout << "  - solve(expr, [var]): Solve an algebraic equation." << std::endl;
                    std::cout << "  - sym_latex(expr): Returns LaTeX for a symbolic expression." << std::endl;
                    std::cout << "  - to_latex(val): Converts value or matrix to LaTeX string." << std::endl;
                    std::cout << "  - write_latex(path, content, [standalone]): Writes LaTeX to file." << std::endl;
                    std::cout << "  - read_latex(path): Reads content from LaTeX document environment." << std::endl;
                    std::cout << "  - parse_latex(text): Extracts math expressions from text ($...$)." << std::endl;
                    std::cout << "  - compile_latex(path): Runs pdflatex on a file." << std::endl;
                    std::cout << "Constants:" << std::endl;
                    std::cout << "  - math.pi (3.14159...)" << std::endl;
                } else if (topic == "tcp") {
                    std::cout << "Description: TCP/IP Networking module." << std::endl;
                    std::cout << "Functions:" << std::endl;
                    std::cout << "  - listen(port): Starts a TCP server socket." << std::endl;
                    std::cout << "  - accept(fd): Waits for a client connection (returns Socket object)." << std::endl;
                    std::cout << "  - connect(host, port): Connects to a server (returns Socket object)." << std::endl;
                    std::cout << "  - send(fd, msg): Sends string message over socket." << std::endl;
                    std::cout << "  - recv(fd, [size]): Receives data from socket." << std::endl;
                    std::cout << "  - close(fd): Closes a socket descriptor." << std::endl;
                    std::cout << "  - nic(): Lists network interface configurations (returns vector of NIC objects)." << std::endl;
                    std::cout << "  - routes(): Lists system routing table info (returns vector of Route objects)." << std::endl;
                } else if (topic == "ai") {
                    std::cout << "Description: AI integration (Gemini, Mistral, Ollama)." << std::endl;
                    std::cout << "Functions:" << std::endl;
                    std::cout << "  - prompt(text): Sends a prompt to the current AI provider." << std::endl;
                    std::cout << "  - prompt_native(text): High-speed native Mistral C++ bridge call." << std::endl;
                    std::cout << "  - useMistral(model): Sets Mistral AI as the provider." << std::endl;
                    std::cout << "  - useOllama(model, [host]): Sets Ollama (local AI) as provider." << std::endl;
                    std::cout << "  - useGemini(): Switches back to Gemini as the provider." << std::endl;
                    std::cout << "  - setKey(key), setHost(host), setPath(path): API configuration." << std::endl;
                    std::cout << "Properties: provider, model, host" << std::endl;
                } else if (topic == "text") {
                    std::cout << "Description: String manipulation and PDF processing." << std::endl;
                    std::cout << "Functions:" << std::endl;
                    std::cout << "  - read(path): Reads entire file into a string." << std::endl;
                    std::cout << "  - sub(s, start, [len]): Returns a substring." << std::endl;
                    std::cout << "  - replace(s, old, new): Replaces all occurrences in a string." << std::endl;
                    std::cout << "  - write_pdf(path, text): Creates a simple PDF file." << std::endl;
                    std::cout << "  - write_pdf_a(path, text): Creates a PDF/A compliant file (Archival)." << std::endl;
                    std::cout << "  - read_pdf(path): Extracts text from a PDF file." << std::endl;
                    std::cout << "  - read_markdown(path), write_markdown(path, text): Markdown support." << std::endl;
                    std::cout << "  - read_yaml(path), write_yaml(path, obj): YAML support (Stubs)." << std::endl;
                    std::cout << "  - read_html(path), write_html(path, text): HTML support." << std::endl;
                    std::cout << "  - read_xml(path), write_xml(path, text): XML support." << std::endl;
                    std::cout << "  - read_fits_header(path): Returns FITS header as an object." << std::endl;
                    std::cout << "  - read_hdf_header(path): Returns HDF header as an object." << std::endl;
                } else if (topic == "algo") {
                    std::cout << "Description: Algorithms, sorting, and time." << std::endl;
                    std::cout << "Functions:" << std::endl;
                    std::cout << "  - add(...): Sums all numeric arguments." << std::endl;
                    std::cout << "  - quicksort(v), sort(v, [start], [end]): Sorts numeric vectors." << std::endl;
                    std::cout << "  - now(): Returns current local time as a string." << std::endl;
                    std::cout << "  - date_add(ts, days): Adds days to a unix timestamp." << std::endl;
                    std::cout << "  - date_diff(t1, t2): Difference in days between timestamps." << std::endl;
                } else if (topic == "nlp") {
                    std::cout << "Description: Natural Language Processing (Stub)." << std::endl;
                } else if (topic == "bev") {
                    std::cout << "Description: Bevington Data Reduction (Legacy Scientific Analysis)." << std::endl;
                    std::cout << "Functions:" << std::endl;
                    std::cout << "  - data(x_vec, y_vec): Loads data into Bevington buffers." << std::endl;
                    std::cout << "  - fit_line(): Performs a linear regression." << std::endl;
                    std::cout << "  - param(idx): Returns fitted parameter (0=intercept, 1=slope)." << std::endl;
                } else if (topic == "file") {
                    std::cout << "Description: File system operations." << std::endl;
                    std::cout << "Functions:" << std::endl;
                    std::cout << "  - write(path, content): Writes string to file." << std::endl;
                    std::cout << "  - exists(path): Returns true if file or directory exists." << std::endl;
                } else if (topic == "geo") {
                    std::cout << "Description: GIS, Geolocation, and GeoJSON." << std::endl;
                    std::cout << "Functions:" << std::endl;
                    std::cout << "  - lookup(): Returns current IP-based geolocation object (.lat, .lon, .city, .country)." << std::endl;
                    std::cout << "  - distance(lat1, lon1, lat2, lon2): Haversine distance in km." << std::endl;
                    std::cout << "  - write_geojson(path, features): Writes a GeoJSON FeatureCollection file." << std::endl;
                    std::cout << "  - history(plate): Returns geological history of a tectonic plate (AI assisted)." << std::endl;
                    std::cout << "  - plot2d(data, [layout]): Generates a 2D Plotly map (open-street-map / scattermapbox)." << std::endl;
                    std::cout << "  - plot3d(data, [layout]): Generates a 3D globe visualization (orthographic scattergeo)." << std::endl;
                } else if (topic == "mobl") {
                    std::cout << "Description: Mobile & cross-platform cell phone object (mobl.gm)." << std::endl;
                    std::cout << "Cross-platform: Android Chrome, iPhone Safari, macOS, Linux, Windows 11." << std::endl;
                    std::cout << "Usage: use \"mobl.gm\"  then  mobl phone(\"device_name\")" << std::endl;
                    std::cout << "Functions:" << std::endl;
                    std::cout << "  - dictate(spoken_text): NLP parse via ai.prompt() → JSON {title, note, tags}." << std::endl;
                    std::cout << "  - make_feature(lat, lon, spoken_text): Builds a GeoJSON Feature from GPS + dictation." << std::endl;
                    std::cout << "Architecture:" << std::endl;
                    std::cout << "  Browser supplies GPS (Geolocation API) + speech (Web Speech API)." << std::endl;
                    std::cout << "  Gem server handles NLP, GeoJSON assembly, and file persistence." << std::endl;
                    std::cout << "Reference app: ./gem travel_log.g  →  http://localhost:8080" << std::endl;
                    std::cout << "  /       → travel_log.html  (PWA: mic + live Plotly map)" << std::endl;
                    std::cout << "  /log    → POST {lat,lon,text} → returns GeoJSON feature" << std::endl;
                    std::cout << "  /data   → GET full GeoJSON FeatureCollection" << std::endl;
                } else if (topic == "www") {
                    std::cout << "Description: Web framework and mapping services (Flask-like)." << std::endl;
                    std::cout << "Functions:" << std::endl;
                    std::cout << "  - wget(url, file): Downloads a file from the internet." << std::endl;
                    std::cout << "  - app(port, [routes]): Starts a web server. Routes is a map of path->content." << std::endl;
                    std::cout << "  - redirect(target, [port]): Returns a redirect header or starts a redirect server." << std::endl;
                    std::cout << "  - map2d(geojson, output): Renders a 2D map from GeoJSON via Mapnik." << std::endl;
                } else if (topic == "cpp") {
                    std::cout << "Description: C++ Interop and JIT execution via Cling." << std::endl;
                    std::cout << "Functions:" << std::endl;
                    std::cout << "  - repl(): Starts an interactive C++26 REPL." << std::endl;
                    std::cout << "  - exec(code): Executes C++ code string directly." << std::endl;
                } else if (topic == "itr") {
                    std::cout << "Description: Iterators, Ranges, and Functional Loops." << std::endl;
                    std::cout << "Functions:" << std::endl;
                    std::cout << "  - range(n): Returns a vector [0, 1, ..., n-1]." << std::endl;
                    std::cout << "  - while(cond_fun, body_fun): Executes body function while condition is true." << std::endl;
                } else if (topic == "zip") {
                    std::cout << "Description: Native file compression and decompression (miniz)." << std::endl;
                    std::cout << "Functions:" << std::endl;
                    std::cout << "  - compress(src, archive): Recursively zips files/folders." << std::endl;
                    std::cout << "  - decompress(archive, dest): Unzips archive to destination." << std::endl;
                } else if (topic == "chart") {
                    std::cout << "Description: Interactive 2D/3D Plotting (Plotly.js)." << std::endl;
                    std::cout << "Functions:" << std::endl;
                    std::cout << "  - plot(traces, [layout]): Generates an interactive HTML chart." << std::endl;
                    std::cout << "  - show(path): Opens the chart HTML in a browser." << std::endl;
                    std::cout << "  - server(path, [port]): Starts a background server to serve the chart." << std::endl;
                } else if (topic == "fun") {
                    std::cout << "Keyword: fun" << std::endl;
                    std::cout << "Usage: fun name(p1, p2) ... end [return_expr]" << std::endl;
                    std::cout << "Description: Defines a first-class function. Supports recursion and closures." << std::endl;
                } else if (topic == "obj") {
                    std::cout << "Keyword: obj" << std::endl;
                    std::cout << "Usage: obj Name(params) : Parent ... end" << std::endl;
                    std::cout << "Description: Defines a new object/class with single inheritance." << std::endl;
                    std::cout << "Syntax: .attr accesses own attribute, ..attr accesses parent attribute." << std::endl;
                    std::cout << "Tip: All objects automatically inherit from 'sys'." << std::endl;
                } else if (topic == "use") {
                    std::cout << "Keyword: use" << std::endl;
                    std::cout << "Usage: use \"filename.g\" OR use \"filename.py\"" << std::endl;
                    std::cout << "Description: Includes a Gem file or translates and runs foreign code (Python, Julia, C++)." << std::endl;
                } else if (topic == "alias") {
                    std::cout << "Keyword: alias" << std::endl;
                    std::cout << "Usage: alias shortcut = expansion" << std::endl;
                    std::cout << "Description: Creates a REPL shortcut (e.g., alias ? = sys.help())." << std::endl;
                } else if (topic == "his") {
                    std::cout << "Keyword: his" << std::endl;
                    std::cout << "Description: Displays today's session history and prompts for prior days." << std::endl;
                } else if (topic == "lib") {
                    std::cout << "Keyword: lib" << std::endl;
                    std::cout << "Description: Lists all currently loaded builtin modules." << std::endl;
                } else if (topic == "if") {
                    std::cout << "Keyword: if" << std::endl;
                    std::cout << "Usage: if condition ... end" << std::endl;
                    std::cout << "Description: Conditional execution. Condition evaluates to true if non-zero or true." << std::endl;
                } else if (topic == "while") {
                    std::cout << "Keyword: while" << std::endl;
                    std::cout << "Usage: while condition ... end" << std::endl;
                    std::cout << "Description: Standard while loop." << std::endl;
                } else if (topic == "int" || topic == "double" || topic == "string" || topic == "bool") {
                    std::cout << "Keyword: " << topic << " (Type Declaration)" << std::endl;
                    std::cout << "Usage: " << topic << " var = value" << std::endl;
                    std::cout << "Description: Declares and initializes a variable of the specified type." << std::endl;
                } else if (topic == "exit") {
                    std::cout << "Keyword: exit" << std::endl;
                    std::cout << "Description: Exits the Gem interpreter." << std::endl;
                } else if (topic == "astro") {
                    std::cout << "Description: Astrophysics, Solar Physics, and Planetary Science module." << std::endl;
                    std::cout << "Constants: G, c, AU, pc, ly, Msun, Rsun, Lsun, Tsun, Mearth, Rearth, H0, sigma_sb" << std::endl;
                    std::cout << "Unit conversions: to_ly(pc), to_pc(ly), to_au(m), deg_to_rad(d), rad_to_deg(r)" << std::endl;
                    std::cout << "Stellar: luminosity(R_rsun,T), stefan_boltzmann(T,R), wien(T), abs_magnitude(m,d_pc)" << std::endl;
                    std::cout << "         distance_modulus(d_pc), spectral_class(T), schwarzschild_radius(M)" << std::endl;
                    std::cout << "         escape_velocity(M,R)" << std::endl;
                    std::cout << "Orbital: orbital_period(a_au,M_msun), orbital_velocity(a_m,M_kg)" << std::endl;
                    std::cout << "         hill_sphere(a,mp,ms), roche_limit(R,rho_p,rho_s), synodic_period(p1,p2)" << std::endl;
                    std::cout << "         planet(name) → Mercury/Venus/Earth/Mars/Jupiter/Saturn/Uranus/Neptune" << std::endl;
                    std::cout << "Solar:   solar_flux(dist_au), solar_wind_pressure(n_cm3,v_km_s)" << std::endl;
                    std::cout << "         sunspot_cycle(year), parker_spiral_angle(v_sw,dist_au), solar_activity()" << std::endl;
                    std::cout << "Cosmology: hubble_distance(z), redshift_velocity(z), lookback_time(z), critical_density()" << std::endl;
                    std::cout << "Coords:  equatorial_to_galactic(ra,dec), angular_separation(ra1,dec1,ra2,dec2)" << std::endl;
                    std::cout << "Exoplanet: transit_depth(rp_rearth,rs_rsun), habitable_zone(L_lsun), equilibrium_temp(L,d,A)" << std::endl;
                    std::cout << "Pulsar:  pulsar_spindown(P_s, Pdot) → {age_yr, Bfield_G, edot_W}" << std::endl;
                } else {
                    std::cout << "No detailed help available for " << topic << std::endl;
                }
            }
            return std::make_shared<GemValue>();
        }, true };

        methods["doc"] = { [this](std::vector<std::shared_ptr<GemValue>> args) {
            if (!args.empty() && std::holds_alternative<std::string>(args[0]->value)) {
                std::string path = std::get<std::string>(args[0]->value);
                std::ifstream t(path);
                if (!t.good()) return std::make_shared<GemValue>("");
                std::stringstream buffer;
                buffer << t.rdbuf();
                return std::make_shared<GemValue>(buffer.str());
            }

            std::string currentFile = "";
            auto currentFileVal = get("__file__");
            if (std::holds_alternative<std::string>(currentFileVal->value)) {
                currentFile = std::get<std::string>(currentFileVal->value);
            }

            if (currentFile.empty()) return std::make_shared<GemValue>("");

            std::ifstream file(currentFile);
            std::string comments = "";
            if (file.is_open()) {
                std::string line;
                while (std::getline(file, line)) {
                    size_t pos = line.find('#');
                    if (pos != std::string::npos) {
                        comments += line.substr(pos) + "\n";
                    }
                }
            }
            return std::make_shared<GemValue>(comments);
        }, true };
    }
};

inline GemObject::GemObject(const std::string& n, std::shared_ptr<GemObject> p) : name(n), parent(p) {
    if (!parent && name != "sys" && sysInstance) {
        parent = sysInstance;
    }
}

// Math Object
class GemMath : public GemSys {
public:
    std::string symbolicProvider = "sympy";

    GemMath() : GemSys() {
        name = "math";
        methods["sin"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty() || !std::holds_alternative<double>(args[0]->value)) return std::make_shared<GemValue>(0.0);
            return std::make_shared<GemValue>(std::sin(std::get<double>(args[0]->value)));
        }, true };
        methods["cos"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty() || !std::holds_alternative<double>(args[0]->value)) return std::make_shared<GemValue>(0.0);
            return std::make_shared<GemValue>(std::cos(std::get<double>(args[0]->value)));
        }, true };
        methods["sqrt"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty() || !std::holds_alternative<double>(args[0]->value)) return std::make_shared<GemValue>(0.0);
            return std::make_shared<GemValue>(std::sqrt(std::get<double>(args[0]->value)));
        }, true };
        properties["pi"] = std::make_shared<GemValue>(3.14159265358979323846);
    }
};

// AI Object
class GemAI : public GemSys {
public:
    static bool pythonInitialized;
    std::string provider = "gemini";
    std::string model = "gemini-1.5-flash";
    std::string host = "generativelanguage.googleapis.com";
    std::string path = "/v1beta/models/";

    GemAI() : GemSys() {
        name = "ai";
        properties["provider"] = std::make_shared<GemValue>(provider);
        properties["model"] = std::make_shared<GemValue>(model);
        properties["host"] = std::make_shared<GemValue>(host);

        methods["setKey"] = { [this](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty()) return std::make_shared<GemValue>(false);
            set("__key__", args[0]);
            return std::make_shared<GemValue>(true);
        }, true };

        methods["setHost"] = { [this](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty()) return std::make_shared<GemValue>(false);
            host = args[0]->toString();
            properties["host"] = std::make_shared<GemValue>(host);
            return std::make_shared<GemValue>(true);
        }, true };

        methods["setPath"] = { [this](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty()) return std::make_shared<GemValue>(false);
            path = args[0]->toString();
            return std::make_shared<GemValue>(true);
        }, true };

        methods["useMistral"] = { [this](std::vector<std::shared_ptr<GemValue>> args) {
            provider = "mistral";
            model = args.empty() ? "mistral-tiny" : args[0]->toString();
            host = "api.mistral.ai";
            path = "/v1/";
            properties["provider"] = std::make_shared<GemValue>(provider);
            properties["model"] = std::make_shared<GemValue>(model);
            properties["host"] = std::make_shared<GemValue>(host);
            return std::make_shared<GemValue>(true);
        }, true };

        methods["useOllama"] = { [this](std::vector<std::shared_ptr<GemValue>> args) {
            provider = "ollama";
            model = args.empty() ? "mistral" : args[0]->toString();
            host = args.size() > 1 ? args[1]->toString() : "localhost:11434";
            path = "/api/generate";
            properties["provider"] = std::make_shared<GemValue>(provider);
            properties["model"] = std::make_shared<GemValue>(model);
            properties["host"] = std::make_shared<GemValue>(host);
            return std::make_shared<GemValue>(true);
        }, true };

        methods["prompt_native"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
#ifdef HAS_MISTRAL
            if (args.empty()) return std::make_shared<GemValue>("");
            std::string p = args[0]->toString();
            char* response = call_mistral_bridge((char*)p.c_str());
            if (response == NULL) return std::make_shared<GemValue>("Error: Mistral bridge failed");
            std::string res_str(response);
            free(response);
            return std::make_shared<GemValue>(res_str);
#else
            return std::make_shared<GemValue>("Mistral bridge not enabled");
#endif
        }, true };

        methods["prompt"] = { [this](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty()) return std::make_shared<GemValue>("");
            std::string p = args[0]->toString();
            
            std::string key = "";
            auto keyVal = get("__key__");
            if (std::holds_alternative<std::string>(keyVal->value)) key = std::get<std::string>(keyVal->value);

            if (provider == "ollama") {
                std::string payload = "{\"model\": \"" + model + "\", \"prompt\": \"" + p + "\", \"stream\": false}";
                std::string cmd = "curl -s -X POST http://" + host + path + " -d '" + payload + "'";
                FILE* pipe = popen(cmd.c_str(), "r");
                if (!pipe) return std::make_shared<GemValue>("Error");
                char buffer[128];
                std::string result = "";
                while (fgets(buffer, 128, pipe)) result += buffer;
                pclose(pipe);
                // Parse "response" from JSON stub
                size_t start = result.find("\"response\":\"");
                if (start != std::string::npos) {
                    size_t end = result.find("\"", start + 12);
                    return std::make_shared<GemValue>(result.substr(start + 12, end - (start + 12)));
                }
                return std::make_shared<GemValue>(result);
            }
            
            // Mock response for other providers if key is missing
            if (key.empty()) return std::make_shared<GemValue>("AI provider " + provider + " configured. Set API key to use.");
            
            return std::make_shared<GemValue>("[AI Result Placeholder]");
        }, true };
    }
};

// TCP Object
class GemTcp : public GemSys {
public:
    GemTcp() : GemSys() {
        name = "tcp";
        methods["listen"] = { [](std::vector<std::shared_ptr<GemValue>> args) { return std::make_shared<GemValue>(0.0); }, true };
        methods["connect"] = { [](std::vector<std::shared_ptr<GemValue>> args) { return std::make_shared<GemValue>(0.0); }, true };
        
        methods["nic"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            std::vector<std::shared_ptr<GemObject>> nics;
            auto n1 = std::make_shared<GemObject>("NIC");
            n1->set("name", std::make_shared<GemValue>("lo0"));
            n1->set("ip", std::make_shared<GemValue>("127.0.0.1"));
            n1->set("status", std::make_shared<GemValue>("up"));
            nics.push_back(n1);
            return std::make_shared<GemValue>(nics);
        }, true };

        methods["routes"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            std::vector<std::shared_ptr<GemObject>> routes;
            auto r1 = std::make_shared<GemObject>("Route");
            r1->set("dest", std::make_shared<GemValue>("default"));
            r1->set("gw", std::make_shared<GemValue>("192.168.1.1"));
            r1->set("iface", std::make_shared<GemValue>("en0"));
            routes.push_back(r1);
            return std::make_shared<GemValue>(routes);
        }, true };
    }
};

// Iterator/Range Object
class GemItr : public GemSys {
public:
    GemItr() : GemSys() {
        name = "itr";
        methods["range"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty() || !std::holds_alternative<double>(args[0]->value)) return std::make_shared<GemValue>();
            int end = (int)std::get<double>(args[0]->value);
            std::vector<double> v;
            for(int i=0; i<end; ++i) v.push_back((double)i);
            return std::make_shared<GemValue>(v);
        }, true };

        // itr.while(condition_func_name, body_func_name)
        methods["while"] = { [this](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.size() < 2 || !this->callGemFunc) return std::make_shared<GemValue>(false);
            
            std::string condFunc = args[0]->toString();
            std::string bodyFunc = args[1]->toString();

            int safety = 0;
            while (safety < 100000) {
                auto condVal = this->callGemFunc(condFunc, {});
                bool cond = false;
                if (std::holds_alternative<bool>(condVal->value)) cond = std::get<bool>(condVal->value);
                else if (std::holds_alternative<double>(condVal->value)) cond = std::get<double>(condVal->value) != 0;

                if (!cond) break;

                this->callGemFunc(bodyFunc, {});
                safety++;
            }
            return std::make_shared<GemValue>(true);
        }, true };
    }
};



// Text Object
class GemText : public GemSys {
public:
    GemText() : GemSys() {
        name = "text";
        methods["read"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty() || !std::holds_alternative<std::string>(args[0]->value)) return std::make_shared<GemValue>();
            std::ifstream t(std::get<std::string>(args[0]->value));
            if (!t.good()) return std::make_shared<GemValue>("");
            std::stringstream buffer; buffer << t.rdbuf(); return std::make_shared<GemValue>(buffer.str());
        }, true };

        methods["read_pdf"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
#ifdef HAS_PDFWRITER
            if (args.empty()) return std::make_shared<GemValue>("");
            return std::make_shared<GemValue>("[PDF Text Extraction Stub]");
#else
            return std::make_shared<GemValue>("PDF support disabled");
#endif
        }, true };

        methods["write_pdf"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
#ifdef HAS_PDFWRITER
            if (args.size() < 2) return std::make_shared<GemValue>(false);
            std::string path = args[0]->toString();
            std::string content = args[1]->toString();
            PDFWriter pdf;
            pdf.StartPDF(path, ePDFVersion13);
            auto page = new PDFPage();
            page->SetMediaBox(PDFRectangle(0, 0, 595, 842));
            auto ctx = pdf.StartPageContentContext(page);
            auto font = pdf.GetFontForFile("/Library/Fonts/Arial.ttf");
            if (font) {
                ctx->WriteText(50, 800, content, font, 12);
            }
            pdf.EndPageContentContext(ctx);
            pdf.WritePageAndRelease(page);
            pdf.EndPDF();
            return std::make_shared<GemValue>(true);
#else
            return std::make_shared<GemValue>(false);
#endif
        }, true };

        methods["sub"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty()) return std::make_shared<GemValue>("");
            std::string s = args[0]->toString();
            int start = (args.size() > 1) ? (int)std::get<double>(args[1]->value) : 0;
            int len = (args.size() > 2) ? (int)std::get<double>(args[2]->value) : (int)s.length();
            return std::make_shared<GemValue>(s.substr(start, len));
        }, true };

        methods["replace"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.size() < 3) return std::make_shared<GemValue>("");
            std::string s = args[0]->toString();
            std::string oldS = args[1]->toString();
            std::string newS = args[2]->toString();
            size_t pos = 0;
            while ((pos = s.find(oldS, pos)) != std::string::npos) {
                s.replace(pos, oldS.length(), newS);
                pos += newS.length();
            }
            return std::make_shared<GemValue>(s);
        }, true };
    }
};

// Algorithms Object
class GemAlgo : public GemSys {
public:
    GemAlgo(const std::string& n = "algo") : GemSys() {
        name = n;
        methods["add"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            double sum = 0;
            for(auto& arg : args) if (std::holds_alternative<double>(arg->value)) sum += std::get<double>(arg->value);
            return std::make_shared<GemValue>(sum);
        }, true };
        methods["now"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            auto t = std::time(nullptr);
            auto tm = *std::localtime(&t);
            std::ostringstream oss;
            oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
            return std::make_shared<GemValue>(oss.str());
        }, true };
    }
};

// Zip Object
class GemZip : public GemSys {
public:
    GemZip() : GemSys() {
        name = "zip";
        methods["compress"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.size() < 2) return std::make_shared<GemValue>(false);
            return std::make_shared<GemValue>(true); // Stub
        }, true };
    }
};

// NLP Object
class GemNLP : public GemSys {
public:
    GemNLP() : GemSys() { name = "nlp"; }
};

// Bevington Object
class GemBev : public GemAlgo {
public:
    GemBev() : GemAlgo("bev") {
        methods["data"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
             if (args.size() < 2) return std::make_shared<GemValue>(false);
             std::vector<double> x = std::get<std::vector<double>>(args[0]->value);
             std::vector<double> y = std::get<std::vector<double>>(args[1]->value);
             PutnPts((int)x.size());
             for(size_t i=0; i<x.size(); ++i) PutXYdY((int)i+1, x[i], y[i], 1.0);
             return std::make_shared<GemValue>(true);
        }, true };
        methods["fit_line"] = { [](std::vector<std::shared_ptr<GemValue>> args) { return std::make_shared<GemValue>(LineFit()); }, true };
        methods["param"] = { [](std::vector<std::shared_ptr<GemValue>> args) { return std::make_shared<GemValue>(GetParam((int)std::get<double>(args[0]->value) + 1)); }, true };
    }
};

class GemFile : public GemSys {
public:
    GemFile() : GemSys() {
        name = "file";
        methods["write"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.size() < 2) return std::make_shared<GemValue>(false);
            std::ofstream out(args[0]->toString());
            out << args[1]->toString();
            return std::make_shared<GemValue>(true);
        }, true };
        methods["exists"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty()) return std::make_shared<GemValue>(false);
            return std::make_shared<GemValue>(std::filesystem::exists(args[0]->toString()));
        }, true };
    }
};

class GemImg : public GemSys {
public:
    static bool initialized;
    GemImg() : GemSys() {
        name = "img";
#ifdef HAS_IMAGEMAGICK
        if (!initialized) { Magick::InitializeMagick(nullptr); initialized = true; }
#endif
        methods["resize"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
#ifdef HAS_IMAGEMAGICK
            try { Magick::Image img; img.read(args[0]->toString()); img.resize(Magick::Geometry((int)std::get<double>(args[1]->value), (int)std::get<double>(args[2]->value))); img.write(args[3]->toString()); return std::make_shared<GemValue>(true); } catch(...) { return std::make_shared<GemValue>(false); }
#else
            return std::make_shared<GemValue>(false);
#endif
        }, true };
    }
};

class GemWWW : public GemSys {
public:
    GemWWW() : GemSys() {
        name = "www";
        methods["wget"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.size() < 2) return std::make_shared<GemValue>(false);
            std::string cmd = "curl -L -s -o " + args[1]->toString() + " " + args[0]->toString();
            return std::make_shared<GemValue>(std::system(cmd.c_str()) == 0);
        }, true };
        methods["app"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            int port = (args.empty()) ? 8080 : (int)std::get<double>(args[0]->value);
            std::map<std::string, std::string> routes;
            if (args.size() > 1 && std::holds_alternative<std::map<std::string, std::shared_ptr<GemValue>>>(args[1]->value)) {
                auto rMap = std::get<std::map<std::string, std::shared_ptr<GemValue>>>(args[1]->value);
                for (auto const& [path, content] : rMap) {
                    routes[path] = content->toString();
                }
            } else {
                routes["/"] = "<h1>Gem Web App</h1><p>Welcome to the Gem Language performance web server.</p>";
            }

            std::thread([port, routes]() {
                httplib::Server svr;
                for (auto const& [path, content] : routes) {
                    svr.Get(path.c_str(), [content](const httplib::Request&, httplib::Response& res) {
                        res.set_content(content, "text/html");
                    });
                }
                std::cout << "Starting Gem Web App on port " << port << "..." << std::endl;
                svr.listen("0.0.0.0", port);
            }).detach();
            return std::make_shared<GemValue>(true);
        }, true };
        methods["redirect"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty()) return std::make_shared<GemValue>(false);
            std::string target = args[0]->toString();
            // If a port is given, start a background redirection server immediately
            if (args.size() > 1 && std::holds_alternative<double>(args[1]->value)) {
                int port = (int)std::get<double>(args[1]->value);
                std::thread([target, port]() {
                    httplib::Server svr;
                    svr.Get("/.*", [target](const httplib::Request&, httplib::Response& res) {
                        res.set_redirect(target, 302);
                    });
                    std::cout << "Redirect server running at http://localhost:" << port << " -> " << target << std::endl;
                    svr.listen("0.0.0.0", port);
                }).detach();
                return std::make_shared<GemValue>(true);
            }
            // Otherwise return a standard HTTP redirect header string (useful for www.app routes)
            return std::make_shared<GemValue>("HTTP/1.1 302 Found\r\nLocation: " + target + "\r\n\r\n");
        }, true };
        methods["map2d"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
#ifdef HAS_MAPNIK
            return std::make_shared<GemValue>(true); // Stub
#else
            return std::make_shared<GemValue>(false);
#endif
        }, true };
    }
};

// GemCDN Module (Caching Proxy)
class GemCDN : public GemWWW {
public:
    struct CacheEntry {
        std::string content;
        std::string contentType;
        std::time_t expiry;
    };
    // Cache map: Key -> CacheEntry
    std::map<std::string, CacheEntry> cache;
    std::mutex cacheMutex;
    
    // Stats
    std::atomic<long> hits{0};
    std::atomic<long> misses{0};
    std::atomic<long> bytesServed{0};
    
    // Config: Path -> Origin URL
    std::map<std::string, std::string> routes;

    GemCDN() : GemWWW() {
        name = "cdn";
        
        // cdn.start(port, routes_map)
        methods["start"] = { [this](std::vector<std::shared_ptr<GemValue>> args) {
            int port = 8080;
            if (!args.empty()) {
                if (std::holds_alternative<double>(args[0]->value)) port = (int)std::get<double>(args[0]->value);
                else if (std::holds_alternative<std::string>(args[0]->value)) {
                    try { port = std::stoi(std::get<std::string>(args[0]->value)); } catch(...) {}
                }
            }
            
            if (args.size() > 1 && std::holds_alternative<std::map<std::string, std::shared_ptr<GemValue>>>(args[1]->value)) {
                auto rMap = std::get<std::map<std::string, std::shared_ptr<GemValue>>>(args[1]->value);
                for (auto const& [path, target] : rMap) {
                    routes[path] = target->toString();
                }
            }
            
            std::thread([this, port]() {
                httplib::Server svr;
                
                // Handler for all requests
                svr.Get(".*", [this](const httplib::Request& req, httplib::Response& res) {
                    std::string path = req.path;
                    
                    // Check Cache
                    {
                        std::lock_guard<std::mutex> lock(cacheMutex);
                        if (cache.count(path)) {
                            auto& entry = cache[path];
                            if (entry.expiry > std::time(nullptr)) {
                                hits++;
                                bytesServed += entry.content.size();
                                res.set_content(entry.content, entry.contentType.c_str());
                                return;
                            } else {
                                cache.erase(path); // Expired
                            }
                        }
                    }
                    
                    misses++;
                    
                    // Route to Origin
                    std::string origin = "";
                    for (const auto& [route, target] : routes) {
                        if (path.find(route) == 0) {
                            origin = target;
                            break; // Simple prefix match
                        }
                    }
                    
                    if (origin.empty()) {
                        res.status = 404;
                        res.set_content("Not Found (No Route)", "text/plain");
                        return;
                    }
                    
                    // Simple URL parsing (very basic)
                    std::string originHost = origin;
                    int originPort = 80;
                    std::string originPath = path;
                    
                    if (origin.find("http://") == 0) {
                        originHost = origin.substr(7);
                    } else if (origin.find("https://") == 0) {
                         originHost = origin.substr(8);
                         originPort = 443; // Basic support, though httplib handles https via CPPHTTPLIB_OPENSSL_SUPPORT if enabled.
                         // For now assuming http for simplicity in this builtin or plain proxy.
                    }
                    
                    size_t portPos = originHost.find(':');
                    size_t pathPos = originHost.find('/');
                    
                    if (pathPos != std::string::npos) {
                        // Origin has a path component, might need to rewrite?
                        // For simplicity, just take host.
                         originHost = originHost.substr(0, pathPos);
                    }

                    if (portPos != std::string::npos) {
                        originPort = std::stoi(originHost.substr(portPos + 1));
                        originHost = originHost.substr(0, portPos);
                    }

                    httplib::Client cli(originHost, originPort);
                    // Forward headers? For now simple GET.
                    auto originRes = cli.Get(originPath.c_str());
                    
                    if (originRes && originRes->status == 200) {
                        std::string content = originRes->body;
                        std::string cType = originRes->get_header_value("Content-Type");
                        if (cType.empty()) cType = "text/html";
                        
                        // Cache it (Default TTL 60s)
                        {
                            std::lock_guard<std::mutex> lock(cacheMutex);
                            cache[path] = { content, cType, std::time(nullptr) + 60 };
                        }
                        
                        bytesServed += content.size();
                        res.set_content(content, cType.c_str());
                    } else {
                        res.status = originRes ? originRes->status : 502;
                        res.set_content("Bad Gateway", "text/plain");
                    }
                });

                std::cout << "Starting Gem CDN Server on port " << port << "..." << std::endl;
                svr.listen("0.0.0.0", port);
            }).detach();
            
            return std::make_shared<GemValue>(true);
        }, true };
        
        methods["stats"] = { [this](std::vector<std::shared_ptr<GemValue>> args) {
            auto obj = std::make_shared<GemObject>("CDNStats");
            obj->set("hits", std::make_shared<GemValue>((double)hits));
            obj->set("misses", std::make_shared<GemValue>((double)misses));
            obj->set("bytes", std::make_shared<GemValue>((double)bytesServed));
            obj->set("cached_items", std::make_shared<GemValue>((double)cache.size()));
            return std::make_shared<GemValue>(obj);
        }, true };

        methods["purge"] = { [this](std::vector<std::shared_ptr<GemValue>> args) {
             std::lock_guard<std::mutex> lock(cacheMutex);
             if (args.empty()) {
                 cache.clear();
             } else {
                 std::string pattern = args[0]->toString();
                 // Simple exact match or * clear
                 if (pattern == "*") cache.clear();
                 else cache.erase(pattern);
             }
             return std::make_shared<GemValue>(true);
        }, true };
        
        methods["config"] = { [this](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.size() < 2) return std::make_shared<GemValue>(false);
            std::string type = args[0]->toString(); // "nginx" or "apache"
            std::string content = args[1]->toString();
            
            std::stringstream ss(content);
            std::string line;
            while(std::getline(ss, line)) {
                // Very basic parser
                if (type == "nginx") {
                    // location /path { proxy_pass http://...; }
                    size_t pp = line.find("proxy_pass");
                    if (pp != std::string::npos) {
                        size_t start = line.find("http", pp);
                        size_t end = line.find(";", start);
                        if (start != std::string::npos && end != std::string::npos) {
                             // Assuming simple 1-to-1 mapping for now, strictly speaking need context
                             routes["/"] = line.substr(start, end - start);
                             std::cout << "CDN Config: Mapped / -> " << routes["/"] << std::endl;
                        }
                    }
                } else if (type == "apache") {
                    // ProxyPass /path http://...
                    size_t pp = line.find("ProxyPass");
                    if (pp != std::string::npos) {
                        std::stringstream lss(line);
                        std::string kw, path, url;
                        lss >> kw >> path >> url;
                        if (kw == "ProxyPass" && !url.empty()) {
                            routes[path] = url;
                            std::cout << "CDN Config: Mapped " << path << " -> " << url << std::endl;
                        }
                    }
                }
            }
            return std::make_shared<GemValue>(true);
        }, true };

        methods["dashboard"] = { [this](std::vector<std::shared_ptr<GemValue>> args) {
             int port = 8081;
             if (!args.empty()) {
                if (std::holds_alternative<double>(args[0]->value)) port = (int)std::get<double>(args[0]->value);
                else if (std::holds_alternative<std::string>(args[0]->value)) {
                    try { port = std::stoi(std::get<std::string>(args[0]->value)); } catch(...) {}
                }
             }
             std::thread([this, port]() {
                 httplib::Server svr;
                 
                 // API
                 svr.Get("/api/stats", [this](const httplib::Request&, httplib::Response& res) {
                     std::string json = "{";
                     json += "\"hits\": " + std::to_string(hits) + ",";
                     json += "\"misses\": " + std::to_string(misses) + ",";
                     json += "\"bytes\": " + std::to_string(bytesServed) + ",";
                     json += "\"cached\": " + std::to_string(cache.size());
                     json += "}";
                     res.set_content(json, "application/json");
                 });
                 
                 // SPA
                 svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
                     res.set_content(R"html(
<!DOCTYPE html>
<html>
<head>
    <title>Gem CDN Dashboard</title>
    <style>
        body { font-family: 'Segoe UI', sans-serif; background: #1a1a1a; color: #fff; margin: 0; padding: 20px; }
        .card { background: #2d2d2d; padding: 20px; border-radius: 8px; margin-bottom: 20px; }
        .stat-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 20px; }
        .stat-box { background: #3d3d3d; padding: 15px; border-radius: 6px; text-align: center; }
        .stat-val { font-size: 2em; font-weight: bold; color: #4CAF50; }
        .stat-label { color: #aaa; }
        button { background: #2196F3; color: white; border: none; padding: 10px 20px; border-radius: 4px; cursor: pointer; }
        textarea { width: 100%; height: 200px; background: #333; color: #fff; border: 1px solid #444; margin-top: 10px; }
    </style>
</head>
<body>
    <h1>Gem CDN Control Panel</h1>
    
    <div class="card">
        <h2>Real-time Traffic</h2>
        <div class="stat-grid">
            <div class="stat-box"><div id="hits" class="stat-val">0</div><div class="stat-label">Hits</div></div>
            <div class="stat-box"><div id="misses" class="stat-val">0</div><div class="stat-label">Misses</div></div>
            <div class="stat-box"><div id="bytes" class="stat-val">0</div><div class="stat-label">Bytes Served</div></div>
            <div class="stat-box"><div id="cached" class="stat-val">0</div><div class="stat-label">Cached Items</div></div>
        </div>
    </div>

    <div class="card">
        <h2>Configuration Import</h2>
        <select id="configType"><option value="nginx">Nginx</option><option value="apache">Apache</option></select>
        <textarea id="configText" placeholder="Paste config here..."></textarea>
        <br><br>
        <button onclick="applyConfig()">Apply Config</button>
    </div>

    <script>
        function updateStats() {
            fetch('/api/stats').then(r => r.json()).then(data => {
                document.getElementById('hits').innerText = data.hits;
                document.getElementById('misses').innerText = data.misses;
                document.getElementById('bytes').innerText = (data.bytes / 1024 / 1024).toFixed(2) + ' MB';
                document.getElementById('cached').innerText = data.cached;
            });
        }
        setInterval(updateStats, 2000);
        updateStats();

        function applyConfig() {
            // In a real app we'd POST to an endpoint that calls cdn.config()
            alert('Config application requires backend binding. Use cdn.config() in script.');
        }
    </script>
</body>
</html>
)html", "text/html");
                 });
                 
                 std::cout << "Starting CDN Dashboard on port " << port << "..." << std::endl;
                 svr.listen("0.0.0.0", port);
             }).detach();
             return std::make_shared<GemValue>(true);
        }, true };
    }
};


class GemGeo : public GemSys {
public:
    GemGeo() : GemSys() {
        name = "geo";
        methods["lookup"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            auto obj = std::make_shared<GemObject>("Location");
            obj->set("city", std::make_shared<GemValue>("Los Angeles"));
            obj->set("country", std::make_shared<GemValue>("USA"));
            obj->set("lat", std::make_shared<GemValue>(34.05));
            obj->set("lon", std::make_shared<GemValue>(-118.24));
            return std::make_shared<GemValue>(obj);
        }, true };
        methods["distance"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.size() < 4) return std::make_shared<GemValue>(0.0);
            double lat1 = std::get<double>(args[0]->value);
            double lon1 = std::get<double>(args[1]->value);
            double lat2 = std::get<double>(args[2]->value);
            double lon2 = std::get<double>(args[3]->value);
            double dlat = (lat2 - lat1) * M_PI / 180.0;
            double dlon = (lon2 - lon1) * M_PI / 180.0;
            double a = std::sin(dlat/2) * std::sin(dlat/2) +
                       std::cos(lat1 * M_PI / 180.0) * std::cos(lat2 * M_PI / 180.0) *
                       std::sin(dlon/2) * std::sin(dlon/2);
            return std::make_shared<GemValue>(12742 * std::atan2(std::sqrt(a), std::sqrt(1-a)));
        }, true };
        methods["write_geojson"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.size() < 2) return std::make_shared<GemValue>(false);
            std::string path = args[0]->toString();
            std::ofstream out(path);
            out << "{\"type\": \"FeatureCollection\", \"features\": []}";
            return std::make_shared<GemValue>(true);
        }, true };

        methods["history"] = { [this](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty() || !translateFunc) return std::make_shared<GemValue>("No history data available.");
            std::string plate = args[0]->toString();
            std::string prompt = "Provide a concise geological and tectonic history of the " + plate + " plate. Include major movements, collisions, and current status.";
            return std::make_shared<GemValue>(translateFunc(prompt, "English"));
        }, true };

        methods["plot2d"] = { [this](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty()) return std::make_shared<GemValue>("");
            static int geoPlotCount = 0;
            std::string path = "gem_geo2d_" + std::to_string(std::time(nullptr)) + "_" + std::to_string(++geoPlotCount) + ".html";
            std::ofstream out(path);
            out << "<html><head><script src='https://cdn.plot.ly/plotly-latest.min.js'></script></head><body><div id='gd' style='height:100vh;'></div><script>";
            out << "var data = " << serializeValue(args[0]) << ";\n";
            out << "var layout = { title: 'Gem 2D Geo Visualization', geo: { projection: { type: 'robinson' } } };\n";
            if (args.size() > 1) out << "Object.assign(layout, " << serializeValue(args[1]) << ");\n";
            out << "Plotly.newPlot('gd', data, layout);";
            out << "</script></body></html>";
            return std::make_shared<GemValue>(path);
        }, true };

        methods["plot3d"] = { [this](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty()) return std::make_shared<GemValue>("");
            static int geo3dPlotCount = 0;
            std::string path = "gem_geo3d_" + std::to_string(std::time(nullptr)) + "_" + std::to_string(++geo3dPlotCount) + ".html";
            std::ofstream out(path);
            out << "<html><head><script src='https://cdn.plot.ly/plotly-latest.min.js'></script></head><body><div id='gd' style='height:100vh;'></div><script>";
            out << "var data = " << serializeValue(args[0]) << ";\n";
            out << "var layout = { title: 'Gem 3D Globe Visualization', showlegend: false, geo: { projection: { type: 'orthographic' }, showland: true, showocean: true, oceancolor: 'rgb(0, 255, 255)', landcolor: 'rgb(230, 145, 56)', lakecolor: 'rgb(0, 255, 255)' } };\n";
            if (args.size() > 1) out << "Object.assign(layout, " << serializeValue(args[1]) << ");\n";
            out << "Plotly.newPlot('gd', data, layout);";
            out << "</script></body></html>";
            return std::make_shared<GemValue>(path);
        }, true };
    }

private:
    std::string serializeValue(std::shared_ptr<GemValue> val) {
        if (!val) return "null";
        if (std::holds_alternative<double>(val->value)) return std::to_string(std::get<double>(val->value));
        if (std::holds_alternative<std::string>(val->value)) return "'" + std::get<std::string>(val->value) + "'";
        if (std::holds_alternative<bool>(val->value)) return std::get<bool>(val->value) ? "true" : "false";
        if (std::holds_alternative<std::vector<double>>(val->value)) {
            const auto& v = std::get<std::vector<double>>(val->value);
            std::string s = "[";
            for (size_t i = 0; i < v.size(); ++i) {
                s += std::to_string(v[i]) + (i < v.size() - 1 ? "," : "");
            }
            return s + "]";
        }
        if (std::holds_alternative<std::vector<std::string>>(val->value)) {
            const auto& v = std::get<std::vector<std::string>>(val->value);
            std::string s = "[";
            for (size_t i = 0; i < v.size(); ++i) {
                s += "'" + v[i] + "'" + (i < v.size() - 1 ? "," : "");
            }
            return s + "]";
        }
        if (std::holds_alternative<std::vector<std::shared_ptr<GemObject>>>(val->value)) {
            const auto& v = std::get<std::vector<std::shared_ptr<GemObject>>>(val->value);
            std::string s = "[";
            for (size_t i = 0; i < v.size(); ++i) {
                s += serializeObject(v[i]) + (i < v.size() - 1 ? "," : "");
            }
            return s + "]";
        }
        if (std::holds_alternative<std::shared_ptr<GemObject>>(val->value)) return serializeObject(std::get<std::shared_ptr<GemObject>>(val->value));
        if (std::holds_alternative<std::map<std::string, std::shared_ptr<GemValue>>>(val->value)) return serializeMap(std::get<std::map<std::string, std::shared_ptr<GemValue>>>(val->value));
        return "null";
    }

    std::string serializeObject(std::shared_ptr<GemObject> obj) {
        if (!obj) return "{}";
        return serializeMap(obj->properties);
    }

    std::string serializeMap(const std::map<std::string, std::shared_ptr<GemValue>>& m) {
        std::string s = "{";
        for (auto it = m.begin(); it != m.end(); ++it) {
            s += "'" + it->first + "': " + serializeValue(it->second);
            if (std::next(it) != m.end()) s += ",";
        }
        return s + "}";
    }
};
class GemCpp : public GemSys { public: GemCpp() : GemSys() { name = "cpp"; methods["repl"] = { [](std::vector<std::shared_ptr<GemValue>> args) { return std::make_shared<GemValue>((double)std::system("cling -std=c++26")); }, true }; methods["exec"] = { [](std::vector<std::shared_ptr<GemValue>> args) { if (args.empty() || !std::holds_alternative<std::string>(args[0]->value)) return std::make_shared<GemValue>(-1.0); std::string code = std::get<std::string>(args[0]->value); std::string cmd = "cling -std=c++26 -execute \"" + code + "\""; return std::make_shared<GemValue>((double)std::system(cmd.c_str())); }, true }; } };

// Charting Object (Plotly.js based)
class GemChart : public GemSys {
public:
    GemChart() : GemSys() {
        name = "chart";
        
        // chart.plot(traces, layout)
        // traces is a vector of objects
        // layout is an object or map
        methods["plot"] = { [this](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty()) return std::make_shared<GemValue>("");
            
            static int plotCount = 0;
            std::string path = "gem_chart_" + std::to_string(std::time(nullptr)) + "_" + std::to_string(++plotCount) + ".html";
            std::ofstream out(path);
            if (!out.is_open()) return std::make_shared<GemValue>("");

            out << "<html><head><script src='https://cdn.plot.ly/plotly-latest.min.js'></script>";
            out << "<style>body, html { margin: 0; padding: 0; height: 100%; overflow: hidden; } #gd { height: 100vh; width: 100vw; }</style>";
            out << "</head><body>";
            out << "<div id='gd'></div>";
            out << "<script>";
            
            // Serialize traces
            out << "var data = [";
            if (std::holds_alternative<std::vector<std::shared_ptr<GemObject>>>(args[0]->value)) {
                const auto& traces = std::get<std::vector<std::shared_ptr<GemObject>>>(args[0]->value);
                for (size_t i = 0; i < traces.size(); ++i) {
                    out << serializeObject(traces[i]);
                    if (i < traces.size() - 1) out << ",";
                }
            } else if (std::holds_alternative<std::shared_ptr<GemObject>>(args[0]->value)) {
                out << serializeObject(std::get<std::shared_ptr<GemObject>>(args[0]->value));
            }
            out << "];\n";

            // Serialize layout
            out << "var layout = ";
            if (args.size() > 1) {
                if (std::holds_alternative<std::shared_ptr<GemObject>>(args[1]->value)) {
                    out << serializeObject(std::get<std::shared_ptr<GemObject>>(args[1]->value));
                } else if (std::holds_alternative<std::map<std::string, std::shared_ptr<GemValue>>>(args[1]->value)) {
                    out << serializeMap(std::get<std::map<std::string, std::shared_ptr<GemValue>>>(args[1]->value));
                } else {
                    out << "{}";
                }
            } else {
                out << "{ title: 'Gem Interactive Chart' }";
            }
            out << ";\n";

            out << "Plotly.newPlot('gd', data, layout, {responsive: true});";
            out << "</script></body></html>";
            out.close();

            return std::make_shared<GemValue>(path);
        }, true };

        methods["show"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty()) return std::make_shared<GemValue>(false);
            std::string path = args[0]->toString();
#ifdef __APPLE__
            std::string cmd = "open \"" + path + "\"";
#elif defined(_WIN32)
            std::string cmd = "start \"\" \"" + path + "\"";
#else
            std::string cmd = "xdg-open \"" + path + "\"";
#endif
            return std::make_shared<GemValue>(std::system(cmd.c_str()) == 0);
        }, true };

        // chart.server(path, port) - Start a mini-webserver to serve the chart
        methods["server"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty()) return std::make_shared<GemValue>(false);
            std::string path = args[0]->toString();
            int port = (args.size() > 1) ? (int)std::get<double>(args[1]->value) : 8080;
            
            std::thread([path, port]() {
                httplib::Server svr;
                svr.Get("/", [path](const httplib::Request&, httplib::Response& res) {
                    std::ifstream t(path);
                    std::stringstream buffer;
                    buffer << t.rdbuf();
                    res.set_content(buffer.str(), "text/html");
                });
                std::cout << "Chart server running at http://localhost:" << port << std::endl;
                svr.listen("0.0.0.0", port);
            }).detach();
            
            return std::make_shared<GemValue>(true);
        }, true };
    }

private:
    std::string serializeValue(std::shared_ptr<GemValue> val) {
        if (!val) return "null";
        if (std::holds_alternative<double>(val->value)) return std::to_string(std::get<double>(val->value));
        if (std::holds_alternative<std::string>(val->value)) return "'" + std::get<std::string>(val->value) + "'";
        if (std::holds_alternative<bool>(val->value)) return std::get<bool>(val->value) ? "true" : "false";
        if (std::holds_alternative<std::vector<double>>(val->value)) {
            const auto& v = std::get<std::vector<double>>(val->value);
            std::string s = "[";
            for (size_t i = 0; i < v.size(); ++i) {
                s += std::to_string(v[i]) + (i < v.size() - 1 ? "," : "");
            }
            return s + "]";
        }
        if (std::holds_alternative<std::vector<std::string>>(val->value)) {
            const auto& v = std::get<std::vector<std::string>>(val->value);
            std::string s = "[";
            for (size_t i = 0; i < v.size(); ++i) {
                s += "'" + v[i] + "'" + (i < v.size() - 1 ? "," : "");
            }
            return s + "]";
        }
        if (std::holds_alternative<std::vector<std::shared_ptr<GemObject>>>(val->value)) {
            const auto& v = std::get<std::vector<std::shared_ptr<GemObject>>>(val->value);
            std::string s = "[";
            for (size_t i = 0; i < v.size(); ++i) {
                s += serializeObject(v[i]) + (i < v.size() - 1 ? "," : "");
            }
            return s + "]";
        }
        if (std::holds_alternative<std::shared_ptr<GemObject>>(val->value)) return serializeObject(std::get<std::shared_ptr<GemObject>>(val->value));
        if (std::holds_alternative<std::map<std::string, std::shared_ptr<GemValue>>>(val->value)) return serializeMap(std::get<std::map<std::string, std::shared_ptr<GemValue>>>(val->value));
        return "null";
    }

    std::string serializeObject(std::shared_ptr<GemObject> obj) {
        if (!obj) return "{}";
        return serializeMap(obj->properties);
    }

    std::string serializeMap(const std::map<std::string, std::shared_ptr<GemValue>>& m) {
        std::string s = "{";
        for (auto it = m.begin(); it != m.end(); ++it) {
            s += "'" + it->first + "': " + serializeValue(it->second);
            if (std::next(it) != m.end()) s += ",";
        }
        return s + "}";
    }
};

// Data Science Object
class GemData : public GemSys {
public:
    GemData() : GemSys() {
        name = "data";
        
        // data.read_csv(path) -> Returns a vector of objects
        methods["read_csv"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            std::vector<std::shared_ptr<GemObject>> results;
            if (args.empty() || !std::holds_alternative<std::string>(args[0]->value)) return std::make_shared<GemValue>(results);
            
            std::string path = std::get<std::string>(args[0]->value);
            std::ifstream file(path);
            if (!file.is_open()) return std::make_shared<GemValue>(results);

            std::string line;
            std::vector<std::string> headers;
            if (std::getline(file, line)) {
                std::stringstream ss(line);
                std::string cell;
                while (std::getline(ss, cell, ',')) {
                    // Trim spaces and quotes
                    size_t first = cell.find_first_not_of(" \"\t\r\n");
                    size_t last = cell.find_last_not_of(" \"\t\r\n");
                    if (first != std::string::npos && last != std::string::npos) headers.push_back(cell.substr(first, last - first + 1));
                    else headers.push_back(cell);
                }
            }

            while (std::getline(file, line)) {
                if (line.empty()) continue;
                std::stringstream ss(line);
                std::string cell;
                auto obj = std::make_shared<GemObject>("DataRow");
                size_t hIdx = 0;
                while (std::getline(ss, cell, ',') && hIdx < headers.size()) {
                    size_t first = cell.find_first_not_of(" \"\t\r\n");
                    size_t last = cell.find_last_not_of(" \"\t\r\n");
                    std::string val = (first != std::string::npos && last != std::string::npos) ? cell.substr(first, last - first + 1) : cell;
                    
                    try {
                        double d = std::stod(val);
                        obj->set(headers[hIdx], std::make_shared<GemValue>(d));
                    } catch (...) {
                        obj->set(headers[hIdx], std::make_shared<GemValue>(val));
                    }
                    hIdx++;
                }
                results.push_back(obj);
            }
            return std::make_shared<GemValue>(results);
        }, true };

        // data.mean(vector)
        methods["mean"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty() || !std::holds_alternative<std::vector<double>>(args[0]->value)) return std::make_shared<GemValue>(0.0);
            const auto& v = std::get<std::vector<double>>(args[0]->value);
            if (v.empty()) return std::make_shared<GemValue>(0.0);
            double sum = 0;
            for (double d : v) sum += d;
            return std::make_shared<GemValue>(sum / v.size());
        }, true };

        // data.std(vector)
        methods["std"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty() || !std::holds_alternative<std::vector<double>>(args[0]->value)) return std::make_shared<GemValue>(0.0);
            const auto& v = std::get<std::vector<double>>(args[0]->value);
            if (v.size() < 2) return std::make_shared<GemValue>(0.0);
            double sum = 0;
            for (double d : v) sum += d;
            double mean = sum / v.size();
            double sq_sum = 0;
            for (double d : v) sq_sum += (d - mean) * (d - mean);
            return std::make_shared<GemValue>(std::sqrt(sq_sum / (v.size() - 1)));
        }, true };
    }
};

// Docker and K3s Object
class GemContainer : public GemSys {
public:
    GemContainer() : GemSys() {
        name = "container";
        
        // container.docker_run(image, command)
        methods["docker_run"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.size() < 2) return std::make_shared<GemValue>(false);
            std::string image = args[0]->toString();
            std::string cmdVal = args[1]->toString();
            std::string fullCmd = "docker run --rm " + image + " " + cmdVal;
            return std::make_shared<GemValue>((double)std::system(fullCmd.c_str()));
        }, true };

        // container.docker_ps()
        methods["docker_ps"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            std::string cmd = "docker ps";
            return std::make_shared<GemValue>((double)std::system(cmd.c_str()));
        }, true };

        // container.k3s_apply(yaml_content)
        methods["k3s_apply"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty()) return std::make_shared<GemValue>(false);
            std::string yaml = args[0]->toString();
            std::string filename = "/tmp/gem_k3s_" + std::to_string(std::time(nullptr)) + ".yaml";
            std::ofstream out(filename);
            out << yaml;
            out.close();
            
            std::string cmd = "kubectl apply -f " + filename;
            int ret = std::system(cmd.c_str());
            return std::make_shared<GemValue>(ret == 0);
        }, true };
        
        // container.k3s_get(resource)
        methods["k3s_get"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
             if (args.empty()) return std::make_shared<GemValue>("");
             std::string resource = args[0]->toString();
             std::string cmd = "kubectl get " + resource;
             return std::make_shared<GemValue>((double)std::system(cmd.c_str()));
        }, true };

        // container.k3s_pods()
        methods["k3s_pods"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
             std::string cmd = "kubectl get pods -A";
             return std::make_shared<GemValue>((double)std::system(cmd.c_str()));
        }, true };

        // container.k3s_nodes()
        methods["k3s_nodes"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
             std::string cmd = "kubectl get nodes";
             return std::make_shared<GemValue>((double)std::system(cmd.c_str()));
        }, true };

        // container.k3s_logs(pod_name)
        methods["k3s_logs"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
             if (args.empty()) return std::make_shared<GemValue>("");
             std::string pod = args[0]->toString();
             std::string cmd = "kubectl logs " + pod;
             return std::make_shared<GemValue>((double)std::system(cmd.c_str()));
        }, true };

        // container.docker_build(path, tag)
        methods["docker_build"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
             if (args.empty()) return std::make_shared<GemValue>(false);
             std::string path = args[0]->toString();
             std::string tag = (args.size() > 1) ? args[1]->toString() : "latest";
             std::string cmd = "docker build -t " + tag + " " + path;
             return std::make_shared<GemValue>((double)std::system(cmd.c_str()));
        }, true };

        // container.docker_stop(container_id)
        methods["docker_stop"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
             if (args.empty()) return std::make_shared<GemValue>(false);
             std::string id = args[0]->toString();
             std::string cmd = "docker stop " + id;
             return std::make_shared<GemValue>((double)std::system(cmd.c_str()));
        }, true };
    }
};

// Vagrant VM Object
class GemVM : public GemSys {
public:
    GemVM() : GemSys() {
        name = "vm";

        // vm.init(box_name)
        methods["init"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty()) return std::make_shared<GemValue>(false);
            std::string box = args[0]->toString();
            std::string cmd = "vagrant init " + box;
            return std::make_shared<GemValue>((double)std::system(cmd.c_str()));
        }, true };

        // vm.up()
        methods["up"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            return std::make_shared<GemValue>((double)std::system("vagrant up"));
        }, true };

        // vm.status()
        methods["status"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            return std::make_shared<GemValue>((double)std::system("vagrant status"));
        }, true };
        
        // vm.ssh(command)
        methods["ssh"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty()) return std::make_shared<GemValue>(false);
            std::string cmd = args[0]->toString();
            std::string fullCmd = "vagrant ssh -c \"" + cmd + "\"";
            return std::make_shared<GemValue>((double)std::system(fullCmd.c_str()));
        }, true };

        // vm.destroy()
        methods["destroy"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            return std::make_shared<GemValue>((double)std::system("vagrant destroy -f"));
        }, true };

        // vm.halt()
        methods["halt"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            return std::make_shared<GemValue>((double)std::system("vagrant halt"));
        }, true };
    }
};

// Golang Interop Object
class GemGo : public GemSys {
public:
    GemGo() : GemSys() {
        name = "go";
        // go.run(file_or_code)
        methods["run"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty()) return std::make_shared<GemValue>(false);
            std::string input = args[0]->toString();
            bool isFile = input.length() > 3 && input.substr(input.length() - 3) == ".go";
            if (isFile) {
                std::string cmd = "go run " + input;
                return std::make_shared<GemValue>((double)std::system(cmd.c_str()));
            } else {
                // Execute as inline code
                std::string filename = "/tmp/gem_go_" + std::to_string(std::time(nullptr)) + ".go";
                std::ofstream out(filename);
                out << input;
                out.close();
                std::string cmd = "go run " + filename;
                int ret = std::system(cmd.c_str());
                return std::make_shared<GemValue>((double)ret);
            }
        }, true };
        // go.build(file)
        methods["build"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty()) return std::make_shared<GemValue>(false);
            std::string file = args[0]->toString();
            std::string cmd = "go build " + file;
            return std::make_shared<GemValue>((double)std::system(cmd.c_str()));
        }, true };
    }
};

// Ruby Interop Object
class GemRuby : public GemSys {
public:
    GemRuby() : GemSys() {
        name = "ruby";
        // ruby.run(file_or_code)
        methods["run"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty()) return std::make_shared<GemValue>(false);
            std::string input = args[0]->toString();
            bool isFile = input.length() > 3 && input.substr(input.length() - 3) == ".rb";
            if (isFile) {
                std::string cmd = "ruby " + input;
                return std::make_shared<GemValue>((double)std::system(cmd.c_str()));
            } else {
                // Execute as inline code
                std::string cmd = "ruby -e '" + input + "'";
                return std::make_shared<GemValue>((double)std::system(cmd.c_str()));
            }
        }, true };
    }
};

// Node.js Interop Object
class GemNode : public GemSys {
public:
    GemNode() : GemSys() {
        name = "node";
        // node.run(file_or_code)
        methods["run"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty()) return std::make_shared<GemValue>(false);
            std::string input = args[0]->toString();
            bool isFile = input.length() > 3 && input.substr(input.length() - 3) == ".js";
            if (isFile) {
                std::string cmd = "node " + input;
                return std::make_shared<GemValue>((double)std::system(cmd.c_str()));
            } else {
                // Execute as inline code
                std::string cmd = "node -e \"" + input + "\"";
                return std::make_shared<GemValue>((double)std::system(cmd.c_str()));
            }
        }, true };
        
        // node.npm_install(package)
        methods["npm_install"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty()) return std::make_shared<GemValue>(false);
            std::string pkg = args[0]->toString();
            std::string cmd = "npm install " + pkg;
            return std::make_shared<GemValue>((double)std::system(cmd.c_str()));
        }, true };
    }
};

// Rust Interop Object
class GemRust : public GemSys {
public:
    GemRust() : GemSys() {
        name = "rust";
        // rust.run(file_or_code)
        methods["run"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty()) return std::make_shared<GemValue>(false);
            std::string input = args[0]->toString();
            bool isFile = input.length() > 3 && input.substr(input.length() - 3) == ".rs";
            if (isFile) {
                // Compile and run: rustc file.rs && ./file
                std::string binName = input.substr(0, input.find_last_of("."));
                std::string cmd = "rustc " + input + " && ./" + binName;
                return std::make_shared<GemValue>((double)std::system(cmd.c_str()));
            } else {
                // Execute as inline code
                std::string filename = "/tmp/gem_rust_" + std::to_string(std::time(nullptr)) + ".rs";
                std::ofstream out(filename);
                out << "fn main() {\n" << input << "\n}";
                out.close();
                std::string binName = filename.substr(0, filename.find_last_of("."));
                std::string cmd = "rustc " + filename + " -o " + binName + " && " + binName;
                int ret = std::system(cmd.c_str());
                return std::make_shared<GemValue>((double)ret);
            }
        }, true };

        // rust.cargo_new(name)
        methods["cargo_new"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
             if (args.empty()) return std::make_shared<GemValue>(false);
             std::string name = args[0]->toString();
             std::string cmd = "cargo new " + name;
             return std::make_shared<GemValue>((double)std::system(cmd.c_str()));
        }, true };
    }
};

// GemFin Module (yfinance + TradingView)
class GemFin : public GemSys {
public:
    GemFin() : GemSys() {
        name = "fin";
        set("www", std::make_shared<GemValue>(std::make_shared<GemWWW>()));

        methods["ticker"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty() || !std::holds_alternative<std::string>(args[0]->value)) return std::make_shared<GemValue>();
            std::string symbol = std::get<std::string>(args[0]->value);
            char* response = call_yfinance_ticker((char*)symbol.c_str());
            if (response == NULL) return std::make_shared<GemValue>();
            std::string res_str(response);
            free(response);
            
            auto obj = std::make_shared<GemObject>("Ticker");
            auto parseStr = [&](const std::string& key) {
                size_t kPos = res_str.find("\"" + key + "\":");
                if (kPos != std::string::npos) {
                    size_t vStart = res_str.find("\"", kPos + key.length() + 3);
                    if (vStart != std::string::npos) {
                        size_t vEnd = res_str.find("\"", vStart + 1);
                        if (vEnd != std::string::npos) return res_str.substr(vStart + 1, vEnd - vStart - 1);
                    }
                }
                return std::string("");
            };
            auto parseNum = [&](const std::string& key) {
                size_t kPos = res_str.find("\"" + key + "\":");
                if (kPos != std::string::npos) {
                    size_t vStart = kPos + key.length() + 3;
                    size_t vEnd = res_str.find_first_of(",}", vStart);
                    if (vEnd != std::string::npos) {
                        try { return std::stod(res_str.substr(vStart, vEnd - vStart)); } catch(...) {}
                    }
                }
                return 0.0;
            };

            obj->set("symbol", std::make_shared<GemValue>(parseStr("symbol")));
            obj->set("price", std::make_shared<GemValue>(parseNum("price")));
            obj->set("open", std::make_shared<GemValue>(parseNum("open")));
            obj->set("high", std::make_shared<GemValue>(parseNum("high")));
            obj->set("low", std::make_shared<GemValue>(parseNum("low")));
            obj->set("volume", std::make_shared<GemValue>(parseNum("volume")));
            obj->set("currency", std::make_shared<GemValue>(parseStr("currency")));
            return std::make_shared<GemValue>(obj);
        }, true };

        auto parseJSONArray = [](const std::string& json) {
            std::vector<std::shared_ptr<GemObject>> results;
            size_t pos = 0;
            while ((pos = json.find('{', pos)) != std::string::npos) {
                size_t end = json.find('}', pos);
                if (end == std::string::npos) break;
                std::string objStr = json.substr(pos, end - pos + 1);
                auto obj = std::make_shared<GemObject>("FinancialData");
                size_t kPos = 0;
                while ((kPos = objStr.find('"', kPos)) != std::string::npos) {
                    size_t kEnd = objStr.find('"', kPos + 1);
                    if (kEnd == std::string::npos) break;
                    std::string key = objStr.substr(kPos + 1, kEnd - kPos - 1);
                    size_t vStart = objStr.find(':', kEnd);
                    if (vStart == std::string::npos) break;
                    vStart++;
                    while(vStart < objStr.size() && isspace(objStr[vStart])) vStart++;
                    if (vStart < objStr.size() && objStr[vStart] == '"') {
                        size_t vEnd = objStr.find('"', vStart + 1);
                        if (vEnd != std::string::npos) {
                            obj->set(key, std::make_shared<GemValue>(objStr.substr(vStart + 1, vEnd - vStart - 1)));
                            kPos = vEnd + 1;
                        } else break;
                    } else if (vStart < objStr.size()) {
                        size_t vEnd = objStr.find_first_of(",}", vStart);
                        if (vEnd != std::string::npos) {
                            std::string valStr = objStr.substr(vStart, vEnd - vStart);
                            try {
                                if (valStr == "true") obj->set(key, std::make_shared<GemValue>(true));
                                else if (valStr == "false") obj->set(key, std::make_shared<GemValue>(false));
                                else if (valStr == "null") obj->set(key, std::make_shared<GemValue>());
                                else obj->set(key, std::make_shared<GemValue>(std::stod(valStr)));
                            } catch(...) { obj->set(key, std::make_shared<GemValue>(valStr)); }
                            kPos = vEnd;
                        } else break;
                    } else break;
                }
                results.push_back(obj);
                pos = end + 1;
            }
            return results;
        };

        methods["high_yield_bonds"] = { [parseJSONArray](std::vector<std::shared_ptr<GemValue>> args) {
            char* response = call_tradingview_high_yield((char*)"bonds");
            if (response == NULL) return std::make_shared<GemValue>(std::vector<std::shared_ptr<GemObject>>{});
            std::string res_str(response);
            free(response);
            return std::make_shared<GemValue>(parseJSONArray(res_str));
        }, true };

        methods["high_yield_etfs"] = { [parseJSONArray](std::vector<std::shared_ptr<GemValue>> args) {
            char* response = call_tradingview_high_yield((char*)"etfs");
            if (response == NULL) return std::make_shared<GemValue>(std::vector<std::shared_ptr<GemObject>>{});
            std::string res_str(response);
            free(response);
            return std::make_shared<GemValue>(parseJSONArray(res_str));
        }, true };

        methods["high_yield_equities"] = { [parseJSONArray](std::vector<std::shared_ptr<GemValue>> args) {
            char* response = call_tradingview_high_yield((char*)"equities");
            if (response == NULL) return std::make_shared<GemValue>(std::vector<std::shared_ptr<GemObject>>{});
            std::string res_str(response);
            free(response);
            return std::make_shared<GemValue>(parseJSONArray(res_str));
        }, true };

        methods["dashboard"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            int port = (args.empty()) ? 8081 : (int)std::get<double>(args[0]->value);
            std::thread([port]() {
                httplib::Server svr;
                svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
                    res.set_content(R"HTML(<!DOCTYPE html>
<html>
<head>
    <title>Gem Financial Dashboard</title>
    <style>
        body { font-family: sans-serif; margin: 20px; background: #f0f2f5; }
        h1 { color: #1a73e8; }
        .table-container { background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); margin-bottom: 20px; overflow-x: auto; }
        table { width: 100%; border-collapse: collapse; }
        th, td { padding: 12px; border-bottom: 1px solid #eee; text-align: left; }
        th { background: #f8f9fa; font-weight: bold; }
        tr:hover { background: #f1f3f4; }
        .yield { color: #28a745; font-weight: bold; }
        .price { color: #17a2b8; }
        .tabs { margin-bottom: 20px; }
        .tab { padding: 10px 20px; cursor: pointer; border: 1px solid #ccc; background: #fff; margin-right: 5px; border-radius: 4px; }
        .tab.active { background: #1a73e8; color: white; border-color: #1a73e8; }
    </style>
</head>
<body>
    <h1>Gem Financial Dashboard</h1>
    <div class="tabs">
        <button class="tab active" onclick="loadData('bonds')">High Yield Bonds</button>
        <button class="tab" onclick="loadData('etfs')">High Yield ETFs</button>
        <button class="tab" onclick="loadData('equities')">High Yield Equities</button>
    </div>
    <div id="content" class="table-container">Loading data...</div>
    <script>
        async function loadData(type) {
            document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
            if (event && event.target && event.target.classList) event.target.classList.add('active');
            document.getElementById('content').innerHTML = 'Loading ' + type + '...';
            try {
                const response = await fetch('/api/' + type);
                const data = await response.json();
                renderTable(data);
            } catch (e) { document.getElementById('content').innerHTML = 'Error: ' + e; }
        }
        function renderTable(data) {
            if (!data || data.length === 0) { document.getElementById('content').innerHTML = 'No data.'; return; }
            let keys = Object.keys(data[0]);
            let html = '<table><thead><tr>' + keys.map(k => '<th>'+k+'</th>').join('') + '</tr></thead><tbody>';
            data.forEach(row => {
                html += '<tr>' + keys.map(k => {
                    let c = k.includes('yield') ? 'yield' : (k=='close'||k=='price'?'price':'');
                    return '<td class="'+c+'">'+(row[k]!==null?row[k]:'-')+'</td>';
                }).join('') + '</tr>';
            });
            document.getElementById('content').innerHTML = html + '</tbody></table>';
        }
        loadData('bonds');
    </script>
</body>
</html>)HTML", "text/html");
                });
                
                auto serveAPI = [](const char* type, httplib::Response& res) {
                    char* response = call_tradingview_high_yield((char*)type);
                    if (response) {
                        res.set_content(response, "application/json");
                        free(response);
                    } else res.set_content("[]", "application/json");
                };
                svr.Get("/api/bonds", [serveAPI](const httplib::Request&, httplib::Response& res) { serveAPI("bonds", res); });
                svr.Get("/api/etfs", [serveAPI](const httplib::Request&, httplib::Response& res) { serveAPI("etfs", res); });
                svr.Get("/api/equities", [serveAPI](const httplib::Request&, httplib::Response& res) { serveAPI("equities", res); });

                std::cout << "Starting Financial Dashboard on port " << port << "..." << std::endl;
                svr.listen("0.0.0.0", port);
            }).detach();
            return std::make_shared<GemValue>(true);
        }, true };

#ifdef HAS_QUANTLIB
        methods["date"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
             int d = (args.size() > 0) ? (int)std::get<double>(args[0]->value) : 1;
             int m = (args.size() > 1) ? (int)std::get<double>(args[1]->value) : 1;
             int y = (args.size() > 2) ? (int)std::get<double>(args[2]->value) : 2024;
             QuantLib::Date date(d, (QuantLib::Month)m, y);
             auto obj = std::make_shared<GemObject>("Date");
             obj->set("day", std::make_shared<GemValue>((double)d));
             obj->set("month", std::make_shared<GemValue>((double)m));
             obj->set("year", std::make_shared<GemValue>((double)y));
             obj->set("serial", std::make_shared<GemValue>((double)date.serialNumber()));
             obj->set("str", std::make_shared<GemValue>(std::to_string(d) + "/" + std::to_string(m) + "/" + std::to_string(y)));
             return std::make_shared<GemValue>(obj);
        }, true };
        methods["bs_price"] = { std::function<std::shared_ptr<GemValue>(std::vector<std::shared_ptr<GemValue>>)>([](std::vector<std::shared_ptr<GemValue>> args) {
             if (args.size() < 6) return std::make_shared<GemValue>(0.0);
             std::string type = args[0]->toString();
             double strike = std::get<double>(args[1]->value), spot = std::get<double>(args[2]->value), rate = std::get<double>(args[3]->value), vol = std::get<double>(args[4]->value), t = std::get<double>(args[5]->value);
             QuantLib::Option::Type optionType = (type == "put" || type == "Put") ? QuantLib::Option::Put : QuantLib::Option::Call;
             QuantLib::Date today = QuantLib::Date::todaysDate();
             QuantLib::Settings::instance().evaluationDate() = today;
             QuantLib::Date maturity = today + (int)(t*365);
             auto exercise = QuantLib::ext::make_shared<QuantLib::EuropeanExercise>(maturity);
             auto payoff = QuantLib::ext::make_shared<QuantLib::PlainVanillaPayoff>(optionType, strike);
             auto underlyingH = QuantLib::Handle<QuantLib::Quote>(QuantLib::ext::make_shared<QuantLib::SimpleQuote>(spot));
             auto rTS = QuantLib::Handle<QuantLib::YieldTermStructure>(QuantLib::ext::make_shared<QuantLib::FlatForward>(today, rate, QuantLib::Actual365Fixed()));
             auto volTS = QuantLib::Handle<QuantLib::BlackVolTermStructure>(QuantLib::ext::make_shared<QuantLib::BlackConstantVol>(today, QuantLib::TARGET(), vol, QuantLib::Actual365Fixed()));
             auto process = QuantLib::ext::make_shared<QuantLib::BlackScholesProcess>(underlyingH, rTS, volTS);
             QuantLib::VanillaOption option(payoff, exercise);
             option.setPricingEngine(QuantLib::ext::make_shared<QuantLib::AnalyticEuropeanEngine>(process));
             double npv = option.NPV();
             return std::make_shared<GemValue>(npv == 0.0 ? 0.000001 : npv);
        }), true };
        methods["greeks"] = { std::function<std::shared_ptr<GemValue>(std::vector<std::shared_ptr<GemValue>>)>([](std::vector<std::shared_ptr<GemValue>> args) {
             if (args.size() < 6) return std::make_shared<GemValue>(0.0);
             std::string type = args[0]->toString();
             double strike = std::get<double>(args[1]->value), spot = std::get<double>(args[2]->value), rate = std::get<double>(args[3]->value), vol = std::get<double>(args[4]->value), t = std::get<double>(args[5]->value);
             QuantLib::Option::Type optionType = (type == "put" || type == "Put") ? QuantLib::Option::Put : QuantLib::Option::Call;
             QuantLib::Date today = QuantLib::Date::todaysDate();
             QuantLib::Settings::instance().evaluationDate() = today;
             QuantLib::Date maturity = today + (int)(t*365);
             auto exercise = QuantLib::ext::make_shared<QuantLib::EuropeanExercise>(maturity);
             auto payoff = QuantLib::ext::make_shared<QuantLib::PlainVanillaPayoff>(optionType, strike);
             auto underlyingH = QuantLib::Handle<QuantLib::Quote>(QuantLib::ext::make_shared<QuantLib::SimpleQuote>(spot));
             auto rTS = QuantLib::Handle<QuantLib::YieldTermStructure>(QuantLib::ext::make_shared<QuantLib::FlatForward>(today, rate, QuantLib::Actual365Fixed()));
             auto volTS = QuantLib::Handle<QuantLib::BlackVolTermStructure>(QuantLib::ext::make_shared<QuantLib::BlackConstantVol>(today, QuantLib::TARGET(), vol, QuantLib::Actual365Fixed()));
             auto process = QuantLib::ext::make_shared<QuantLib::BlackScholesProcess>(underlyingH, rTS, volTS);
             QuantLib::VanillaOption option(payoff, exercise);
             option.setPricingEngine(QuantLib::ext::make_shared<QuantLib::AnalyticEuropeanEngine>(process));
             auto obj = std::make_shared<GemObject>("Greeks");
             obj->set("npv", std::make_shared<GemValue>(option.NPV()));
             obj->set("delta", std::make_shared<GemValue>(option.delta()));
             obj->set("gamma", std::make_shared<GemValue>(option.gamma()));
             obj->set("theta", std::make_shared<GemValue>(option.theta()));
             obj->set("vega", std::make_shared<GemValue>(option.vega()));
             return std::make_shared<GemValue>(obj);
        }), true };
#endif
    }
};

class GemBSM : public GemFin {
public:
    GemBSM() : GemFin() {
        name = "bsm";
        methods["price_american"] = { std::function<std::shared_ptr<GemValue>(std::vector<std::shared_ptr<GemValue>>)>(
            [this](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.size() < 3) return std::make_shared<GemValue>(0.0);
            std::string symbol = args[0]->toString(), type = args[1]->toString(), duration = args[2]->toString();
            auto tickerVal = call("ticker", {std::make_shared<GemValue>(symbol)});
            if (!std::holds_alternative<std::shared_ptr<GemObject>>(tickerVal->value)) return std::make_shared<GemValue>(0.0);
            auto tickerObj = std::get<std::shared_ptr<GemObject>>(tickerVal->value);
            double spot = std::get<double>(tickerObj->get("price")->value), T = 0.25;
            if (duration == "weekly") T = 0.019178; else if (duration == "monthly") T = 0.082192;
            auto euroPriceVal = call("bs_price", {std::make_shared<GemValue>(type), std::make_shared<GemValue>(spot), std::make_shared<GemValue>(spot), std::make_shared<GemValue>(0.05), std::make_shared<GemValue>(0.30), std::make_shared<GemValue>(T)});
            double euro_price = std::get<double>(euroPriceVal->value), premium = (type == "put" || type == "Put") ? euro_price * 0.05 : euro_price * 0.01;
            return std::make_shared<GemValue>(euro_price + premium);
        }), true };
    }
};

inline bool GemImg::initialized = false;
inline bool GemAI::pythonInitialized = false;

#include "gem_drvr.hpp"
#include "gem_astro.hpp"

#endif
