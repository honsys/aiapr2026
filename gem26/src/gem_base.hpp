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
#include <regex>

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
                std::cout << "  sys, math, ai, text, rex, algo, bev, file, zip, nlp, img, www, cdn, geo, mobl, trek, seo, cpp, tcp, itr, thread, data, k3s, vm, go, ruby, node, rust, fin, bsm, chart, astro, drvr, art" << std::endl;
                std::cout << "\nKeywords for Documentation:" << std::endl;
                std::cout << "  fun, obj, use, alias, his, lib, end, if, else, while," << std::endl;
                std::cout << "  int, double, string, bool, exit, quit, langport," << std::endl;
                std::cout << "  true, false, null, nil, nan, vectors, repl" << std::endl;
                std::cout << "\nBuiltin Functions:" << std::endl;
                std::cout << "  isnil(x)  isnan(x)  tonum(x)  tostr(x)" << std::endl;
                std::cout << "\nOperators:" << std::endl;
                std::cout << "  Arithmetic:   +  -  *  /" << std::endl;
                std::cout << "  Compound:     +=  -=  *=  /=" << std::endl;
                std::cout << "  Comparison:   ==  !=  >  >=  <  <=" << std::endl;
                std::cout << "  Logical:      !  (prefix not)  &&  ||" << std::endl;
                std::cout << "  Separator:    ;  (multiple statements per line)" << std::endl;
                std::cout << "  String concat: +" << std::endl;
                std::cout << "\nMobile & Cross-Platform:" << std::endl;
                std::cout << "  mobl phone(\"name\")  phone.dictate(text)  phone.make_feature(lat,lon,text)" << std::endl;
                std::cout << "  ./gem travel_log.g  →  http://localhost:8080  (Android/iPhone/macOS/Linux/Win11)" << std::endl;
                std::cout << "\nDetailed help: help \"topic\" or help(topic)" << std::endl;
                std::cout << "Full reference: helpfull  or  sys.helpfull()  (writes helpfull_DATETIME.md)" << std::endl;
                std::cout << "Paged reference: helpless  or  sys.helpless()  (writes + pipes through less)" << std::endl;
                } else {
                std::string topic = args[0]->toString();
                std::cout << "\n--- Help: " << topic << " ---" << std::endl;
                if (topic == "fin") {
                    std::cout << "Module: fin — Financial engineering (yfinance + TradingView + QuantLib)" << std::endl;
                    std::cout << "Market data:" << std::endl;
                    std::cout << "  ticker(symbol)            — real-time data -> object" << std::endl;
                    std::cout << "    returns: .price, .volume, .open, .high, .low, .close," << std::endl;
                    std::cout << "             .pe_ratio, .market_cap, .dividend_yield, .symbol" << std::endl;
                    std::cout << "  high_yield_bonds()        — top 50 bonds by yield -> vector" << std::endl;
                    std::cout << "  high_yield_etfs()         — top 50 ETFs by dividend yield -> vector" << std::endl;
                    std::cout << "  high_yield_equities()     — top 50 stocks by dividend yield -> vector" << std::endl;
                    std::cout << "  dashboard()               — serve financial dashboard on :8082 -> bool" << std::endl;
                    std::cout << "Options pricing (QuantLib):" << std::endl;
                    std::cout << "  bs_price(type,strike,spot,rate,vol,t) -> double (NPV)" << std::endl;
                    std::cout << "    type: \"call\" or \"put\"; t: years to expiry (0.25 = 3 months)" << std::endl;
                    std::cout << "  greeks(type,strike,spot,rate,vol,t) -> object" << std::endl;
                    std::cout << "    returns: .npv, .delta, .gamma, .theta, .vega (all double)" << std::endl;
                    std::cout << "Date arithmetic (QuantLib):" << std::endl;
                    std::cout << "  date(d,m,y)               — QuantLib Date -> object" << std::endl;
                    std::cout << "    returns: .day, .month, .year, .serial (int), .str (string)" << std::endl;
                    std::cout << "  calendar(name)            — market calendar -> object" << std::endl;
                    std::cout << "    name: \"TARGET\", \"USA\", \"UK\" (default: \"TARGET\")" << std::endl;
                    std::cout << "  is_holiday(cal,d,m,y)     — is market holiday? -> bool" << std::endl;
                    std::cout << "  add_days(d,m,y,n)         — add n calendar days -> Date object" << std::endl;
                    std::cout << "    returns: .day, .month, .year, .str" << std::endl;
                    std::cout << "  diff_days(d1,m1,y1,d2,m2,y2) — days between dates -> double" << std::endl;
                    std::cout << "Example:" << std::endl;
                    std::cout << "  string t = fin.ticker(\"AAPL\")" << std::endl;
                    std::cout << "  sys.print(t.price, t.volume)" << std::endl;
                    std::cout << "  double p = fin.bs_price(\"call\", 150.0, 148.0, 0.05, 0.25, 0.25)" << std::endl;
                    std::cout << "  string g = fin.greeks(\"call\", 150.0, 148.0, 0.05, 0.25, 0.25)" << std::endl;
                    std::cout << "  sys.print(g.delta, g.gamma)" << std::endl;
                    std::cout << "  string d2 = fin.add_days(15, 1, 2026, 30)" << std::endl;
                    std::cout << "  sys.print(d2.str)" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    QuantLib directly; no built-in market data" << std::endl;
                    std::cout << "  Python: yfinance, quantlib-python, pandas-datareader" << std::endl;
                    std::cout << "  Julia:  MarketData.jl, QuantLib.jl" << std::endl;
                    std::cout << "  Go:     piquette/quantlib (bindings); no built-in" << std::endl;
                    std::cout << "  Ruby:   quantlib-ruby gem; no built-in market data" << std::endl;
                    std::cout << "  Rust:   no mature QuantLib bindings; ta crate for indicators" << std::endl;
                } else if (topic == "bsm") {
                    std::cout << "Module: bsm — Black-Scholes-Merton (inherits all fin methods)" << std::endl;
                    std::cout << "  price_american(symbol,type,duration) — PDE American option pricing" << std::endl;
                    std::cout << "    duration: 'weekly' | 'monthly' | 'quarterly' (default)" << std::endl;
                    std::cout << "  + all fin methods: ticker, bs_price, greeks, date, calendar, ..." << std::endl;
                    std::cout << "Example:" << std::endl;
                    std::cout << "  double p = bsm.price_american(\"AAPL\", \"call\", \"monthly\")" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    QuantLib AmericanOption + FDBlackScholesVanillaEngine" << std::endl;
                    std::cout << "  Python: quantlib-python AmericanOption + FDBlackScholesVanillaEngine" << std::endl;
                    std::cout << "  Julia:  QuantLib.jl AmericanExercise" << std::endl;
                    std::cout << "  Go/Ruby/Rust: no mature American option PDE solvers" << std::endl;
                } else if (topic == "img") {
                    std::cout << "Module: img — Image processing via ImageMagick" << std::endl;
                    std::cout << "  resize(src,width,height,dest) — resize image file" << std::endl;
                    std::cout << "Example: img.resize(\"photo.jpg\", 800, 600, \"thumb.jpg\")" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    Magick++ (ImageMagick C++ API), OpenCV" << std::endl;
                    std::cout << "  Python: Pillow (PIL), OpenCV, imageio" << std::endl;
                    std::cout << "  Julia:  Images.jl, ImageMagick.jl" << std::endl;
                    std::cout << "  Go:     imaging package, disintegration/imaging" << std::endl;
                    std::cout << "  Ruby:   rmagick gem, mini_magick gem" << std::endl;
                    std::cout << "  Rust:   image crate, magick_rust crate" << std::endl;
                } else if (topic == "k3s") {
                    std::cout << "Module: k3s — Docker and Kubernetes (K3s) management" << std::endl;
                    std::cout << "Docker:" << std::endl;
                    std::cout << "  docker_run(image,cmd)  — run container (--rm)" << std::endl;
                    std::cout << "  docker_ps()            — list running containers" << std::endl;
                    std::cout << "  docker_build(path,tag) — build image from Dockerfile" << std::endl;
                    std::cout << "  docker_stop(id)        — stop container" << std::endl;
                    std::cout << "Kubernetes (K3s):" << std::endl;
                    std::cout << "  k3s_apply(yaml)        — apply manifest (kubectl apply)" << std::endl;
                    std::cout << "  k3s_get(resource)      — get resources (pods/svc/deploy)" << std::endl;
                    std::cout << "  k3s_pods()             — list all pods" << std::endl;
                    std::cout << "  k3s_nodes()            — list cluster nodes" << std::endl;
                    std::cout << "  k3s_logs(pod)          — get pod logs" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    Docker SDK (HTTP API); no built-in k8s client" << std::endl;
                    std::cout << "  Python: docker-py, kubernetes Python client" << std::endl;
                    std::cout << "  Julia:  no official Docker/k8s SDK" << std::endl;
                    std::cout << "  Go:     docker/docker/client, k8s.io/client-go (official)" << std::endl;
                    std::cout << "  Ruby:   docker-api gem, kubeclient gem" << std::endl;
                    std::cout << "  Rust:   bollard (Docker), kube-rs (Kubernetes)" << std::endl;
                } else if (topic == "vm") {
                    std::cout << "Module: vm — Vagrant virtual machine management" << std::endl;
                    std::cout << "  init(box)    — vagrant init <box> (e.g. \"ubuntu/focal64\")" << std::endl;
                    std::cout << "  up()         — vagrant up (start VM)" << std::endl;
                    std::cout << "  ssh(cmd)     — vagrant ssh -c \"cmd\"" << std::endl;
                    std::cout << "  status()     — vagrant status" << std::endl;
                    std::cout << "  halt()       — vagrant halt (power off)" << std::endl;
                    std::cout << "  destroy()    — vagrant destroy -f" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    libvirt C API; no built-in Vagrant" << std::endl;
                    std::cout << "  Python: python-vagrant, libvirt-python" << std::endl;
                    std::cout << "  Julia:  no official Vagrant bindings" << std::endl;
                    std::cout << "  Go:     no official Vagrant SDK; use os/exec" << std::endl;
                    std::cout << "  Ruby:   Vagrant itself is written in Ruby" << std::endl;
                    std::cout << "  Rust:   no official Vagrant bindings; use std::process::Command" << std::endl;
                } else if (topic == "go" || topic == "ruby" || topic == "node" || topic == "rust") {
                    std::cout << "Module: " << topic << " — Polyglot interop (direct execution)" << std::endl;
                    std::cout << "  run(input)       — execute source file or inline code string" << std::endl;
                    if (topic == "go")   std::cout << "  build(file)      — compile Go binary (go build)" << std::endl;
                    if (topic == "node") std::cout << "  npm_install(pkg) — install NPM package" << std::endl;
                    if (topic == "rust") std::cout << "  cargo_new(name)  — create new Cargo project" << std::endl;
                    std::cout << "Note: Unlike 'use', these run code natively without AI translation." << std::endl;
                    std::cout << "Example:" << std::endl;
                    if (topic == "go")   std::cout << "  go.run(\"hello.go\")  # or go.run(\"package main...\")" << std::endl;
                    if (topic == "ruby") std::cout << "  ruby.run(\"puts 'hello'\")" << std::endl;
                    if (topic == "node") std::cout << "  node.run(\"console.log('hello')\")" << std::endl;
                    if (topic == "rust") std::cout << "  rust.run(\"fn main(){println!(\\\"hi\\\");}\")" << std::endl;
                    std::cout << "Comparisons (calling " << topic << " from other languages):" << std::endl;
                    std::cout << "  C++:    std::system() or popen() to invoke " << topic << " subprocess" << std::endl;
                    std::cout << "  Python: subprocess.run([\"" << topic << "\", ...]) or ctypes FFI" << std::endl;
                    std::cout << "  Julia:  run(`" << topic << " script`) or ccall for native libs" << std::endl;
                } else if (topic == "data") {
                    std::cout << "Module: data — Data science, statistics, and CSV" << std::endl;
                    std::cout << "  read_csv(path)   — read CSV into vector of row objects" << std::endl;
                    std::cout << "  mean(vector)     — arithmetic mean of numeric vector" << std::endl;
                    std::cout << "  std(vector)      — sample standard deviation" << std::endl;
                    std::cout << "Example:" << std::endl;
                    std::cout << "  string rows = data.read_csv(\"data.csv\")" << std::endl;
                    std::cout << "  double m = data.mean([1.0, 2.0, 3.0])" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    Eigen, Armadillo, csv-parser" << std::endl;
                    std::cout << "  Python: pandas, numpy (pd.read_csv, np.mean, np.std)" << std::endl;
                    std::cout << "  Julia:  DataFrames.jl, CSV.jl, Statistics.mean()" << std::endl;
                    std::cout << "  Go:     gonum.org/v1/gonum/stat, encoding/csv" << std::endl;
                    std::cout << "  Ruby:   csv stdlib, daru gem" << std::endl;
                    std::cout << "  Rust:   polars crate, csv crate, statrs crate" << std::endl;
                } else if (topic == "sys") {
                    std::cout << "Module: sys — System root (inherited by all objects)" << std::endl;
                    std::cout << "  print(...)                     — print to stdout" << std::endl;
                    std::cout << "  args()                         — CLI args as vector" << std::endl;
                    std::cout << "  async(func,[params],[timeout]) — background thread; returns thread handle" << std::endl;
                    std::cout << "  sethandler(signals,func)       — custom signal handler" << std::endl;
                    std::cout << "  host()                         — OS info string" << std::endl;
                    std::cout << "  exec(cmd)                      — run shell command, returns exit code" << std::endl;
                    std::cout << "  doc([path])                    — read file comments" << std::endl;
                    std::cout << "  help([topic])                  — this help system" << std::endl;
                    std::cout << "  helpfull()                     — write complete reference to helpfull_DATETIME.md" << std::endl;
                    std::cout << "  helpless()                     — write + pipe reference through less" << std::endl;
                    std::cout << "  exit()                         — exit interpreter" << std::endl;
                    std::cout << "  langport(pattern,[output])     — AI-port foreign code to Gem" << std::endl;
                    std::cout << "  redirect(url,[port])           — HTTP redirect server" << std::endl;
                    std::cout << "  app(port,[routes])             — background HTTP server" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    std::cout, std::system(), std::thread, signal()" << std::endl;
                    std::cout << "  Python: print(), os.system(), threading.Thread, signal" << std::endl;
                    std::cout << "  Julia:  println(), run(), @async, Sys.KERNEL" << std::endl;
                    std::cout << "  Go:     fmt.Println(), os.Args, go func(), os/signal" << std::endl;
                    std::cout << "  Ruby:   puts, system(), Thread.new, Signal.trap" << std::endl;
                    std::cout << "  Rust:   println!(), std::process::Command, std::thread::spawn" << std::endl;
                } else if (topic == "drvr") {
                    std::cout << "Module: drvr — Device driver development (Linux/Win11/macOS/Android)" << std::endl;
                    std::cout << "  linux(name)   — generate Linux kernel module template (.c + Makefile)" << std::endl;
                    std::cout << "  win11(name)   — generate Windows Driver Framework (WDF) template" << std::endl;
                    std::cout << "  macos(name)   — generate macOS IOKit/DriverKit template" << std::endl;
                    std::cout << "  android(name) — generate Android HAL template" << std::endl;
                    std::cout << "  build(target) — cross-compile driver for target platform" << std::endl;
                    std::cout << "  deploy(target)— deploy driver to target device" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    Linux: module_init/module_exit macros; Win: WDK/WDF" << std::endl;
                    std::cout << "  Python: no native driver support (use ctypes/cffi for userspace)" << std::endl;
                    std::cout << "  Julia:  no native driver support" << std::endl;
                    std::cout << "  Go:     TinyGo for embedded; no kernel driver support" << std::endl;
                    std::cout << "  Ruby:   no native driver support" << std::endl;
                    std::cout << "  Rust:   linux-kernel-module-rust crate; growing kernel support" << std::endl;
                } else if (topic == "math") {
                    std::cout << "Module: math — Numeric + symbolic math (SymPy/Sage backend)" << std::endl;
                    std::cout << "Numeric functions (params: double → double):" << std::endl;
                    std::cout << "  sin(x)         — sine of x (radians) → double" << std::endl;
                    std::cout << "  cos(x)         — cosine of x (radians) → double" << std::endl;
                    std::cout << "  sqrt(x)        — square root of x → double" << std::endl;
                    std::cout << "  math.pi        — π constant (3.14159...) → double (property)" << std::endl;
                    std::cout << "Symbolic math (requires Python 3 + sympy or sage):" << std::endl;
                    std::cout << "  useSymPy()                    — switch to SymPy backend (default)" << std::endl;
                    std::cout << "  useSage()                     — switch to SageMath backend" << std::endl;
                    std::cout << "  diff(expr, [var])             — symbolic derivative → string" << std::endl;
                    std::cout << "    expr: string e.g. \"x**2 + 3*x\", var: string e.g. \"x\"" << std::endl;
                    std::cout << "  integrate(expr, [var])        — symbolic integral → string" << std::endl;
                    std::cout << "  simplify(expr)                — algebraic simplification → string" << std::endl;
                    std::cout << "  solve(expr, [var])            — solve equation → string" << std::endl;
                    std::cout << "  sym_latex(expr)               — LaTeX for expression → string" << std::endl;
                    std::cout << "  to_latex(val)                 — value/matrix → LaTeX string" << std::endl;
                    std::cout << "  write_latex(path, content, [standalone]) — write .tex file → bool" << std::endl;
                    std::cout << "    standalone=true adds \\documentclass preamble" << std::endl;
                    std::cout << "  read_latex(path)              — read LaTeX document body → string" << std::endl;
                    std::cout << "  parse_latex(text)             — extract $...$ expressions → list" << std::endl;
                    std::cout << "  compile_latex(path)           — run pdflatex on file → int (exit code)" << std::endl;
                    std::cout << "Indirect domain — LaTeX workflow:" << std::endl;
                    std::cout << "  string expr = \"x**2 + sin(x)\"" << std::endl;
                    std::cout << "  string d = math.diff(expr, \"x\")   # \"2*x + cos(x)\"" << std::endl;
                    std::cout << "  string tex = math.sym_latex(d)" << std::endl;
                    std::cout << "  math.write_latex(\"out.tex\", tex, true)" << std::endl;
                    std::cout << "  math.compile_latex(\"out.tex\")      # produces out.pdf" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    <cmath>; no built-in symbolic (use GiNaC/SymEngine)" << std::endl;
                    std::cout << "  Python: math.sin(); sympy.diff(), sympy.integrate()" << std::endl;
                    std::cout << "  Julia:  sin(); Symbolics.jl, SymEngine.jl" << std::endl;
                    std::cout << "  Go:     math.Sin(); no built-in symbolic" << std::endl;
                    std::cout << "  Ruby:   Math::sin(); no built-in symbolic" << std::endl;
                    std::cout << "  Rust:   f64::sin(); no built-in symbolic" << std::endl;
                } else if (topic == "tcp") {
                    std::cout << "Module: tcp — TCP/IP networking" << std::endl;
                    std::cout << "Server:" << std::endl;
                    std::cout << "  listen(port)          — bind server socket → int fd" << std::endl;
                    std::cout << "  accept(fd)            — block for client → Socket {.fd, .addr}" << std::endl;
                    std::cout << "Client:" << std::endl;
                    std::cout << "  connect(host, port)   — connect to server → int fd" << std::endl;
                    std::cout << "Data transfer:" << std::endl;
                    std::cout << "  send(fd, msg)         — send string → int bytes_sent" << std::endl;
                    std::cout << "  recv(fd, [size])      — receive data → string (size default 4096)" << std::endl;
                    std::cout << "  close(fd)             — close socket → bool" << std::endl;
                    std::cout << "Network info:" << std::endl;
                    std::cout << "  nic()    — network interfaces → vector of NIC objects" << std::endl;
                    std::cout << "    NIC properties: .name (string), .ip (string), .status (string)" << std::endl;
                    std::cout << "  routes() — routing table → vector of Route objects" << std::endl;
                    std::cout << "    Route properties: .dest (string), .gw (string), .iface (string)" << std::endl;
                    std::cout << "Example — echo server:" << std::endl;
                    std::cout << "  int srv = tcp.listen(8080)" << std::endl;
                    std::cout << "  string client = tcp.accept(srv)" << std::endl;
                    std::cout << "  string msg = tcp.recv(client.fd)" << std::endl;
                    std::cout << "  tcp.send(client.fd, msg)" << std::endl;
                    std::cout << "  tcp.close(client.fd)" << std::endl;
                    std::cout << "  tcp.close(srv)" << std::endl;
                    std::cout << "Example — client:" << std::endl;
                    std::cout << "  int fd = tcp.connect(\"localhost\", 8080)" << std::endl;
                    std::cout << "  tcp.send(fd, \"hello\")" << std::endl;
                    std::cout << "  string reply = tcp.recv(fd)" << std::endl;
                    std::cout << "  tcp.close(fd)" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    POSIX socket()/bind()/listen()/accept()/send()/recv()" << std::endl;
                    std::cout << "  Python: socket.socket(); s.bind(); s.listen(); s.accept()" << std::endl;
                    std::cout << "  Julia:  Sockets.listen(), accept(), read(), write()" << std::endl;
                    std::cout << "  Go:     net.Listen(), l.Accept(), conn.Read(), conn.Write()" << std::endl;
                    std::cout << "  Ruby:   TCPServer.new, server.accept, socket.gets" << std::endl;
                    std::cout << "  Rust:   TcpListener::bind(), listener.accept(), stream.read()" << std::endl;
                } else if (topic == "ai") {
                    std::cout << "Module: ai — AI integration (Gemini, Mistral, Ollama)" << std::endl;
                    std::cout << "Prompting:" << std::endl;
                    std::cout << "  prompt(text)                  — send prompt to current provider → string" << std::endl;
                    std::cout << "  prompt_native(text)           — Mistral C++ bridge → string" << std::endl;
                    std::cout << "    requires: mistralai >= 1.0, Python 3.14 embedded" << std::endl;
                    std::cout << "Provider switching:" << std::endl;
                    std::cout << "  useGemini()                   — switch to Gemini (default) → bool" << std::endl;
                    std::cout << "  useMistral(model)             — switch to Mistral API → bool" << std::endl;
                    std::cout << "    model: string e.g. \"mistral-small\", \"mistral-large\"" << std::endl;
                    std::cout << "  useOllama(model, [host])      — switch to local Ollama → bool" << std::endl;
                    std::cout << "    model: string e.g. \"llama3\", \"mistral\"" << std::endl;
                    std::cout << "    host: string (default \"localhost:11434\")" << std::endl;
                    std::cout << "Configuration:" << std::endl;
                    std::cout << "  setKey(key)                   — set API key → bool" << std::endl;
                    std::cout << "  setHost(host)                 — override API host → bool" << std::endl;
                    std::cout << "  setPath(path)                 — override API path → bool" << std::endl;
                    std::cout << "Properties (read-only after set):" << std::endl;
                    std::cout << "  ai.provider   — current provider string (\"gemini\"/\"mistral\"/\"ollama\")" << std::endl;
                    std::cout << "  ai.model      — current model string" << std::endl;
                    std::cout << "  ai.host       — current API host string" << std::endl;
                    std::cout << "Example:" << std::endl;
                    std::cout << "  ai.setKey(\"AIza...\")" << std::endl;
                    std::cout << "  string r = ai.prompt(\"Explain black holes\")" << std::endl;
                    std::cout << "  ai.useMistral(\"mistral-small\")" << std::endl;
                    std::cout << "  string r2 = ai.prompt(\"What is entropy?\")" << std::endl;
                    std::cout << "  ai.useGemini()   # switch back" << std::endl;
                    std::cout << "  ai.useOllama(\"llama3\")   # local model" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    libcurl + nlohmann/json; mistralai C++ SDK" << std::endl;
                    std::cout << "  Python: google-generativeai, mistralai, openai packages" << std::endl;
                    std::cout << "  Julia:  HTTP.jl + JSON3.jl (no official SDK)" << std::endl;
                    std::cout << "  Go:     google/generative-ai-go; sashabaranov/go-openai" << std::endl;
                    std::cout << "  Ruby:   ruby-openai gem" << std::endl;
                    std::cout << "  Rust:   async-openai crate" << std::endl;
                } else if (topic == "text") {
                    std::cout << "Module: text — String, document, and file-format processing" << std::endl;
                    std::cout << "String operations:" << std::endl;
                    std::cout << "  read(path)                    — read file into string → string" << std::endl;
                    std::cout << "  sub(s, start, [len])          — substring → string" << std::endl;
                    std::cout << "    start: int (0-based), len: int (default: to end)" << std::endl;
                    std::cout << "  replace(s, old, new)          — replace all occurrences → string" << std::endl;
                    std::cout << "PDF:" << std::endl;
                    std::cout << "  write_pdf(path, text)         — create PDF → bool" << std::endl;
                    std::cout << "  write_pdf_a(path, text)       — create PDF/A archival → bool" << std::endl;
                    std::cout << "  read_pdf(path)                — extract text from PDF → string" << std::endl;
                    std::cout << "Document formats (path: string, content: string):" << std::endl;
                    std::cout << "  read_markdown(path)           — read Markdown → string" << std::endl;
                    std::cout << "  write_markdown(path, text)    — write Markdown → bool" << std::endl;
                    std::cout << "  read_yaml(path)               — read YAML → object" << std::endl;
                    std::cout << "  write_yaml(path, obj)         — write YAML → bool" << std::endl;
                    std::cout << "  read_html(path)               — read HTML → string" << std::endl;
                    std::cout << "  write_html(path, text)        — write HTML → bool" << std::endl;
                    std::cout << "  read_xml(path)                — read XML → string" << std::endl;
                    std::cout << "  write_xml(path, text)         — write XML → bool" << std::endl;
                    std::cout << "Scientific formats:" << std::endl;
                    std::cout << "  read_fits_header(path)        — FITS header → object (key/value pairs)" << std::endl;
                    std::cout << "  read_hdf_header(path)         — HDF5 header → object (key/value pairs)" << std::endl;
                    std::cout << "Indirect domain — string manipulation:" << std::endl;
                    std::cout << "  Concatenation: s1 + s2  or  s += \" more\"" << std::endl;
                    std::cout << "  Length: s.length  or  s.size  or  s.count" << std::endl;
                    std::cout << "  Char access: s[0], s[1], ..." << std::endl;
                    std::cout << "  Regex: use rex module for pattern matching" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    std::string, fstream; pdfium/libharu for PDF; pugixml for XML" << std::endl;
                    std::cout << "  Python: str, open(); pypdf, markdown, pyyaml, lxml" << std::endl;
                    std::cout << "  Julia:  String, read(); no built-in PDF; YAML.jl, LightXML.jl" << std::endl;
                    std::cout << "  Go:     strings, os; no built-in PDF; encoding/xml" << std::endl;
                    std::cout << "  Ruby:   String, File; prawn (PDF); nokogiri (XML/HTML)" << std::endl;
                    std::cout << "  Rust:   String, std::fs; lopdf (PDF); quick-xml" << std::endl;
                } else if (topic == "rex") {
                    std::cout << "Module: rex — ECMAScript regular expressions" << std::endl;
                    std::cout << "All functions accept optional flags: \"i\"=case-insensitive" << std::endl;
                    std::cout << "  match(text,pattern,[flags])      -> bool   — true if pattern found" << std::endl;
                    std::cout << "  find(text,pattern,[flags])       -> string — first match or \"\"" << std::endl;
                    std::cout << "  findall(text,pattern,[flags])    -> list   — all non-overlapping matches" << std::endl;
                    std::cout << "  groups(text,pattern,[flags])     -> list   — capture groups from first match" << std::endl;
                    std::cout << "  sub(text,pattern,repl,[flags])   -> string — replace first match" << std::endl;
                    std::cout << "  gsub(text,pattern,repl,[flags])  -> string — replace all matches" << std::endl;
                    std::cout << "  split(text,pattern,[flags])      -> list   — split on pattern" << std::endl;
                    std::cout << "  count(text,pattern,[flags])      -> int    — count non-overlapping matches" << std::endl;
                    std::cout << "Example: rex.gsub(\"Hello World\", \"[aeiou]\", \"*\", \"i\") -> \"H*ll* W*rld\"" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    std::regex_search/replace (ECMAScript default)" << std::endl;
                    std::cout << "  Python: re.search(), re.sub(), re.findall(), re.split()" << std::endl;
                    std::cout << "  Julia:  match(), replace(), eachmatch(), split() with Regex" << std::endl;
                    std::cout << "  Go:     regexp.MatchString(), re.ReplaceAllString()" << std::endl;
                    std::cout << "  Ruby:   =~, .match(), .gsub(), .scan(), .split()" << std::endl;
                    std::cout << "  Rust:   regex crate: Regex::new(), re.find(), re.replace_all()" << std::endl;
                } else if (topic == "algo") {
                    std::cout << "Module: algo — Algorithms, sorting, and time" << std::endl;
                    std::cout << "  add(n1, n2, ...)          — sum all numeric args → double" << std::endl;
                    std::cout << "  quicksort(v)              — sort numeric vector in-place → vector" << std::endl;
                    std::cout << "    v: numeric vector (modified in-place and returned)" << std::endl;
                    std::cout << "  sort(v, [start], [end])   — sort slice of vector → vector" << std::endl;
                    std::cout << "    start: int (default 0), end: int (default v.length)" << std::endl;
                    std::cout << "  now()                     — current local time → string \"YYYY-MM-DD HH:MM:SS\"" << std::endl;
                    std::cout << "  date_add(ts, days)        — add days to unix timestamp → double" << std::endl;
                    std::cout << "    ts: double (unix timestamp seconds), days: double" << std::endl;
                    std::cout << "    returns: new unix timestamp (ts + days*86400)" << std::endl;
                    std::cout << "  date_diff(t1, t2)         — days between timestamps → double" << std::endl;
                    std::cout << "    t1, t2: double (unix timestamps); returns (t2-t1)/86400" << std::endl;
                    std::cout << "Indirect domain — date arithmetic:" << std::endl;
                    std::cout << "  double now_ts = algo.add(0)   # use fin.date() for calendar dates" << std::endl;
                    std::cout << "  For calendar-aware dates use fin.date(), fin.add_days(), fin.diff_days()" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    std::sort(), std::chrono::system_clock::now()" << std::endl;
                    std::cout << "  Python: sorted(), list.sort(), datetime.now()" << std::endl;
                    std::cout << "  Julia:  sort!(), Dates.now()" << std::endl;
                    std::cout << "  Go:     sort.Slice(), time.Now()" << std::endl;
                    std::cout << "  Ruby:   Array#sort!, Time.now" << std::endl;
                    std::cout << "  Rust:   slice.sort(), std::time::SystemTime::now()" << std::endl;
                } else if (topic == "nlp") {
                    std::cout << "Module: nlp — Natural Language Processing" << std::endl;
                    std::cout << "  Used internally by mobl.dictate() for speech-to-structured-JSON." << std::endl;
                    std::cout << "  Delegates to ai.prompt() for NLP tasks." << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    no built-in NLP (use OpenNLP/CRFsuite via FFI)" << std::endl;
                    std::cout << "  Python: nltk, spacy, transformers (HuggingFace)" << std::endl;
                    std::cout << "  Julia:  TextAnalysis.jl, WordTokenizers.jl" << std::endl;
                    std::cout << "  Go:     jdkato/prose, neurosnap/sentences" << std::endl;
                    std::cout << "  Ruby:   ruby-nlp, treat gem" << std::endl;
                    std::cout << "  Rust:   rust-bert crate (HuggingFace models)" << std::endl;
                } else if (topic == "bev") {
                    std::cout << "Module: bev — Bevington data reduction (scientific curve fitting)" << std::endl;
                    std::cout << "  data(x_vec,y_vec)  — load x/y data into Bevington buffers" << std::endl;
                    std::cout << "  fit_line()         — linear regression (least squares)" << std::endl;
                    std::cout << "  param(idx)         — fitted parameter: 0=intercept, 1=slope" << std::endl;
                    std::cout << "Based on: Bevington & Robinson 'Data Reduction and Error Analysis'" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    Eigen, GSL gsl_fit_linear()" << std::endl;
                    std::cout << "  Python: numpy.polyfit(), scipy.stats.linregress()" << std::endl;
                    std::cout << "  Julia:  GLM.jl lm(@formula(y~x), df)" << std::endl;
                    std::cout << "  Go:     gonum.org/v1/gonum/stat" << std::endl;
                    std::cout << "  Ruby:   statsample gem" << std::endl;
                    std::cout << "  Rust:   linregress crate" << std::endl;
                } else if (topic == "file") {
                    std::cout << "Module: file — File system operations" << std::endl;
                    std::cout << "  write(path,content)  — write string to file (creates/overwrites)" << std::endl;
                    std::cout << "  exists(path)         — true if file or directory exists" << std::endl;
                    std::cout << "Example:" << std::endl;
                    std::cout << "  file.write(\"out.txt\", \"Hello Gem\")" << std::endl;
                    std::cout << "  if file.exists(\"out.txt\") sys.print(\"ok\") end" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    std::ofstream, std::filesystem::exists()" << std::endl;
                    std::cout << "  Python: open('f','w').write(), os.path.exists()" << std::endl;
                    std::cout << "  Julia:  write(\"f\",s), isfile(\"f\")" << std::endl;
                    std::cout << "  Go:     os.WriteFile(), os.Stat()" << std::endl;
                    std::cout << "  Ruby:   File.write(), File.exist?()" << std::endl;
                    std::cout << "  Rust:   std::fs::write(), std::path::Path::exists()" << std::endl;
                } else if (topic == "geo") {
                    std::cout << "Module: geo — GIS, geolocation, and GeoJSON" << std::endl;
                    std::cout << "  lookup()                      — IP geolocation (.lat,.lon,.city,.country)" << std::endl;
                    std::cout << "  distance(lat1,lon1,lat2,lon2) — Haversine distance in km" << std::endl;
                    std::cout << "  write_geojson(path,features)  — write GeoJSON FeatureCollection" << std::endl;
                    std::cout << "  history(plate)                — tectonic plate history (AI-assisted)" << std::endl;
                    std::cout << "  plot2d(data,[layout])         — 2D Plotly map (OpenStreetMap)" << std::endl;
                    std::cout << "  plot3d(data,[layout])         — 3D globe (orthographic scattergeo)" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    GDAL, GEOS, Proj libraries" << std::endl;
                    std::cout << "  Python: geopandas, shapely, folium, geopy" << std::endl;
                    std::cout << "  Julia:  GeoStats.jl, Geodesy.jl" << std::endl;
                    std::cout << "  Go:     paulmach/orb, twpayne/go-geom" << std::endl;
                    std::cout << "  Ruby:   rgeo gem, geocoder gem" << std::endl;
                    std::cout << "  Rust:   geo crate, geojson crate" << std::endl;
                } else if (topic == "mobl") {
                    std::cout << "Module: mobl — Mobile & cross-platform PWA builtin" << std::endl;
                    std::cout << "Platforms: Android Chrome, iPhone Safari, macOS, Linux, Windows 11" << std::endl;
                    std::cout << "  phone(name)                   — instantiate a phone object" << std::endl;
                    std::cout << "  dictate(spoken_text)          — NLP parse → JSON {title,note,tags}" << std::endl;
                    std::cout << "  make_feature(lat,lon,text)    — GPS + dictation → GeoJSON Feature" << std::endl;
                    std::cout << "Architecture: Browser (GPS + Web Speech) → POST /log → Gem NLP → GeoJSON" << std::endl;
                    std::cout << "  /       → PWA HTML (mic + live Plotly map)" << std::endl;
                    std::cout << "  /log    → POST {lat,lon,text} → GeoJSON feature" << std::endl;
                    std::cout << "  /data   → GET full FeatureCollection" << std::endl;
                    std::cout << "Example: mobl phone(\"MyPhone\")  phone.dictate(\"Arrived at Yosemite\")" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    Qt for mobile; no built-in PWA/speech" << std::endl;
                    std::cout << "  Python: Flask + SpeechRecognition + geopy" << std::endl;
                    std::cout << "  Julia:  no built-in mobile/PWA support" << std::endl;
                    std::cout << "  Go:     net/http + custom JS; no built-in speech" << std::endl;
                    std::cout << "  Ruby:   Rails + Web Speech API (custom)" << std::endl;
                    std::cout << "  Rust:   actix-web + custom JS; no built-in speech" << std::endl;
                } else if (topic == "trek") {
                    std::cout << "Module: trek — Travel log creation, display, and editing (OSM/GeoJSON/GPX)" << std::endl;
                    std::cout << "  new(path)                     — create empty GeoJSON travel log" << std::endl;
                    std::cout << "  add(path,lat,lon,[title],[note]) — append waypoint" << std::endl;
                    std::cout << "  edit(path,index,title,note)   — update waypoint at index" << std::endl;
                    std::cout << "  remove(path,index)            — remove waypoint at index" << std::endl;
                    std::cout << "  load(path)                    — return GeoJSON string" << std::endl;
                    std::cout << "  show(path,[port])             — serve OSM/Leaflet map with live edit UI (default :8090)" << std::endl;
                    std::cout << "  export_gpx(geojson,gpx)       — export to GPX format" << std::endl;
                    std::cout << "  stats(path)                   — {.waypoints, .distance_km}" << std::endl;
                    std::cout << "Example:" << std::endl;
                    std::cout << "  trek.new(\"trip.geojson\")" << std::endl;
                    std::cout << "  trek.add(\"trip.geojson\", 48.85, 2.35, \"Paris\", \"Eiffel Tower\")" << std::endl;
                    std::cout << "  trek.show(\"trip.geojson\")   # opens browser with OSM map" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    GDAL + libgpx; no built-in travel log" << std::endl;
                    std::cout << "  Python: gpxpy, folium, geojson packages" << std::endl;
                    std::cout << "  Julia:  no built-in travel log; GPX.jl" << std::endl;
                    std::cout << "  Go:     tkrajina/gpxgo; no built-in" << std::endl;
                    std::cout << "  Ruby:   gpx gem; no built-in" << std::endl;
                    std::cout << "  Rust:   gpx crate; no built-in" << std::endl;
                } else if (topic == "seo") {
                    std::cout << "Module: seo — Search Engine Optimization indexer" << std::endl;
                    std::cout << "  index(urls, output_path)  — crawl list of URLs, extract SEO signals, write JSON index" << std::endl;
                    std::cout << "  analyze(index_path)       — load index JSON and print SEO report to stdout" << std::endl;
                    std::cout << "SEO signals extracted per URL:" << std::endl;
                    std::cout << "  title, meta description, meta keywords, og:title, og:description" << std::endl;
                    std::cout << "  canonical URL, word count, image count, images with alt text" << std::endl;
                    std::cout << "  internal links, external links, h1/h2/h3 headings" << std::endl;
                    std::cout << "Output: JSON array written to output_path, one object per URL." << std::endl;
                    std::cout << "Example:" << std::endl;
                    std::cout << "  seo.index([\"https://example.com\", \"https://example.com/about\"], \"index.json\")" << std::endl;
                    std::cout << "  seo.analyze(\"index.json\")" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    no built-in SEO; use libcurl + custom HTML parser" << std::endl;
                    std::cout << "  Python: scrapy, beautifulsoup4, requests + custom extraction" << std::endl;
                    std::cout << "  Julia:  HTTP.jl + Gumbo.jl for HTML parsing" << std::endl;
                    std::cout << "  Go:     colly, goquery (jQuery-like HTML scraper)" << std::endl;
                    std::cout << "  Ruby:   nokogiri + mechanize gems" << std::endl;
                    std::cout << "  Rust:   scraper crate + reqwest" << std::endl;
                } else if (topic == "art") {
                    std::cout << "Module: art — ASCII art generation, SVG conversion, and doc rendering" << std::endl;
                    std::cout << "  text_to_art(text,[font])  — render text as ASCII art (uses figlet/toilet)" << std::endl;
                    std::cout << "  art_to_file(text,path)    — write ASCII art string to a text file" << std::endl;
                    std::cout << "  art_to_svg(text,[path])   — convert ASCII art to SVG (returns SVG string)" << std::endl;
                    std::cout << "  svg_to_art(svg_path,[w])  — convert SVG file to ASCII art (uses rsvg-convert+jp2a)" << std::endl;
                    std::cout << "  mindmap([path])           — render docs/gem_mindmap.md as ASCII art" << std::endl;
                    std::cout << "  readme([path])            — render README.md headings as ASCII art summary" << std::endl;
                    std::cout << "  tutorial([path])          — render tutorial/README.md as ASCII art summary" << std::endl;
                    std::cout << "Optional deps: figlet, toilet (text→art), mmdc (mermaid), rsvg-convert+jp2a (SVG→art)" << std::endl;
                    std::cout << "Example:" << std::endl;
                    std::cout << "  string banner = art.text_to_art(\"Gem\", \"big\")" << std::endl;
                    std::cout << "  art.art_to_file(banner, \"banner.txt\")" << std::endl;
                    std::cout << "  string svg = art.art_to_svg(banner, \"banner.svg\")" << std::endl;
                    std::cout << "  string ascii = art.svg_to_art(\"docs/gem_mindmap.svg\")" << std::endl;
                    std::cout << "  sys.print(art.mindmap())" << std::endl;
                    std::cout << "  sys.print(art.readme())" << std::endl;
                    std::cout << "  sys.print(art.tutorial())" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    no built-in; use figlet CLI or libaa" << std::endl;
                    std::cout << "  Python: art, pyfiglet, ascii_magic libraries" << std::endl;
                    std::cout << "  Julia:  no built-in" << std::endl;
                    std::cout << "  Go:     no built-in" << std::endl;
                    std::cout << "  Ruby:   artii gem" << std::endl;
                    std::cout << "  Rust:   figlet-rs crate" << std::endl;
                } else if (topic == "www") {
                    std::cout << "Module: www — Web framework (Flask-like, built on cpp-httplib)" << std::endl;
                    std::cout << "  wget(url,file)         — download file via curl" << std::endl;
                    std::cout << "  app(port,[routes])     — background HTTP server; routes: path→HTML string" << std::endl;
                    std::cout << "  redirect(target,[port])— redirect server or return 302 header string" << std::endl;
                    std::cout << "  map2d(geojson,output)  — render 2D map via Mapnik (requires HAS_MAPNIK)" << std::endl;
                    std::cout << "Example:" << std::endl;
                    std::cout << "  www.app(8080, {\"/\": \"<h1>Hello</h1>\", \"/api\": \"{}\"})" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    cpp-httplib, Crow, Drogon, Pistache" << std::endl;
                    std::cout << "  Python: Flask, FastAPI, http.server" << std::endl;
                    std::cout << "  Julia:  HTTP.jl, Genie.jl" << std::endl;
                    std::cout << "  Go:     net/http (built-in), Gin, Echo" << std::endl;
                    std::cout << "  Ruby:   Sinatra, Rails, WEBrick" << std::endl;
                    std::cout << "  Rust:   actix-web, axum, warp, hyper" << std::endl;
                } else if (topic == "cdn") {
                    std::cout << "Module: cdn — Caching reverse-proxy (inherits all www methods)" << std::endl;
                    std::cout << "  start(port, routes_map)  — start proxy; routes: path-prefix->origin URL → bool" << std::endl;
                    std::cout << "    port: int, routes_map: object {path: origin_url}" << std::endl;
                    std::cout << "  stats()                  — cache statistics → object" << std::endl;
                    std::cout << "    returns: .hits, .misses, .bytes, .cached_items (all int)" << std::endl;
                    std::cout << "  purge([path])            — evict path or \"*\" to clear all → bool" << std::endl;
                    std::cout << "  config(type, content)    — generate config string → string" << std::endl;
                    std::cout << "    type: \"nginx\" or \"apache\", content: routes object" << std::endl;
                    std::cout << "  dashboard()              — serve CDN stats dashboard on :8083 → bool" << std::endl;
                    std::cout << "  + all www methods: app, wget, redirect, map2d" << std::endl;
                    std::cout << "Example:" << std::endl;
                    std::cout << "  cdn.start(8081, {\"/api\": \"http://origin:3000\"})" << std::endl;
                    std::cout << "  string s = cdn.stats()" << std::endl;
                    std::cout << "  sys.print(s.hits, s.misses)" << std::endl;
                    std::cout << "  cdn.purge(\"/api/users\")" << std::endl;
                    std::cout << "  cdn.purge(\"*\")   # clear all" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    nginx/Varnish (external); no built-in CDN" << std::endl;
                    std::cout << "  Python: no built-in CDN; use nginx + Flask" << std::endl;
                    std::cout << "  Julia:  no built-in CDN" << std::endl;
                    std::cout << "  Go:     groupcache, caddyserver (external)" << std::endl;
                    std::cout << "  Ruby:   rack-cache middleware" << std::endl;
                    std::cout << "  Rust:   no built-in CDN; use nginx reverse proxy" << std::endl;
                } else if (topic == "thread") {
                    std::cout << "Module: thread — Background thread handle (returned by sys.async())" << std::endl;
                    std::cout << "  wait()        — block until complete; returns result or timeout error" << std::endl;
                    std::cout << "  is_finished() — non-blocking bool status check" << std::endl;
                    std::cout << "Example:" << std::endl;
                    std::cout << "  string t = sys.async(\"my_func\", [arg1], 30.0)" << std::endl;
                    std::cout << "  while !t.is_finished() sys.print(\"waiting...\") end" << std::endl;
                    std::cout << "  string result = t.wait()" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    std::future/std::async, std::thread + join()" << std::endl;
                    std::cout << "  Python: concurrent.futures.Future, threading.Thread" << std::endl;
                    std::cout << "  Julia:  @async / @spawn returns Task; fetch(task)" << std::endl;
                    std::cout << "  Go:     goroutine + channel; no explicit thread handle" << std::endl;
                    std::cout << "  Ruby:   Thread.new { ... }; thread.join; thread.alive?" << std::endl;
                    std::cout << "  Rust:   std::thread::spawn returns JoinHandle; handle.join()" << std::endl;
                } else if (topic == "cpp") {
                    std::cout << "Module: cpp — C++26 JIT interop via Cling" << std::endl;
                    std::cout << "  repl()       — start interactive C++26 REPL (Cling)" << std::endl;
                    std::cout << "  exec(code)   — execute C++ code string directly via Cling JIT" << std::endl;
                    std::cout << "Example:" << std::endl;
                    std::cout << "  cpp.exec(\"#include <iostream>\\nstd::cout << 42;\")" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    native (this IS C++)" << std::endl;
                    std::cout << "  Python: ctypes, cffi, pybind11, Cython for C++ interop" << std::endl;
                    std::cout << "  Julia:  ccall(), @cxx macro (Cxx.jl)" << std::endl;
                    std::cout << "  Go:     cgo for C interop; no direct C++ JIT" << std::endl;
                    std::cout << "  Ruby:   RubyInline, fiddle for C; no C++ JIT" << std::endl;
                    std::cout << "  Rust:   bindgen for C/C++ FFI; no JIT" << std::endl;
                } else if (topic == "itr") {
                    std::cout << "Module: itr — Iterators, ranges, and functional loops" << std::endl;
                    std::cout << "  range(n)              — vector [0,1,...,n-1]" << std::endl;
                    std::cout << "  while(cond_fun,body_fun) — functional while loop" << std::endl;
                    std::cout << "Example:" << std::endl;
                    std::cout << "  string v = itr.range(5)   # [0,1,2,3,4]" << std::endl;
                    std::cout << "  itr.while(fun() i<10 end, fun() i=i+1 end)" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    std::ranges::iota_view, std::views::iota (C++20)" << std::endl;
                    std::cout << "  Python: range(n), itertools" << std::endl;
                    std::cout << "  Julia:  1:n, eachindex(), Iterators.take()" << std::endl;
                    std::cout << "  Go:     for i := range n { } (Go 1.22+)" << std::endl;
                    std::cout << "  Ruby:   (0...n).each, Enumerator" << std::endl;
                    std::cout << "  Rust:   (0..n), Iterator trait, .map()/.filter()" << std::endl;
                } else if (topic == "zip") {
                    std::cout << "Module: zip — File compression/decompression (miniz/libzip)" << std::endl;
                    std::cout << "  compress(src,archive)    — zip files/folders to archive" << std::endl;
                    std::cout << "  decompress(archive,dest) — unzip archive to destination" << std::endl;
                    std::cout << "Example:" << std::endl;
                    std::cout << "  zip.compress(\"mydir\", \"mydir.zip\")" << std::endl;
                    std::cout << "  zip.decompress(\"mydir.zip\", \"output/\")" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    libzip, miniz, zlib" << std::endl;
                    std::cout << "  Python: zipfile stdlib, shutil.make_archive()" << std::endl;
                    std::cout << "  Julia:  ZipFile.jl" << std::endl;
                    std::cout << "  Go:     archive/zip stdlib" << std::endl;
                    std::cout << "  Ruby:   zip gem (rubyzip)" << std::endl;
                    std::cout << "  Rust:   zip crate" << std::endl;
                } else if (topic == "chart") {
                    std::cout << "Module: chart — Interactive 2D/3D plotting (Plotly.js)" << std::endl;
                    std::cout << "  plot(traces,[layout])  — generate interactive HTML chart" << std::endl;
                    std::cout << "  show(path)             — open chart HTML in browser" << std::endl;
                    std::cout << "  server(path,[port])    — serve chart via background HTTP server" << std::endl;
                    std::cout << "Example:" << std::endl;
                    std::cout << "  string c = chart.plot([{x:[1,2,3],y:[4,5,6],type:\"scatter\"}])" << std::endl;
                    std::cout << "  chart.show(c)" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    matplotlib-cpp, gnuplot-iostream, ROOT" << std::endl;
                    std::cout << "  Python: plotly, matplotlib, seaborn, bokeh" << std::endl;
                    std::cout << "  Julia:  Plots.jl, Makie.jl, PlotlyJS.jl" << std::endl;
                    std::cout << "  Go:     gonum/plot, go-echarts" << std::endl;
                    std::cout << "  Ruby:   gruff, gnuplot gem" << std::endl;
                    std::cout << "  Rust:   plotters crate, charming crate" << std::endl;
                } else if (topic == "fun") {
                    std::cout << "Keyword: fun — Define a named function" << std::endl;
                    std::cout << "Usage:   fun name(p1, p2) ... end  [last expr is return value]" << std::endl;
                    std::cout << "Example: fun add(a, b) a + b end" << std::endl;
                    std::cout << "         fun fact(n) if n < 2 1 else n * fact(n-1) end end" << std::endl;
                    std::cout << "Notes:   First-class; supports recursion and closures." << std::endl;
                    std::cout << "         No explicit 'return' needed — last expression is returned." << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    int add(int a, int b) { return a + b; }" << std::endl;
                    std::cout << "  Python: def add(a, b): return a + b" << std::endl;
                    std::cout << "  Julia:  add(a, b) = a + b" << std::endl;
                    std::cout << "  Go:     func add(a, b int) int { return a + b }" << std::endl;
                    std::cout << "  Ruby:   def add(a, b) = a + b" << std::endl;
                    std::cout << "  Rust:   fn add(a: i64, b: i64) -> i64 { a + b }" << std::endl;
                } else if (topic == "obj") {
                    std::cout << "Keyword: obj — Define an object (class) with single inheritance" << std::endl;
                    std::cout << "Usage:   obj Name(params) : Parent ... end" << std::endl;
                    std::cout << "         obj Name(params) ... end   # inherits from sys by default" << std::endl;
                    std::cout << "Example:" << std::endl;
                    std::cout << "  obj Animal(name)" << std::endl;
                    std::cout << "    .name = name" << std::endl;
                    std::cout << "    fun speak() sys.print(\"...\")" << std::endl;
                    std::cout << "    end" << std::endl;
                    std::cout << "  end" << std::endl;
                    std::cout << "  obj Dog(name) : Animal" << std::endl;
                    std::cout << "    .name = name" << std::endl;
                    std::cout << "    fun speak() sys.print(\"Woof!\") end" << std::endl;
                    std::cout << "    fun parent_name() ..name end   # ..attr accesses parent attr" << std::endl;
                    std::cout << "  end" << std::endl;
                    std::cout << "  Dog d = Dog(\"Rex\")" << std::endl;
                    std::cout << "  d.speak()          # calls Dog.speak" << std::endl;
                    std::cout << "  sys.print(d.name)  # access attribute" << std::endl;
                    std::cout << "Attribute syntax:" << std::endl;
                    std::cout << "  .attr = val    — set/get own attribute (this.attr)" << std::endl;
                    std::cout << "  ..attr = val   — set/get parent attribute" << std::endl;
                    std::cout << "  obj.attr       — access from outside" << std::endl;
                    std::cout << "Method definition inside obj:" << std::endl;
                    std::cout << "  fun method(params) ... end   — last expr is return value" << std::endl;
                    std::cout << "Instantiation:" << std::endl;
                    std::cout << "  TypeName varname = TypeName(args)" << std::endl;
                    std::cout << "  string varname = TypeName(args)  # also valid (untyped)" << std::endl;
                    std::cout << "Notes:   All objects auto-inherit from sys (gives sys.print etc)." << std::endl;
                    std::cout << "         Single inheritance only. For multiple: chain obj C:B where obj B:A." << std::endl;
                    std::cout << "         Anonymous object: obj(parent) — creates inline object." << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    class Dog : public Animal { ... };" << std::endl;
                    std::cout << "  Python: class Dog(Animal): ..." << std::endl;
                    std::cout << "  Julia:  struct Dog <: Animal; ... end" << std::endl;
                    std::cout << "  Go:     type Dog struct { ... }  (embedding, not inheritance)" << std::endl;
                    std::cout << "  Ruby:   class Dog < Animal; ... end" << std::endl;
                    std::cout << "  Rust:   struct Dog { ... }  (traits, no inheritance)" << std::endl;
                } else if (topic == "use") {
                    std::cout << "Keyword: use — Include a Gem module or run foreign code" << std::endl;
                    std::cout << "Usage:   use \"file.g\"      # include Gem source" << std::endl;
                    std::cout << "         use \"file.gm\"     # include Gem module" << std::endl;
                    std::cout << "         use \"file.py\"     # AI-translate Python → Gem and run" << std::endl;
                    std::cout << "         use \"file.jl\"     # AI-translate Julia → Gem and run" << std::endl;
                    std::cout << "         use \"file.r\"      # AI-translate R → Gem and run" << std::endl;
                    std::cout << "         use \"file.cpp\"    # AI-translate C++ → Gem and run" << std::endl;
                    std::cout << "         use \"file.go\"     # AI-translate Go → Gem and run" << std::endl;
                    std::cout << "         use \"file.rb\"     # AI-translate Ruby → Gem and run" << std::endl;
                    std::cout << "         use \"file.rs\"     # AI-translate Rust → Gem and run" << std::endl;
                    std::cout << "         use \"file.js\"     # AI-translate Node.js → Gem and run" << std::endl;
                    std::cout << "Notes:   Foreign files are AI-translated to Gem on-the-fly via ai.prompt()." << std::endl;
                    std::cout << "         Use langport(\"*.py\", \"out.gm\") to save the translation." << std::endl;
                    std::cout << "         For direct execution without translation use go.run(), ruby.run(), etc." << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    #include \"file.h\"  /  import (C++20 modules)" << std::endl;
                    std::cout << "  Python: import module  /  from module import x" << std::endl;
                    std::cout << "  Julia:  include(\"file.jl\")  /  using Module" << std::endl;
                    std::cout << "  Go:     import \"package\"" << std::endl;
                    std::cout << "  Ruby:   require 'file'  /  require_relative 'file'" << std::endl;
                    std::cout << "  Rust:   mod module;  /  use crate::module;" << std::endl;
                } else if (topic == "if") {
                    std::cout << "Keyword: if — Conditional execution" << std::endl;
                    std::cout << "Usage:   if condition ... end" << std::endl;
                    std::cout << "         if condition ... else ... end" << std::endl;
                    std::cout << "Example: if x > 0" << std::endl;
                    std::cout << "           sys.print(\"positive\")" << std::endl;
                    std::cout << "         else" << std::endl;
                    std::cout << "           sys.print(\"non-positive\")" << std::endl;
                    std::cout << "         end" << std::endl;
                    std::cout << "Notes:   Condition is truthy if non-zero, non-empty, or boolean true." << std::endl;
                    std::cout << "         No parentheses required around condition." << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    if (x > 0) { ... } else { ... }" << std::endl;
                    std::cout << "  Python: if x > 0: ... else: ..." << std::endl;
                    std::cout << "  Julia:  if x > 0 ... else ... end" << std::endl;
                    std::cout << "  Go:     if x > 0 { ... } else { ... }" << std::endl;
                    std::cout << "  Ruby:   if x > 0 ... else ... end" << std::endl;
                    std::cout << "  Rust:   if x > 0 { ... } else { ... }" << std::endl;
                } else if (topic == "while") {
                    std::cout << "Keyword: while — Loop while condition is true" << std::endl;
                    std::cout << "Usage:   while condition ... end" << std::endl;
                    std::cout << "Example: int i = 0" << std::endl;
                    std::cout << "         while i < 10" << std::endl;
                    std::cout << "           sys.print(i)" << std::endl;
                    std::cout << "           i = i + 1" << std::endl;
                    std::cout << "         end" << std::endl;
                    std::cout << "Notes:   For range-based iteration use itr.range(n)." << std::endl;
                    std::cout << "         Functional loops: itr.while(cond_fun, body_fun)." << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    while (i < 10) { ... }  /  for (int i=0; i<10; i++) { ... }" << std::endl;
                    std::cout << "  Python: while i < 10: ...  /  for i in range(10): ..." << std::endl;
                    std::cout << "  Julia:  while i < 10 ... end  /  for i in 1:10 ... end" << std::endl;
                    std::cout << "  Go:     for i < 10 { ... }  /  for i := 0; i < 10; i++ { ... }" << std::endl;
                    std::cout << "  Ruby:   while i < 10 do ... end  /  10.times { ... }" << std::endl;
                    std::cout << "  Rust:   while i < 10 { ... }  /  for i in 0..10 { ... }" << std::endl;
                } else if (topic == "int" || topic == "double" || topic == "string" || topic == "bool") {
                    std::cout << "Keyword: " << topic << " — Declare and initialize a typed variable" << std::endl;
                    std::cout << "Usage:   " << topic << " var = value" << std::endl;
                    std::cout << "         " << topic << " a, b, c = value   # multi-var: all get same value" << std::endl;
                    std::cout << "         a = b = c = 0                    # chained assignment" << std::endl;
                    std::cout << "Examples:" << std::endl;
                    std::cout << "  int x = 42" << std::endl;
                    std::cout << "  int a, b, c = 0          # declares a, b, c all = 0" << std::endl;
                    std::cout << "  double pi = 3.14159" << std::endl;
                    std::cout << "  string name = \"Gem\"" << std::endl;
                    std::cout << "  bool flag = true" << std::endl;
                    std::cout << "  a = b = c = 0            # chained: sets a, b, c to 0" << std::endl;
                    std::cout << "Notes:   All variables MUST be initialized at declaration." << std::endl;
                    std::cout << "         Global scope: prefix with _ (e.g. _config = 1)." << std::endl;
                    std::cout << "         Local scope: all other names." << std::endl;
                    std::cout << "         int can also hold a vector: int v = [1.0, 2.0, 3.0]" << std::endl;
                    std::cout << "         Access vector elements: v[0], v[1], ..." << std::endl;
                    std::cout << "         Vector length: v.length  or  v.size  or  v.count" << std::endl;
                    std::cout << "         See also: help \"vectors\"" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    int x = 42;  double pi = 3.14;  std::string s = \"hi\";  bool b = true;" << std::endl;
                    std::cout << "  Python: x = 42  (dynamic; no declaration needed)" << std::endl;
                    std::cout << "  Julia:  x::Int = 42  (optional annotation; dynamic by default)" << std::endl;
                    std::cout << "  Go:     x := 42  /  var x int = 42" << std::endl;
                    std::cout << "  Ruby:   x = 42  (dynamic; no declaration needed)" << std::endl;
                    std::cout << "  Rust:   let x: i64 = 42;  let s: String = String::from(\"hi\");" << std::endl;
                } else if (topic == "alias") {
                    std::cout << "Keyword: alias — Create a REPL shortcut" << std::endl;
                    std::cout << "Usage:   alias shortcut = expansion" << std::endl;
                    std::cout << "Example: alias ? = sys.help()" << std::endl;
                    std::cout << "         alias h = sys.help" << std::endl;
                    std::cout << "Notes:   Aliases are REPL-only; not available in scripts." << std::endl;
                    std::cout << "         Useful for frequently-used commands." << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    #define ALIAS expr  /  using alias = type;" << std::endl;
                    std::cout << "  Python: alias = lambda: expr  (no built-in alias)" << std::endl;
                    std::cout << "  Julia:  const alias = expr" << std::endl;
                    std::cout << "  Go:     type Alias = OriginalType  (type aliases only)" << std::endl;
                    std::cout << "  Ruby:   alias new_name old_name  (method aliases)" << std::endl;
                    std::cout << "  Rust:   type Alias = OriginalType;" << std::endl;
                } else if (topic == "his") {
                    std::cout << "Keyword: his — View session history" << std::endl;
                    std::cout << "Usage:   his" << std::endl;
                    std::cout << "Notes:   Displays today's REPL session log (~/.gem/YYYYDDD.txt)." << std::endl;
                    std::cout << "         Prompts to view prior days." << std::endl;
                    std::cout << "         CLI equivalent: gem -h" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  Bash:   history" << std::endl;
                    std::cout << "  Python: readline history via readline module" << std::endl;
                    std::cout << "  Julia:  REPL history in ~/.julia/logs/repl_history.jl" << std::endl;
                    std::cout << "  (No direct equivalent in Go/Ruby/Rust REPLs)" << std::endl;
                } else if (topic == "lib") {
                    std::cout << "Keyword: lib — List loaded builtin modules" << std::endl;
                    std::cout << "Usage:   lib" << std::endl;
                    std::cout << "Notes:   Shows all registered builtins: sys, math, ai, text, rex, ..." << std::endl;
                    std::cout << "         Use 'help \"module\"' for details on any listed module." << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  Python: help('modules')  /  sys.modules" << std::endl;
                    std::cout << "  Julia:  Base.loaded_modules" << std::endl;
                    std::cout << "  Go:     go list (CLI)" << std::endl;
                    std::cout << "  Ruby:   $LOADED_FEATURES" << std::endl;
                    std::cout << "  Rust:   cargo metadata (CLI)" << std::endl;
                } else if (topic == "exit") {
                    std::cout << "Keyword: exit — Exit the Gem interpreter or script" << std::endl;
                    std::cout << "Usage:   exit  /  sys.exit()" << std::endl;
                    std::cout << "Notes:   In REPL: terminates the session." << std::endl;
                    std::cout << "         In script: terminates execution immediately." << std::endl;
                    std::cout << "         Also triggered by Ctrl+D (EOF) in REPL." << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    std::exit(0);  /  return 0; (from main)" << std::endl;
                    std::cout << "  Python: sys.exit(0)  /  exit()" << std::endl;
                    std::cout << "  Julia:  exit(0)" << std::endl;
                    std::cout << "  Go:     os.Exit(0)" << std::endl;
                    std::cout << "  Ruby:   exit(0)  /  Kernel.exit" << std::endl;
                    std::cout << "  Rust:   std::process::exit(0);" << std::endl;
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
                } else if (topic == "end") {
                    std::cout << "Keyword: end — Close a block (fun, obj, if, while)" << std::endl;
                    std::cout << "Usage:   end  (no arguments)" << std::endl;
                    std::cout << "Example: fun add(a,b) a+b end" << std::endl;
                    std::cout << "         if x > 0 sys.print(x) end" << std::endl;
                    std::cout << "         while i < 10 i = i+1 end" << std::endl;
                    std::cout << "         obj Dog(name) .name=name end" << std::endl;
                    std::cout << "Notes:   Every fun/obj/if/while block MUST be closed with end." << std::endl;
                    std::cout << "         No braces or indentation-based scoping — end is mandatory." << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    closing brace }" << std::endl;
                    std::cout << "  Python: indentation (no explicit close)" << std::endl;
                    std::cout << "  Julia:  end (same)" << std::endl;
                    std::cout << "  Go:     closing brace }" << std::endl;
                    std::cout << "  Ruby:   end (same)" << std::endl;
                    std::cout << "  Rust:   closing brace }" << std::endl;
                } else if (topic == "langport") {
                    std::cout << "Function: langport — AI-port foreign code files to Gem" << std::endl;
                    std::cout << "Usage:   langport(pattern, [output])" << std::endl;
                    std::cout << "         sys.langport(pattern, [output])" << std::endl;
                    std::cout << "Example: langport(\"*.py\", \"out.gm\")   # port all .py files" << std::endl;
                    std::cout << "         langport(\"script.jl\")          # print ported code" << std::endl;
                    std::cout << "Notes:   Uses ai.prompt() to translate source to Gem syntax." << std::endl;
                    std::cout << "         Supports: .py .jl .r .cpp .go .rb .rs .js" << std::endl;
                    std::cout << "         'use' runs translation in-memory; langport saves to file." << std::endl;
                    std::cout << "         CLI: gem -t <file> [-o output]" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    no built-in; use transpilers (e.g. cppfront)" << std::endl;
                    std::cout << "  Python: no built-in; use 2to3 for Py2→Py3" << std::endl;
                    std::cout << "  Julia:  no built-in; manual porting" << std::endl;
                    std::cout << "  Go:     no built-in; use gofmt for formatting only" << std::endl;
                    std::cout << "  Ruby:   no built-in" << std::endl;
                    std::cout << "  Rust:   no built-in; c2rust for C→Rust" << std::endl;
                } else if (topic == "else") {
                    std::cout << "Keyword: else — Optional alternate branch for if" << std::endl;
                    std::cout << "Usage:   if condition ... else ... end" << std::endl;
                    std::cout << "Example: if x > 0" << std::endl;
                    std::cout << "           sys.print(\"positive\")" << std::endl;
                    std::cout << "         else" << std::endl;
                    std::cout << "           sys.print(\"non-positive\")" << std::endl;
                    std::cout << "         end" << std::endl;
                    std::cout << "Notes:   else must appear between if and end on its own line." << std::endl;
                    std::cout << "         Nested if/else/end blocks are fully supported." << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    } else {" << std::endl;
                    std::cout << "  Python: else:" << std::endl;
                    std::cout << "  Julia:  else" << std::endl;
                    std::cout << "  Go:     } else {" << std::endl;
                    std::cout << "  Ruby:   else" << std::endl;
                    std::cout << "  Rust:   } else {" << std::endl;
                } else if (topic == "true" || topic == "false") {
                    std::cout << "Literal: " << topic << " — Boolean literal value" << std::endl;
                    std::cout << "Usage:   bool flag = true   /   bool done = false" << std::endl;
                    std::cout << "Example: bool ok = true" << std::endl;
                    std::cout << "         if ok sys.print(\"yes\") end" << std::endl;
                    std::cout << "Notes:   Truthy: non-zero numbers, non-empty strings, true." << std::endl;
                    std::cout << "         Falsy:  0, \"\", false, null." << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    true / false (bool)" << std::endl;
                    std::cout << "  Python: True / False" << std::endl;
                    std::cout << "  Julia:  true / false" << std::endl;
                    std::cout << "  Go:     true / false" << std::endl;
                    std::cout << "  Ruby:   true / false" << std::endl;
                    std::cout << "  Rust:   true / false" << std::endl;
                } else if (topic == "null") {
                    std::cout << "Literal: null — Null/void value (no value)" << std::endl;
                    std::cout << "Usage:   Returned by functions with no return value." << std::endl;
                    std::cout << "         Uninitialized object properties resolve to null." << std::endl;
                    std::cout << "Notes:   null is falsy in boolean context." << std::endl;
                    std::cout << "         Comparing with == null checks for absence of value." << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    nullptr / void" << std::endl;
                    std::cout << "  Python: None" << std::endl;
                    std::cout << "  Julia:  nothing" << std::endl;
                    std::cout << "  Go:     nil" << std::endl;
                    std::cout << "  Ruby:   nil" << std::endl;
                    std::cout << "  Rust:   None (Option<T>)" << std::endl;
                } else if (topic == "quit") {
                    std::cout << "Keyword: quit — Alias for exit; terminates the interpreter" << std::endl;
                    std::cout << "Usage:   quit  /  exit  /  sys.exit()" << std::endl;
                    std::cout << "Notes:   All three forms are equivalent." << std::endl;
                    std::cout << "         In REPL: ends the session." << std::endl;
                    std::cout << "         In script: terminates execution immediately." << std::endl;
                    std::cout << "         Also triggered by Ctrl+D (EOF) in REPL." << std::endl;
                } else if (topic == "operators" || topic == "+=" || topic == "-=" || topic == "*=" || topic == "/=") {
                    std::cout << "Operators — Gem operator reference" << std::endl;
                    std::cout << "Arithmetic:   +  -  *  /" << std::endl;
                    std::cout << "  int x = 10; x = x + 5   # x is 15" << std::endl;
                    std::cout << "Compound assignment:" << std::endl;
                    std::cout << "  x += 5  x -= 3  x *= 2  x /= 4  s += \" more\"" << std::endl;
                    std::cout << "Comparison (numeric and string):" << std::endl;
                    std::cout << "  ==  !=  >  >=  <  <=" << std::endl;
                    std::cout << "  if x >= 10 sys.print(\"big\") end" << std::endl;
                    std::cout << "  if s != \"done\" sys.print(\"still going\") end" << std::endl;
                    std::cout << "Logical:" << std::endl;
                    std::cout << "  !expr        — prefix not (bool negation)" << std::endl;
                    std::cout << "  a && b       — logical and (short-circuit)" << std::endl;
                    std::cout << "  a || b       — logical or  (short-circuit)" << std::endl;
                    std::cout << "  if x > 0 && x < 10 sys.print(\"single digit\") end" << std::endl;
                    std::cout << "  if !done || retry sys.print(\"keep going\") end" << std::endl;
                    std::cout << "Statement separator:  ;" << std::endl;
                    std::cout << "  int a = 1; int b = 2; sys.print(a + b)" << std::endl;
                    std::cout << "String concatenation:  +" << std::endl;
                    std::cout << "  string s = \"Hello\" + \" \" + \"World\"" << std::endl;
                } else if (topic == "nil") {
                    std::cout << "Literal: nil — Null/void value (alias for null)" << std::endl;
                    std::cout << "Usage:   nil  (same as null)" << std::endl;
                    std::cout << "Example: if isnil(x) sys.print(\"no value\") end" << std::endl;
                    std::cout << "Notes:   nil and null are identical — both produce a void GemValue." << std::endl;
                    std::cout << "         Falsy in boolean context." << std::endl;
                    std::cout << "         Use isnil(x) to test for nil/null." << std::endl;
                    std::cout << "See also: isnil(), null, nan" << std::endl;
                } else if (topic == "nan") {
                    std::cout << "Literal: nan — IEEE 754 Not-a-Number double value" << std::endl;
                    std::cout << "Usage:   double x = nan" << std::endl;
                    std::cout << "Example: double x = nan" << std::endl;
                    std::cout << "         if isnan(x) sys.print(\"not a number\") end" << std::endl;
                    std::cout << "Notes:   nan != nan is true (IEEE 754 rule)." << std::endl;
                    std::cout << "         Use isnan(x) to test for NaN — do not use x == nan." << std::endl;
                    std::cout << "         Produced by: 0.0/0.0, tonum(\"abc\"), etc." << std::endl;
                    std::cout << "See also: isnan(), nil, null" << std::endl;
                } else if (topic == "isnil") {
                    std::cout << "Function: isnil(x) — test if value is nil/null/void" << std::endl;
                    std::cout << "Params:   x — any value" << std::endl;
                    std::cout << "Returns:  bool — true if x is nil/null, false otherwise" << std::endl;
                    std::cout << "Example:  if isnil(result) sys.print(\"no result\") end" << std::endl;
                    std::cout << "Notes:    isnil(nil) == true" << std::endl;
                    std::cout << "          isnil(null) == true" << std::endl;
                    std::cout << "          isnil(0) == false  (0 is a valid double, not nil)" << std::endl;
                    std::cout << "          isnil(\"\") == false  (empty string is not nil)" << std::endl;
                    std::cout << "See also: nil, null, isnan()" << std::endl;
                } else if (topic == "isnan") {
                    std::cout << "Function: isnan(x) — test if value is IEEE 754 NaN" << std::endl;
                    std::cout << "Params:   x — any value (non-double returns false)" << std::endl;
                    std::cout << "Returns:  bool — true if x is NaN, false otherwise" << std::endl;
                    std::cout << "Example:  double v = tonum(\"abc\")" << std::endl;
                    std::cout << "          if isnan(v) sys.print(\"parse failed\") end" << std::endl;
                    std::cout << "Notes:    isnan(nan) == true" << std::endl;
                    std::cout << "          isnan(0.0) == false" << std::endl;
                    std::cout << "          isnan(\"text\") == false  (non-numeric returns false)" << std::endl;
                    std::cout << "See also: nan, isnil(), tonum()" << std::endl;
                } else if (topic == "tonum") {
                    std::cout << "Function: tonum(x) — convert value to double" << std::endl;
                    std::cout << "Params:   x — string, bool, double, or nil" << std::endl;
                    std::cout << "Returns:  double" << std::endl;
                    std::cout << "  tonum(\"3.14\")  -> 3.14" << std::endl;
                    std::cout << "  tonum(\"42\")    -> 42.0" << std::endl;
                    std::cout << "  tonum(true)    -> 1.0" << std::endl;
                    std::cout << "  tonum(false)   -> 0.0" << std::endl;
                    std::cout << "  tonum(nil)     -> 0.0" << std::endl;
                    std::cout << "  tonum(\"abc\")   -> nan  (parse failure)" << std::endl;
                    std::cout << "  tonum(3.14)    -> 3.14  (passthrough)" << std::endl;
                    std::cout << "Example:  double n = tonum(sys.args()[1])  # parse CLI arg" << std::endl;
                    std::cout << "See also: tostr(), isnan(), nan" << std::endl;
                } else if (topic == "tostr") {
                    std::cout << "Function: tostr(x) — convert value to string" << std::endl;
                    std::cout << "Params:   x — any value" << std::endl;
                    std::cout << "Returns:  string" << std::endl;
                    std::cout << "  tostr(42.0)    -> \"42.000000\"" << std::endl;
                    std::cout << "  tostr(true)    -> \"true\"" << std::endl;
                    std::cout << "  tostr(false)   -> \"false\"" << std::endl;
                    std::cout << "  tostr(nil)     -> \"\"" << std::endl;
                    std::cout << "  tostr(\"hi\")    -> \"hi\"  (passthrough)" << std::endl;
                    std::cout << "Example:  string s = tostr(x) + \" items\"" << std::endl;
                    std::cout << "See also: tonum(), sys.print()" << std::endl;
                } else if (topic == ";") {
                    std::cout << "Operator: ; — Statement separator (multiple statements per line)" << std::endl;
                    std::cout << "Usage:   stmt1; stmt2; stmt3" << std::endl;
                    std::cout << "Example: int a = 1; int b = 2; sys.print(a + b)" << std::endl;
                    std::cout << "Notes:   Semicolons are optional at end of line." << std::endl;
                    std::cout << "         Useful for compact one-liners in REPL." << std::endl;
                } else if (topic == "vectors" || topic == "vector" || topic == "array") {
                    std::cout << "Vectors — Gem's built-in dynamic array type" << std::endl;
                    std::cout << "Creation:" << std::endl;
                    std::cout << "  int v = [1.0, 2.0, 3.0]       # numeric vector" << std::endl;
                    std::cout << "  string s = [\"a\", \"b\", \"c\"]    # string vector" << std::endl;
                    std::cout << "  int empty = []                 # empty numeric vector" << std::endl;
                    std::cout << "Indexing (0-based):" << std::endl;
                    std::cout << "  v[0]   # first element" << std::endl;
                    std::cout << "  v[2]   # third element" << std::endl;
                    std::cout << "  s[0]   # first string" << std::endl;
                    std::cout << "  str[1] # second character of a string" << std::endl;
                    std::cout << "Length:" << std::endl;
                    std::cout << "  v.length   # number of elements (also .size, .count)" << std::endl;
                    std::cout << "  v.size     # same as .length" << std::endl;
                    std::cout << "  v.count    # same as .length" << std::endl;
                    std::cout << "Sorting (algo module):" << std::endl;
                    std::cout << "  algo.quicksort(v)          # sort in-place, returns sorted vector" << std::endl;
                    std::cout << "  algo.sort(v, [start], [end]) # sort a slice" << std::endl;
                    std::cout << "Iteration:" << std::endl;
                    std::cout << "  int i = 0" << std::endl;
                    std::cout << "  while i < v.length" << std::endl;
                    std::cout << "    sys.print(v[i])" << std::endl;
                    std::cout << "    i += 1" << std::endl;
                    std::cout << "  end" << std::endl;
                    std::cout << "  itr.range(5)   # returns [0,1,2,3,4]" << std::endl;
                    std::cout << "Object vectors:" << std::endl;
                    std::cout << "  tcp.nic()    # returns vector of NIC objects (.name, .ip, .status)" << std::endl;
                    std::cout << "  tcp.routes() # returns vector of Route objects (.dest, .gw, .iface)" << std::endl;
                    std::cout << "  data.read_csv(path) # returns vector of row objects" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  C++:    std::vector<double>, std::vector<std::string>" << std::endl;
                    std::cout << "  Python: list  [1, 2, 3]" << std::endl;
                    std::cout << "  Julia:  Vector{Float64}  [1.0, 2.0, 3.0]" << std::endl;
                    std::cout << "  Go:     []float64{1, 2, 3}" << std::endl;
                    std::cout << "  Ruby:   Array  [1, 2, 3]" << std::endl;
                    std::cout << "  Rust:   Vec<f64>  vec![1.0, 2.0, 3.0]" << std::endl;
                } else if (topic == "repl") {
                    std::cout << "REPL — Gem Interactive Read-Eval-Print Loop" << std::endl;
                    std::cout << "Start:   ./gem          # enter REPL" << std::endl;
                    std::cout << "Exit:    exit  or  quit  or  Ctrl+D" << std::endl;
                    std::cout << "Expression output:" << std::endl;
                    std::cout << "  Any expression that produces a value prints: => value" << std::endl;
                    std::cout << "  Example: 2 + 3        => 5" << std::endl;
                    std::cout << "  Example: \"hello\"      => hello" << std::endl;
                    std::cout << "  Example: math.sin(0)  => 0" << std::endl;
                    std::cout << "History:" << std::endl;
                    std::cout << "  Arrow keys navigate history (readline)" << std::endl;
                    std::cout << "  his          — show today's session log" << std::endl;
                    std::cout << "  gem -h       — print history and exit" << std::endl;
                    std::cout << "  Saved to: ~/.gem/YYYYDDD.txt (Julian date)" << std::endl;
                    std::cout << "  Readline history: ~/.gem/history.txt" << std::endl;
                    std::cout << "Help system:" << std::endl;
                    std::cout << "  help              — general help" << std::endl;
                    std::cout << "  help \"topic\"      — detailed help for topic" << std::endl;
                    std::cout << "  help(\"topic\")     — same, function call form" << std::endl;
                    std::cout << "  sys.help(\"topic\") — same, explicit form" << std::endl;
                    std::cout << "  helpfull          — write full reference to helpfull_DATETIME.md" << std::endl;
                    std::cout << "  sys.helpfull()    — same, method call form" << std::endl;
                    std::cout << "  helpless          — write + pipe through less" << std::endl;
                    std::cout << "  sys.helpless()    — same, method call form" << std::endl;
                    std::cout << "Module listing:" << std::endl;
                    std::cout << "  lib   — list all loaded builtin modules" << std::endl;
                    std::cout << "Aliases:" << std::endl;
                    std::cout << "  alias ? = sys.help()   # define shortcut" << std::endl;
                    std::cout << "  ?                      # invoke alias" << std::endl;
                    std::cout << "Multi-statement lines:" << std::endl;
                    std::cout << "  int a = 1; int b = 2; sys.print(a + b)" << std::endl;
                    std::cout << "C++ REPL:" << std::endl;
                    std::cout << "  cpp.repl()   # drop into Cling C++26 REPL" << std::endl;
                    std::cout << "Comparisons:" << std::endl;
                    std::cout << "  Python: python3 (interactive)" << std::endl;
                    std::cout << "  Julia:  julia (interactive)" << std::endl;
                    std::cout << "  Go:     no built-in REPL" << std::endl;
                    std::cout << "  Ruby:   irb" << std::endl;
                    std::cout << "  Rust:   no built-in REPL (evcxr external)" << std::endl;
                } else if (topic == "all") {
                    // Generate helpfull_DATETIME.md
                    auto t = std::time(nullptr);
                    auto tm = *std::localtime(&t);
                    std::ostringstream ts;
                    ts << std::put_time(&tm, "%Y%m%d_%H%M%S");
                    std::string filename = "helpfull_" + ts.str() + ".md";

                    std::ostringstream md;
                    md << "# Gem Language — Complete Reference\n\n";
                    md << "**Generated:** " << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "  \n";
                    md << "**Version:** 0.1.0 | **Built with:** C++26\n\n---\n\n";

                    md << "## Keywords\n\n";
                    md << "| Keyword | Purpose |\n|---------|----------|\n";
                    md << "| `fun name(p1,p2) ... end` | Define function; last expr is return value |\n";
                    md << "| `obj Name(params) : Parent ... end` | Define object/class (single inheritance) |\n";
                    md << "| `use \"file\"` | Include .g/.gm or AI-translate .py/.jl/.r/.cpp/.go/.rb/.rs/.js |\n";
                    md << "| `if cond ... else ... end` | Conditional (else optional) |\n";
                    md << "| `while cond ... end` | Loop while condition is truthy |\n";
                    md << "| `end [expr]` | Close block; optional return expr for fun |\n";
                    md << "| `alias name = expr` | REPL shortcut |\n";
                    md << "| `his` | Show today's session history |\n";
                    md << "| `lib` | List all loaded builtin modules |\n";
                    md << "| `exit` / `quit` | Exit interpreter |\n";
                    md << "| `langport(pattern,[out])` | AI-port foreign code to Gem |\n\n";

                    md << "## Types\n\n";
                    md << "| Type | Declaration | Notes |\n|------|-------------|-------|\n";
                    md << "| `int` | `int x = 0` | Stores double; also holds vectors |\n";
                    md << "| `double` | `double pi = 3.14` | IEEE 754 double |\n";
                    md << "| `string` | `string s = \"hi\"` | UTF-8 string |\n";
                    md << "| `bool` | `bool ok = true` | true / false |\n";
                    md << "| `_var` | `_config = 1` | Underscore prefix = global scope |\n\n";
                    md << "Multi-var: `int a, b, c = 0`  Chained: `a = b = c = 0`\n\n";

                    md << "## Literals\n\n";
                    md << "`true` `false` `null` `nil` (=null) `nan` (IEEE 754 NaN)\n\n";

                    md << "## Operators\n\n";
                    md << "| Category | Operators |\n|----------|-----------|\n";
                    md << "| Arithmetic | `+` `-` `*` `/` |\n";
                    md << "| Compound assign | `+=` `-=` `*=` `/=` |\n";
                    md << "| Comparison | `==` `!=` `>` `>=` `<` `<=` |\n";
                    md << "| Logical | `!` (prefix not) `&&` `||` |\n";
                    md << "| Separator | `;` (multiple statements per line) |\n";
                    md << "| String concat | `+` |\n\n";

                    md << "## Builtin Functions\n\n";
                    md << "| Function | Params | Returns | Notes |\n|----------|--------|---------|-------|\n";
                    md << "| `isnil(x)` | any | bool | true if x is nil/null/void |\n";
                    md << "| `isnan(x)` | any | bool | true if x is IEEE 754 NaN |\n";
                    md << "| `tonum(x)` | any | double | parse to number; nan on failure |\n";
                    md << "| `tostr(x)` | any | string | convert to string |\n\n";

                    md << "## Vectors\n\n";
                    md << "```\nint v = [1.0, 2.0, 3.0]   # creation\nv[0]                       # index (0-based)\nv.length / v.size / v.count  # length\n```\n\n";

                    md << "## Builtin Modules (35)\n\n";

                    // sys
                    md << "### sys — System Root (inherited by all objects)\n";
                    md << "| Method | Signature | Returns |\n|--------|-----------|--------|\n";
                    md << "| `print` | `print(...)` | void |\n";
                    md << "| `args` | `args()` | vector\\<string\\> |\n";
                    md << "| `async` | `async(func,[params],[timeout])` | thread handle |\n";
                    md << "| `sethandler` | `sethandler(signals,func)` | bool |\n";
                    md << "| `host` | `host()` | string |\n";
                    md << "| `exec` | `exec(cmd)` | int (exit code) |\n";
                    md << "| `doc` | `doc([path])` | string |\n";
                    md << "| `help` | `help([topic])` | void |\n";
                    md << "| `exit` | `exit()` | void |\n";
                    md << "| `langport` | `langport(pattern,[output])` | bool |\n";
                    md << "| `redirect` | `redirect(url,[port])` | bool |\n";
                    md << "| `app` | `app(port,[routes])` | bool |\n\n";

                    // math
                    md << "### math — Mathematics & Symbolic Math\n";
                    md << "**Numeric:** `sin(x)` `cos(x)` `sqrt(x)` `math.pi`  \n";
                    md << "**Symbolic (SymPy/Sage):** `useSymPy()` `useSage()` `diff(expr,[var])` `integrate(expr,[var])` `simplify(expr)` `solve(expr,[var])` `sym_latex(expr)` `to_latex(val)` `write_latex(path,content,[standalone])` `read_latex(path)` `parse_latex(text)` `compile_latex(path)`\n\n";

                    // ai
                    md << "### ai — AI Integration (Gemini/Mistral/Ollama)\n";
                    md << "| Method | Signature | Returns |\n|--------|-----------|--------|\n";
                    md << "| `prompt` | `prompt(text)` | string |\n";
                    md << "| `prompt_native` | `prompt_native(text)` | string |\n";
                    md << "| `useGemini` | `useGemini()` | bool |\n";
                    md << "| `useMistral` | `useMistral(model)` | bool |\n";
                    md << "| `useOllama` | `useOllama(model,[host])` | bool |\n";
                    md << "| `setKey` | `setKey(key)` | bool |\n";
                    md << "| `setHost` | `setHost(host)` | bool |\n";
                    md << "| `setPath` | `setPath(path)` | bool |\n";
                    md << "**Properties:** `ai.provider` `ai.model` `ai.host`\n\n";

                    // text
                    md << "### text — String & Document Processing\n";
                    md << "`read(path)→string` `sub(s,start,[len])→string` `replace(s,old,new)→string`  \n";
                    md << "`write_pdf(path,text)→bool` `write_pdf_a(path,text)→bool` `read_pdf(path)→string`  \n";
                    md << "`read_markdown(path)` `write_markdown(path,text)` `read_yaml(path)` `write_yaml(path,obj)`  \n";
                    md << "`read_html(path)` `write_html(path,text)` `read_xml(path)` `write_xml(path,text)`  \n";
                    md << "`read_fits_header(path)→object` `read_hdf_header(path)→object`\n\n";

                    // rex
                    md << "### rex — Regular Expressions (ECMAScript)\n";
                    md << "All accept optional `flags` string (`\"i\"` = case-insensitive):  \n";
                    md << "`match(text,pattern,[flags])→bool` `find(text,pattern,[flags])→string`  \n";
                    md << "`findall(text,pattern,[flags])→list` `groups(text,pattern,[flags])→list`  \n";
                    md << "`sub(text,pattern,repl,[flags])→string` `gsub(text,pattern,repl,[flags])→string`  \n";
                    md << "`split(text,pattern,[flags])→list` `count(text,pattern,[flags])→int`\n\n";

                    // algo
                    md << "### algo — Algorithms, Sorting & Time\n";
                    md << "`add(...)→double` `quicksort(v)→vector` `sort(v,[start],[end])→vector`  \n";
                    md << "`now()→string` `date_add(ts,days)→double` `date_diff(t1,t2)→double`\n\n";

                    // geo
                    md << "### geo — GIS & Geolocation\n";
                    md << "`lookup()→{.lat,.lon,.city,.country}` `distance(lat1,lon1,lat2,lon2)→double`  \n";
                    md << "`write_geojson(path,features)→bool` `history(plate)→string`  \n";
                    md << "`plot2d(data,[layout])→string` `plot3d(data,[layout])→string`\n\n";

                    // astro
                    md << "### astro — Astrophysics & Planetary Science\n";
                    md << "**Constants:** `G` `c` `AU` `pc` `ly` `Msun` `Rsun` `Lsun` `Tsun` `Mearth` `Rearth` `H0` `sigma_sb`  \n";
                    md << "**Unit conv:** `to_ly(pc)` `to_pc(ly)` `to_au(m)` `deg_to_rad(d)` `rad_to_deg(r)`  \n";
                    md << "**Stellar:** `luminosity(R,T)` `stefan_boltzmann(T,R)` `wien(T)` `abs_magnitude(m,d_pc)` `distance_modulus(d_pc)` `spectral_class(T)` `schwarzschild_radius(M)` `escape_velocity(M,R)`  \n";
                    md << "**Orbital:** `orbital_period(a_au,M_msun)` `orbital_velocity(a_m,M_kg)` `hill_sphere(a,mp,ms)` `roche_limit(R,rho_p,rho_s)` `synodic_period(p1,p2)` `planet(name)`  \n";
                    md << "**Solar:** `solar_flux(dist_au)` `solar_wind_pressure(n,v)` `sunspot_cycle(year)` `parker_spiral_angle(v,d)` `solar_activity()`  \n";
                    md << "**Cosmology:** `hubble_distance(z)` `redshift_velocity(z)` `lookback_time(z)` `critical_density()`  \n";
                    md << "**Coords:** `equatorial_to_galactic(ra,dec)` `angular_separation(ra1,dec1,ra2,dec2)`  \n";
                    md << "**Exoplanet:** `transit_depth(rp,rs)` `habitable_zone(L)` `equilibrium_temp(L,d,A)`  \n";
                    md << "**Pulsar:** `pulsar_spindown(P,Pdot)→{age_yr,Bfield_G,edot_W}`\n\n";

                    // fin
                    md << "### fin — Financial Engineering (QuantLib + yfinance)\n";
                    md << "`ticker(symbol)→{.price,.volume,.open,.high,.low,.close,.pe_ratio,.market_cap,.dividend_yield}`  \n";
                    md << "`high_yield_bonds()` `high_yield_etfs()` `high_yield_equities()` `dashboard()`  \n";
                    md << "`bs_price(type,strike,spot,rate,vol,t)→double`  \n";
                    md << "`greeks(type,strike,spot,rate,vol,t)→{.npv,.delta,.gamma,.theta,.vega}`  \n";
                    md << "`date(d,m,y)→{.day,.month,.year,.serial,.str}` `calendar(name)→object`  \n";
                    md << "`is_holiday(cal,d,m,y)→bool` `add_days(d,m,y,n)→{.day,.month,.year,.str}` `diff_days(d1,m1,y1,d2,m2,y2)→double`\n\n";

                    // bsm
                    md << "### bsm — Black-Scholes-Merton (extends fin)\n";
                    md << "`price_american(symbol,type,duration)→double`  duration: `\"weekly\"` `\"monthly\"` `\"quarterly\"`\n\n";

                    // chart
                    md << "### chart — Interactive Plotting (Plotly.js)\n";
                    md << "`plot(traces,[layout])→string` `show(path)→bool` `server(path,[port])→bool`\n\n";

                    // data
                    md << "### data — Data Science\n";
                    md << "`read_csv(path)→vector` `mean(vector)→double` `std(vector)→double`\n\n";

                    // tcp
                    md << "### tcp — TCP/IP Networking\n";
                    md << "`listen(port)→int` `accept(fd)→{.fd,.addr}` `connect(host,port)→int`  \n";
                    md << "`send(fd,msg)→int` `recv(fd,[size])→string` `close(fd)→bool`  \n";
                    md << "`nic()→[{.name,.ip,.status}]` `routes()→[{.dest,.gw,.iface}]`\n\n";

                    // thread
                    md << "### thread — Background Thread Handle (from sys.async)\n";
                    md << "`wait()→any` `is_finished()→bool`\n\n";

                    // www
                    md << "### www — Web Framework\n";
                    md << "`wget(url,file)→bool` `app(port,[routes])→bool` `redirect(target,[port])→bool` `map2d(geojson,output)→bool`\n\n";

                    // cdn
                    md << "### cdn — Caching Proxy (extends www)\n";
                    md << "`start(port,routes_map)→bool` `stats()→{.hits,.misses,.bytes,.cached_items}` `purge([path])→bool` `config(type,content)→string` `dashboard()→bool`\n\n";

                    // itr
                    md << "### itr — Iterators\n";
                    md << "`range(n)→[0..n-1]` `while(cond_fun,body_fun)→void`\n\n";

                    // bev
                    md << "### bev — Bevington Data Reduction\n";
                    md << "`data(x_vec,y_vec)→bool` `fit_line()→bool` `param(idx)→double`  idx: 0=intercept, 1=slope\n\n";

                    // file
                    md << "### file — Filesystem\n";
                    md << "`write(path,content)→bool` `exists(path)→bool`\n\n";

                    // zip
                    md << "### zip — Compression\n";
                    md << "`compress(src,archive)→bool` `decompress(archive,dest)→bool`\n\n";

                    // img
                    md << "### img — Image Processing (ImageMagick)\n";
                    md << "`resize(src,width,height,dest)→bool`\n\n";

                    // nlp
                    md << "### nlp — Natural Language Processing\n";
                    md << "Delegates to `ai.prompt()`. Used internally by `mobl.dictate()`.\n\n";

                    // cpp
                    md << "### cpp — C++26 JIT (Cling)\n";
                    md << "`repl()→int` `exec(code)→int`\n\n";

                    // k3s
                    md << "### k3s — Docker & Kubernetes\n";
                    md << "`docker_run(image,cmd)` `docker_ps()` `docker_build(path,tag)` `docker_stop(id)`  \n";
                    md << "`k3s_apply(yaml)` `k3s_get(resource)` `k3s_pods()` `k3s_nodes()` `k3s_logs(pod)`\n\n";

                    // vm
                    md << "### vm — Vagrant VMs\n";
                    md << "`init(box)` `up()` `ssh(cmd)` `status()` `halt()` `destroy()`\n\n";

                    // mobl
                    md << "### mobl — Mobile PWA\n";
                    md << "`phone(name)→object` `dictate(text)→{title,note,tags}` `make_feature(lat,lon,text)→GeoJSON`\n\n";

                    // trek
                    md << "### trek — Travel Logs & OSM\n";
                    md << "`new(path)` `add(path,lat,lon,[title],[note])` `edit(path,idx,title,note)` `remove(path,idx)`  \n";
                    md << "`load(path)→string` `show(path,[port])` `export_gpx(geojson,gpx)` `stats(path)→{.waypoints,.distance_km}`\n\n";

                    // seo
                    md << "### seo — Search Engine Optimization\n";
                    md << "`index(urls,output_path)→bool` `analyze(index_path)→void`\n\n";

                    // drvr
                    md << "### drvr — Device Drivers\n";
                    md << "`linux(name)` `win11(name)` `macos(name)` `android(name)` `build(target)` `deploy(target)`\n\n";

                    // art
                    md << "### art — ASCII Art & SVG\n";
                    md << "`text_to_art(text,[font])→string` `art_to_file(text,path)→bool` `art_to_svg(text,[path])→string`  \n";
                    md << "`svg_to_art(svg_path,[width])→string` `mindmap([path])→string` `readme([path])→string` `tutorial([path])→string`\n\n";

                    // polyglot
                    md << "### go / ruby / node / rust — Polyglot\n";
                    md << "`run(input)→string`  \n";
                    md << "`go.build(file)` `node.npm_install(pkg)` `rust.cargo_new(name)`\n\n";

                    md << "## CLI\n\n";
                    md << "```\ngem <file.g>              Run script\ngem -c <main.g> [mods...] Compile to binary\ngem -o <name>             Output name\ngem -h                    Print history and exit\ngem -t <file> [-o out]    AI-translate to Gem\ngem                       Start REPL\n```\n\n";

                    md << "## REPL\n\n";
                    md << "- Expressions print `=> value`\n";
                    md << "- Arrow keys: readline history\n";
                    md << "- History saved to `~/.gem/YYYYDDD.txt`\n";
                    md << "- `his` — show today's log\n";
                    md << "- `lib` — list modules\n";
                    md << "- `help \"topic\"` — detailed help\n";
                    md << "- `alias ? = sys.help()` — define shortcut\n\n";

                    md << "## Object System\n\n";
                    md << "```\nobj Name(params) : Parent\n  .attr = val        # own attribute\n  ..attr = val       # parent attribute\n  fun method(p) ... end\nend\nName var = Name(args)\n```\nAll objects inherit from `sys`. Single inheritance only.\n\n";

                    md << "---\n*Generated by `sys.help(\"all\")`*\n";

                    // Write file
                    std::ofstream ofs(filename);
                    ofs << md.str();
                    ofs.close();

                    // Dedup: find any prior helpfull_*.md and remove if identical
                    {
                        namespace fs = std::filesystem;
                        std::string newContent = md.str();
                        for (auto& entry : fs::directory_iterator(".")) {
                            std::string name = entry.path().filename().string();
                            if (name == filename) continue;
                            if (name.rfind("helpfull_", 0) == 0 && name.size() > 9 &&
                                name.substr(name.size() - 3) == ".md") {
                                std::ifstream prev(entry.path());
                                std::string prevContent((std::istreambuf_iterator<char>(prev)),
                                                         std::istreambuf_iterator<char>());
                                if (prevContent == newContent) {
                                    fs::remove(filename);
                                    filename = name;
                                    std::cout << "No change — reusing: " << filename << std::endl;
                                    goto helpfull_done;
                                } else {
                                    fs::remove(entry.path());
                                }
                            }
                        }
                    }
                    std::cout << "Written: " << filename << std::endl;
                    helpfull_done:;
                    // Open the markdown file in the default system viewer
                    // (suppressed when a second arg "noopen" is passed, e.g. from helpless)
                    bool noopen = (args.size() >= 2 && std::holds_alternative<std::string>(args[1]->value)
                                   && std::get<std::string>(args[1]->value) == "noopen");
                    if (!noopen) {
                        std::string open_cmd;
#if defined(_WIN32)
                        open_cmd = "start \"\" \"" + filename + "\"";
#elif defined(__APPLE__)
                        open_cmd = "open \"" + filename + "\"";
#else
                        open_cmd = "xdg-open \"" + filename + "\" &";
#endif
                        std::system(open_cmd.c_str());
                    }
                    // Also display to stdout (suppressed when called from helpless — less handles display)
                    if (!noopen) {
                        std::cout << md.str();
                    }
                    std::cout << "Available topics: sys, math, ai, text, rex, algo, bev, file, zip, nlp, img," << std::endl;
                    std::cout << "  www, cdn, geo, astro, fin, bsm, chart, data, tcp, thread, cpp, itr," << std::endl;
                    std::cout << "  k3s, vm, mobl, trek, seo, drvr, art, go, ruby, node, rust," << std::endl;
                    std::cout << "  fun, obj, use, if, else, while, end, alias, his, lib, exit, quit," << std::endl;
                    std::cout << "  int, double, string, bool, true, false, null, nil, nan," << std::endl;
                    std::cout << "  isnil, isnan, tonum, tostr, langport, operators, ;, vectors, repl" << std::endl;
                    return std::make_shared<GemValue>(filename);
                }
            }
            return std::make_shared<GemValue>();
        }, true };

        methods["helpfull"] = { [this](std::vector<std::shared_ptr<GemValue>> args) {
            return call("help", {std::make_shared<GemValue>(std::string("all"))});
        }, true };

        methods["helpless"] = { [this](std::vector<std::shared_ptr<GemValue>> args) {
            // helpless: write the reference file then pipe it through less.
            // We get the filename back from help("all") to avoid a timestamp race.
            auto result = call("help", {std::make_shared<GemValue>(std::string("all")), std::make_shared<GemValue>(std::string("noopen"))});
            std::string filename;
            if (result && std::holds_alternative<std::string>(result->value))
                filename = std::get<std::string>(result->value);
            if (!filename.empty())
                std::system(("less \"" + filename + "\"").c_str());
            return result;
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
        // Initialize embedded Python once (Python 3.14 compatible)
        if (!pythonInitialized) {
            PyImport_AppendInittab("gem_mistral_bridge", PyInit_gem_mistral_bridge);
            Py_Initialize();
            PyImport_ImportModule("gem_mistral_bridge");
            pythonInitialized = true;
        }
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

        methods["useGemini"] = { [this](std::vector<std::shared_ptr<GemValue>> args) {
            provider = "gemini";
            model = "gemini-pro";
            host = "generativelanguage.googleapis.com";
            path = "/v1beta/models/gemini-pro:generateContent";
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
        methods["accept"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            auto sock = std::make_shared<GemObject>("Socket");
            sock->set("fd", std::make_shared<GemValue>(0.0));
            sock->set("addr", std::make_shared<GemValue>("0.0.0.0"));
            return std::make_shared<GemValue>(sock);
        }, true };
        methods["send"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            return std::make_shared<GemValue>(args.size() > 1 ? (double)args[1]->toString().size() : 0.0);
        }, true };
        methods["recv"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            return std::make_shared<GemValue>(std::string(""));
        }, true };
        methods["close"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            return std::make_shared<GemValue>(true);
        }, true };
        
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



// Rex (Regex) Object
class GemRex : public GemSys {
public:
    GemRex() : GemSys() {
        name = "rex";

        // match(text, pattern, [flags]) -> bool
        methods["match"] = { [](std::vector<std::shared_ptr<GemValue>> args) -> std::shared_ptr<GemValue> {
            if (args.size() < 2) return std::make_shared<GemValue>(false);
            std::string text = args[0]->toString();
            std::string pat  = args[1]->toString();
            std::string flags = (args.size() > 2) ? args[2]->toString() : "";
            auto rf = std::regex_constants::ECMAScript;
            if (flags.find('i') != std::string::npos) rf |= std::regex_constants::icase;
            try {
                return std::make_shared<GemValue>(std::regex_search(text, std::regex(pat, rf)));
            } catch (...) { return std::make_shared<GemValue>(false); }
        }, true };

        // find(text, pattern, [flags]) -> first match string or ""
        methods["find"] = { [](std::vector<std::shared_ptr<GemValue>> args) -> std::shared_ptr<GemValue> {
            if (args.size() < 2) return std::make_shared<GemValue>("");
            std::string text = args[0]->toString();
            std::string pat  = args[1]->toString();
            std::string flags = (args.size() > 2) ? args[2]->toString() : "";
            auto rf = std::regex_constants::ECMAScript;
            if (flags.find('i') != std::string::npos) rf |= std::regex_constants::icase;
            try {
                std::smatch m;
                if (std::regex_search(text, m, std::regex(pat, rf)))
                    return std::make_shared<GemValue>(m[0].str());
            } catch (...) {}
            return std::make_shared<GemValue>("");
        }, true };

        // findall(text, pattern, [flags]) -> vector of all match strings
        methods["findall"] = { [](std::vector<std::shared_ptr<GemValue>> args) -> std::shared_ptr<GemValue> {
            auto result = std::make_shared<GemValue>();
            result->value = std::vector<std::shared_ptr<GemValue>>{};
            if (args.size() < 2) return result;
            std::string text = args[0]->toString();
            std::string pat  = args[1]->toString();
            std::string flags = (args.size() > 2) ? args[2]->toString() : "";
            auto rf = std::regex_constants::ECMAScript;
            if (flags.find('i') != std::string::npos) rf |= std::regex_constants::icase;
            try {
                std::regex re(pat, rf);
                auto begin = std::sregex_iterator(text.begin(), text.end(), re);
                auto end   = std::sregex_iterator();
                auto& vec  = std::get<std::vector<std::shared_ptr<GemValue>>>(result->value);
                for (auto it = begin; it != end; ++it)
                    vec.push_back(std::make_shared<GemValue>((*it)[0].str()));
            } catch (...) {}
            return result;
        }, true };

        // groups(text, pattern, [flags]) -> vector of capture groups from first match
        methods["groups"] = { [](std::vector<std::shared_ptr<GemValue>> args) -> std::shared_ptr<GemValue> {
            auto result = std::make_shared<GemValue>();
            result->value = std::vector<std::shared_ptr<GemValue>>{};
            if (args.size() < 2) return result;
            std::string text = args[0]->toString();
            std::string pat  = args[1]->toString();
            std::string flags = (args.size() > 2) ? args[2]->toString() : "";
            auto rf = std::regex_constants::ECMAScript;
            if (flags.find('i') != std::string::npos) rf |= std::regex_constants::icase;
            try {
                std::smatch m;
                if (std::regex_search(text, m, std::regex(pat, rf))) {
                    auto& vec = std::get<std::vector<std::shared_ptr<GemValue>>>(result->value);
                    for (size_t i = 1; i < m.size(); ++i)
                        vec.push_back(std::make_shared<GemValue>(m[i].str()));
                }
            } catch (...) {}
            return result;
        }, true };

        // sub(text, pattern, replacement, [flags]) -> string with first replacement
        methods["sub"] = { [](std::vector<std::shared_ptr<GemValue>> args) -> std::shared_ptr<GemValue> {
            if (args.size() < 3) return std::make_shared<GemValue>("");
            std::string text = args[0]->toString();
            std::string pat  = args[1]->toString();
            std::string repl = args[2]->toString();
            std::string flags = (args.size() > 3) ? args[3]->toString() : "";
            auto rf = std::regex_constants::ECMAScript;
            if (flags.find('i') != std::string::npos) rf |= std::regex_constants::icase;
            try {
                return std::make_shared<GemValue>(
                    std::regex_replace(text, std::regex(pat, rf), repl,
                        std::regex_constants::format_first_only));
            } catch (...) { return std::make_shared<GemValue>(text); }
        }, true };

        // gsub(text, pattern, replacement, [flags]) -> string with all replacements
        methods["gsub"] = { [](std::vector<std::shared_ptr<GemValue>> args) -> std::shared_ptr<GemValue> {
            if (args.size() < 3) return std::make_shared<GemValue>("");
            std::string text = args[0]->toString();
            std::string pat  = args[1]->toString();
            std::string repl = args[2]->toString();
            std::string flags = (args.size() > 3) ? args[3]->toString() : "";
            auto rf = std::regex_constants::ECMAScript;
            if (flags.find('i') != std::string::npos) rf |= std::regex_constants::icase;
            try {
                return std::make_shared<GemValue>(
                    std::regex_replace(text, std::regex(pat, rf), repl));
            } catch (...) { return std::make_shared<GemValue>(text); }
        }, true };

        // split(text, pattern, [flags]) -> vector of substrings
        methods["split"] = { [](std::vector<std::shared_ptr<GemValue>> args) -> std::shared_ptr<GemValue> {
            auto result = std::make_shared<GemValue>();
            result->value = std::vector<std::shared_ptr<GemValue>>{};
            if (args.size() < 2) return result;
            std::string text = args[0]->toString();
            std::string pat  = args[1]->toString();
            std::string flags = (args.size() > 2) ? args[2]->toString() : "";
            auto rf = std::regex_constants::ECMAScript;
            if (flags.find('i') != std::string::npos) rf |= std::regex_constants::icase;
            try {
                std::regex re(pat, rf);
                std::sregex_token_iterator it(text.begin(), text.end(), re, -1), end;
                auto& vec = std::get<std::vector<std::shared_ptr<GemValue>>>(result->value);
                for (; it != end; ++it)
                    vec.push_back(std::make_shared<GemValue>(it->str()));
            } catch (...) {}
            return result;
        }, true };

        // count(text, pattern, [flags]) -> number of non-overlapping matches
        methods["count"] = { [](std::vector<std::shared_ptr<GemValue>> args) -> std::shared_ptr<GemValue> {
            if (args.size() < 2) return std::make_shared<GemValue>(0.0);
            std::string text = args[0]->toString();
            std::string pat  = args[1]->toString();
            std::string flags = (args.size() > 2) ? args[2]->toString() : "";
            auto rf = std::regex_constants::ECMAScript;
            if (flags.find('i') != std::string::npos) rf |= std::regex_constants::icase;
            try {
                std::regex re(pat, rf);
                auto begin = std::sregex_iterator(text.begin(), text.end(), re);
                return std::make_shared<GemValue>((double)std::distance(begin, std::sregex_iterator()));
            } catch (...) { return std::make_shared<GemValue>(0.0); }
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
        methods["quicksort"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty() || !std::holds_alternative<std::vector<double>>(args[0]->value)) return std::make_shared<GemValue>();
            auto v = std::get<std::vector<double>>(args[0]->value);
            std::sort(v.begin(), v.end());
            args[0]->value = v;
            return std::make_shared<GemValue>(v);
        }, true };
        methods["sort"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty() || !std::holds_alternative<std::vector<double>>(args[0]->value)) return std::make_shared<GemValue>();
            auto v = std::get<std::vector<double>>(args[0]->value);
            int start = (args.size() > 1 && std::holds_alternative<double>(args[1]->value)) ? (int)std::get<double>(args[1]->value) : 0;
            int end   = (args.size() > 2 && std::holds_alternative<double>(args[2]->value)) ? (int)std::get<double>(args[2]->value) : (int)v.size();
            if (start < 0) start = 0;
            if (end > (int)v.size()) end = (int)v.size();
            std::sort(v.begin() + start, v.begin() + end);
            args[0]->value = v;
            return std::make_shared<GemValue>(v);
        }, true };
        methods["date_add"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.size() < 2) return std::make_shared<GemValue>(0.0);
            double ts = std::holds_alternative<double>(args[0]->value) ? std::get<double>(args[0]->value) : 0.0;
            double days = std::holds_alternative<double>(args[1]->value) ? std::get<double>(args[1]->value) : 0.0;
            return std::make_shared<GemValue>(ts + days * 86400.0);
        }, true };
        methods["date_diff"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.size() < 2) return std::make_shared<GemValue>(0.0);
            double t1 = std::holds_alternative<double>(args[0]->value) ? std::get<double>(args[0]->value) : 0.0;
            double t2 = std::holds_alternative<double>(args[1]->value) ? std::get<double>(args[1]->value) : 0.0;
            return std::make_shared<GemValue>((t2 - t1) / 86400.0);
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
        methods["decompress"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.size() < 2) return std::make_shared<GemValue>(false);
            std::string archive = args[0]->toString();
            std::string dest = args[1]->toString();
            std::string cmd = "unzip -o \"" + archive + "\" -d \"" + dest + "\" 2>/dev/null";
            int rc = std::system(cmd.c_str());
            return std::make_shared<GemValue>(rc == 0);
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
        name = "k3s";
        
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
        methods["calendar"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            std::string name = args.empty() ? "TARGET" : args[0]->toString();
            auto obj = std::make_shared<GemObject>("Calendar");
            obj->set("name", std::make_shared<GemValue>(name));
            return std::make_shared<GemValue>(obj);
        }, true };
        methods["is_holiday"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.size() < 4) return std::make_shared<GemValue>(false);
            int d = (int)std::get<double>(args[1]->value);
            int m = (int)std::get<double>(args[2]->value);
            int y = (int)std::get<double>(args[3]->value);
            QuantLib::Date date(d, (QuantLib::Month)m, y);
            QuantLib::TARGET cal;
            return std::make_shared<GemValue>(!cal.isBusinessDay(date));
        }, true };
        methods["add_days"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.size() < 4) return std::make_shared<GemValue>();
            int d = (int)std::get<double>(args[0]->value);
            int m = (int)std::get<double>(args[1]->value);
            int y = (int)std::get<double>(args[2]->value);
            int n = (int)std::get<double>(args[3]->value);
            QuantLib::Date date(d, (QuantLib::Month)m, y);
            date += n;
            auto obj = std::make_shared<GemObject>("Date");
            obj->set("day",   std::make_shared<GemValue>((double)date.dayOfMonth()));
            obj->set("month", std::make_shared<GemValue>((double)(int)date.month()));
            obj->set("year",  std::make_shared<GemValue>((double)date.year()));
            obj->set("str",   std::make_shared<GemValue>(std::to_string(date.dayOfMonth()) + "/" + std::to_string((int)date.month()) + "/" + std::to_string(date.year())));
            return std::make_shared<GemValue>(obj);
        }, true };
        methods["diff_days"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.size() < 6) return std::make_shared<GemValue>(0.0);
            QuantLib::Date d1((int)std::get<double>(args[0]->value), (QuantLib::Month)(int)std::get<double>(args[1]->value), (int)std::get<double>(args[2]->value));
            QuantLib::Date d2((int)std::get<double>(args[3]->value), (QuantLib::Month)(int)std::get<double>(args[4]->value), (int)std::get<double>(args[5]->value));
            return std::make_shared<GemValue>((double)(d2 - d1));
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

// GemTrek — Travel Log creation, display, and editing using OSM/geo/GIS
class GemTrek : public GemSys {
public:
    GemTrek() : GemSys() {
        name = "trek";

        // trek.new(path) — create a new empty travel log GeoJSON file
        methods["new"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            std::string path = args.empty() ? "trek_log.geojson" : args[0]->toString();
            std::ofstream out(path);
            out << "{\"type\":\"FeatureCollection\",\"features\":[]}";
            return std::make_shared<GemValue>(path);
        }, true };

        // trek.add(path, lat, lon, title, note) — append a waypoint feature
        methods["add"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.size() < 3) return std::make_shared<GemValue>(false);
            std::string path  = args[0]->toString();
            std::string lat   = args[1]->toString();
            std::string lon   = args[2]->toString();
            std::string title = args.size() > 3 ? args[3]->toString() : "Waypoint";
            std::string note  = args.size() > 4 ? args[4]->toString() : "";

            // Read existing
            std::ifstream in(path);
            std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
            in.close();

            // Build new feature
            std::string feat =
                "{\"type\":\"Feature\","
                "\"geometry\":{\"type\":\"Point\",\"coordinates\":[" + lon + "," + lat + "]},"
                "\"properties\":{\"title\":\"" + title + "\",\"note\":\"" + note + "\"}}";

            // Insert before closing ]
            size_t pos = content.rfind(']');
            if (pos == std::string::npos) return std::make_shared<GemValue>(false);
            bool hasFeatures = content.find("\"type\":\"Feature\"") != std::string::npos;
            content.insert(pos, (hasFeatures ? "," : "") + feat);

            std::ofstream out(path);
            out << content;
            return std::make_shared<GemValue>(true);
        }, true };

        // trek.edit(path, index, title, note) — update title/note of feature at index
        methods["edit"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.size() < 4) return std::make_shared<GemValue>(false);
            std::string path  = args[0]->toString();
            int idx           = (int)std::get<double>(args[1]->value);
            std::string title = args[2]->toString();
            std::string note  = args[3]->toString();

            std::ifstream in(path);
            std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
            in.close();

            // Find the idx-th "properties" block and replace title/note (simple string replacement)
            size_t pos = 0;
            for (int i = 0; i <= idx; ++i) {
                pos = content.find("\"properties\"", pos);
                if (pos == std::string::npos) return std::make_shared<GemValue>(false);
                if (i < idx) pos++;
            }
            size_t end = content.find('}', pos);
            if (end == std::string::npos) return std::make_shared<GemValue>(false);
            std::string newProps = "\"properties\":{\"title\":\"" + title + "\",\"note\":\"" + note + "\"}";
            content.replace(pos, end - pos + 1, newProps);

            std::ofstream out(path);
            out << content;
            return std::make_shared<GemValue>(true);
        }, true };

        // trek.remove(path, index) — remove feature at index
        methods["remove"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.size() < 2) return std::make_shared<GemValue>(false);
            std::string path = args[0]->toString();
            int idx = (int)std::get<double>(args[1]->value);

            std::ifstream in(path);
            std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
            in.close();

            // Find idx-th Feature object and remove it
            size_t pos = 0;
            for (int i = 0; i <= idx; ++i) {
                pos = content.find("{\"type\":\"Feature\"", pos);
                if (pos == std::string::npos) return std::make_shared<GemValue>(false);
                if (i < idx) pos++;
            }
            size_t end = pos;
            int depth = 0;
            for (size_t k = pos; k < content.size(); ++k) {
                if (content[k] == '{') depth++;
                else if (content[k] == '}') { depth--; if (depth == 0) { end = k; break; } }
            }
            // Remove feature and any trailing/leading comma
            size_t removeStart = pos, removeEnd = end + 1;
            if (removeEnd < content.size() && content[removeEnd] == ',') removeEnd++;
            else if (removeStart > 0 && content[removeStart-1] == ',') removeStart--;
            content.erase(removeStart, removeEnd - removeStart);

            std::ofstream out(path);
            out << content;
            return std::make_shared<GemValue>(true);
        }, true };

        // trek.load(path) — return GeoJSON string
        methods["load"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty()) return std::make_shared<GemValue>("{}");
            std::ifstream in(args[0]->toString());
            std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
            return std::make_shared<GemValue>(content);
        }, true };

        // trek.show(path, [port]) — serve interactive OSM map via background HTTP server
        methods["show"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty()) return std::make_shared<GemValue>(false);
            std::string geojsonPath = args[0]->toString();
            int port = (args.size() > 1 && std::holds_alternative<double>(args[1]->value))
                       ? (int)std::get<double>(args[1]->value) : 8090;

            // Read GeoJSON
            std::ifstream in(geojsonPath);
            std::string geojson((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

            // Build OSM Leaflet HTML
            std::string html = R"GEM(<!DOCTYPE html><html><head>
<meta charset="utf-8"><title>Gem Trek Log</title>
<link rel="stylesheet" href="https://unpkg.com/leaflet@1.9.4/dist/leaflet.css"/>
<script src="https://unpkg.com/leaflet@1.9.4/dist/leaflet.js"></script>
<style>html,body,#map{height:100%;margin:0;padding:0;}
#panel{position:absolute;top:10px;right:10px;z-index:1000;background:#fff;padding:10px;border-radius:6px;max-width:260px;font-family:sans-serif;font-size:13px;}
</style></head><body>
<div id="map"></div>
<div id="panel"><b>Gem Trek Log</b><br><small>Click a marker to view/edit</small><br><br>
<button onclick="addWaypoint()">Add Waypoint</button></div>
<script>
var geojson = )GEM" + geojson + R"GEM(;
var map = L.map('map').setView([0,0],2);
L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png',{
  attribution:'&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
}).addTo(map);
var markers = [];
function renderFeatures(){
  markers.forEach(function(m){map.removeLayer(m);});
  markers=[];
  if(!geojson.features||!geojson.features.length)return;
  var bounds=[];
  geojson.features.forEach(function(f,i){
    if(f.geometry.type!=='Point')return;
    var ll=[f.geometry.coordinates[1],f.geometry.coordinates[0]];
    bounds.push(ll);
    var m=L.marker(ll).addTo(map);
    var p=f.properties||{};
    m.bindPopup('<b>'+(p.title||'Waypoint')+'</b><br>'+(p.note||'')+'<br><br>'
      +'<button onclick="editFeature('+i+')">Edit</button> '
      +'<button onclick="removeFeature('+i+')">Remove</button>');
    markers.push(m);
  });
  if(bounds.length)map.fitBounds(bounds,{padding:[40,40]});
}
function addWaypoint(){
  var title=prompt('Title:','Waypoint');if(!title)return;
  var note=prompt('Note:','');
  map.once('click',function(e){
    fetch('/add',{method:'POST',headers:{'Content-Type':'application/json'},
      body:JSON.stringify({lat:e.latlng.lat,lon:e.latlng.lng,title:title,note:note||''})})
    .then(r=>r.json()).then(function(d){geojson=d;renderFeatures();});
  });
  alert('Click on the map to place the waypoint.');
}
function editFeature(i){
  var p=geojson.features[i].properties||{};
  var title=prompt('Title:',p.title||'');if(title===null)return;
  var note=prompt('Note:',p.note||'');if(note===null)return;
  fetch('/edit',{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify({index:i,title:title,note:note})})
  .then(r=>r.json()).then(function(d){geojson=d;renderFeatures();});
}
function removeFeature(i){
  if(!confirm('Remove waypoint?'))return;
  fetch('/remove',{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify({index:i})})
  .then(r=>r.json()).then(function(d){geojson=d;renderFeatures();});
}
renderFeatures();
</script></body></html>)GEM";

            std::thread([port, geojsonPath, html]() {
                httplib::Server svr;
                svr.Get("/", [html](const httplib::Request&, httplib::Response& res) {
                    res.set_content(html, "text/html");
                });
                svr.Get("/data", [geojsonPath](const httplib::Request&, httplib::Response& res) {
                    std::ifstream in(geojsonPath);
                    std::string gj((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
                    res.set_content(gj, "application/json");
                });
                svr.Post("/add", [geojsonPath](const httplib::Request& req, httplib::Response& res) {
                    // Parse lat/lon/title/note from JSON body (minimal parse)
                    auto body = req.body;
                    auto extract = [&](const std::string& key) {
                        size_t p = body.find("\"" + key + "\"");
                        if (p == std::string::npos) return std::string("");
                        p = body.find(':', p) + 1;
                        while (p < body.size() && (body[p]==' '||body[p]=='"')) p++;
                        size_t e = body.find_first_of(",}\"\n", p);
                        return body.substr(p, e - p);
                    };
                    std::string lat = extract("lat"), lon = extract("lon");
                    std::string title = extract("title"), note = extract("note");

                    std::ifstream in(geojsonPath);
                    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
                    in.close();
                    std::string feat = "{\"type\":\"Feature\","
                        "\"geometry\":{\"type\":\"Point\",\"coordinates\":[" + lon + "," + lat + "]},"
                        "\"properties\":{\"title\":\"" + title + "\",\"note\":\"" + note + "\"}}";
                    size_t pos = content.rfind(']');
                    bool has = content.find("\"type\":\"Feature\"") != std::string::npos;
                    content.insert(pos, (has ? "," : "") + feat);
                    std::ofstream out(geojsonPath); out << content;
                    res.set_content(content, "application/json");
                });
                svr.Post("/edit", [geojsonPath](const httplib::Request& req, httplib::Response& res) {
                    auto body = req.body;
                    auto extract = [&](const std::string& key) {
                        size_t p = body.find("\"" + key + "\"");
                        if (p == std::string::npos) return std::string("");
                        p = body.find(':', p) + 1;
                        while (p < body.size() && (body[p]==' '||body[p]=='"')) p++;
                        size_t e = body.find_first_of(",}\"\n", p);
                        return body.substr(p, e - p);
                    };
                    int idx = std::stoi(extract("index"));
                    std::string title = extract("title"), note = extract("note");

                    std::ifstream in(geojsonPath);
                    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
                    in.close();
                    size_t pos = 0;
                    for (int i = 0; i <= idx; ++i) {
                        pos = content.find("\"properties\"", pos);
                        if (pos == std::string::npos) { res.set_content("{}", "application/json"); return; }
                        if (i < idx) pos++;
                    }
                    size_t end = content.find('}', pos);
                    content.replace(pos, end - pos + 1,
                        "\"properties\":{\"title\":\"" + title + "\",\"note\":\"" + note + "\"}");
                    std::ofstream out(geojsonPath); out << content;
                    std::ifstream in2(geojsonPath);
                    std::string updated((std::istreambuf_iterator<char>(in2)), std::istreambuf_iterator<char>());
                    res.set_content(updated, "application/json");
                });
                svr.Post("/remove", [geojsonPath](const httplib::Request& req, httplib::Response& res) {
                    auto body = req.body;
                    size_t p = body.find("\"index\"");
                    int idx = 0;
                    if (p != std::string::npos) {
                        p = body.find(':', p) + 1;
                        idx = std::stoi(body.substr(p));
                    }
                    std::ifstream in(geojsonPath);
                    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
                    in.close();
                    size_t fpos = 0;
                    for (int i = 0; i <= idx; ++i) {
                        fpos = content.find("{\"type\":\"Feature\"", fpos);
                        if (fpos == std::string::npos) { res.set_content(content, "application/json"); return; }
                        if (i < idx) fpos++;
                    }
                    size_t end = fpos; int depth = 0;
                    for (size_t k = fpos; k < content.size(); ++k) {
                        if (content[k]=='{') depth++;
                        else if (content[k]=='}') { depth--; if (!depth) { end = k; break; } }
                    }
                    size_t rs = fpos, re = end + 1;
                    if (re < content.size() && content[re] == ',') re++;
                    else if (rs > 0 && content[rs-1] == ',') rs--;
                    content.erase(rs, re - rs);
                    std::ofstream out(geojsonPath); out << content;
                    std::ifstream in2(geojsonPath);
                    std::string updated((std::istreambuf_iterator<char>(in2)), std::istreambuf_iterator<char>());
                    res.set_content(updated, "application/json");
                });
                std::cout << "Trek map server: http://localhost:" << port << std::endl;
                svr.listen("0.0.0.0", port);
            }).detach();

            // Open in browser
            std::string url = "http://localhost:" + std::to_string(port);
            std::system(("open " + url + " 2>/dev/null || xdg-open " + url + " 2>/dev/null &").c_str());
            return std::make_shared<GemValue>(url);
        }, true };

        // trek.export_gpx(geojson_path, gpx_path) — export GeoJSON waypoints to GPX
        methods["export_gpx"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.size() < 2) return std::make_shared<GemValue>(false);
            std::ifstream in(args[0]->toString());
            std::string gj((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
            std::string gpx = "<?xml version=\"1.0\"?>\n<gpx version=\"1.1\" creator=\"Gem Trek\">\n";
            size_t pos = 0;
            while ((pos = gj.find("\"coordinates\":[", pos)) != std::string::npos) {
                pos += 15;
                size_t end = gj.find(']', pos);
                std::string coords = gj.substr(pos, end - pos);
                size_t comma = coords.find(',');
                std::string lon = coords.substr(0, comma);
                std::string lat = coords.substr(comma + 1);
                // Find associated title
                size_t tp = gj.rfind("\"title\"", pos);
                std::string title = "Waypoint";
                if (tp != std::string::npos) {
                    size_t ts = gj.find('"', tp + 8) + 1;
                    size_t te = gj.find('"', ts);
                    title = gj.substr(ts, te - ts);
                }
                gpx += "  <wpt lat=\"" + lat + "\" lon=\"" + lon + "\"><name>" + title + "</name></wpt>\n";
                pos = end;
            }
            gpx += "</gpx>\n";
            std::ofstream out(args[1]->toString());
            out << gpx;
            return std::make_shared<GemValue>(true);
        }, true };

        // trek.stats(path) — return object with waypoint count and total distance (km)
        methods["stats"] = { [](std::vector<std::shared_ptr<GemValue>> args) {
            if (args.empty()) return std::make_shared<GemValue>(false);
            std::ifstream in(args[0]->toString());
            std::string gj((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

            std::vector<std::pair<double,double>> pts;
            size_t pos = 0;
            while ((pos = gj.find("\"coordinates\":[", pos)) != std::string::npos) {
                pos += 15;
                size_t end = gj.find(']', pos);
                std::string coords = gj.substr(pos, end - pos);
                size_t comma = coords.find(',');
                try {
                    double lon = std::stod(coords.substr(0, comma));
                    double lat = std::stod(coords.substr(comma + 1));
                    pts.push_back({lat, lon});
                } catch(...) {}
                pos = end;
            }

            double totalDist = 0.0;
            for (size_t i = 1; i < pts.size(); ++i) {
                double dlat = (pts[i].first  - pts[i-1].first)  * M_PI / 180.0;
                double dlon = (pts[i].second - pts[i-1].second) * M_PI / 180.0;
                double a = std::sin(dlat/2)*std::sin(dlat/2) +
                           std::cos(pts[i-1].first*M_PI/180.0)*std::cos(pts[i].first*M_PI/180.0)*
                           std::sin(dlon/2)*std::sin(dlon/2);
                totalDist += 12742.0 * std::atan2(std::sqrt(a), std::sqrt(1-a));
            }

            auto obj = std::make_shared<GemObject>("TrekStats");
            obj->set("waypoints", std::make_shared<GemValue>((double)pts.size()));
            obj->set("distance_km", std::make_shared<GemValue>(totalDist));
            return std::make_shared<GemValue>(obj);
        }, true };
    }
};

// GemSEO — Search Engine Optimization: crawl URLs, extract SEO signals, write index
class GemSEO : public GemSys {
public:
    GemSEO() : GemSys() {
        name = "seo";

        // seo.index(urls, output_path) — crawl list of URLs, extract SEO data, write JSON index
        methods["index"] = { [](std::vector<std::shared_ptr<GemValue>> args) -> std::shared_ptr<GemValue> {
            if (args.size() < 2) return std::make_shared<GemValue>(false);

            // Collect URLs from a vector value or single string
            std::vector<std::string> urls;
            if (std::holds_alternative<std::vector<std::shared_ptr<GemValue>>>(args[0]->value)) {
                for (auto& v : std::get<std::vector<std::shared_ptr<GemValue>>>(args[0]->value))
                    urls.push_back(v->toString());
            } else {
                urls.push_back(args[0]->toString());
            }
            std::string outPath = args[1]->toString();

            // Helper: extract first occurrence of a tag's content
            auto extractTag = [](const std::string& html, const std::string& tag) -> std::string {
                std::string open = "<" + tag;
                size_t p = html.find(open);
                if (p == std::string::npos) return "";
                size_t end = html.find('>', p);
                if (end == std::string::npos) return "";
                size_t close = html.find("</" + tag + ">", end);
                if (close == std::string::npos) return "";
                std::string content = html.substr(end + 1, close - end - 1);
                // Strip inner tags
                std::string result;
                bool inTag = false;
                for (char c : content) {
                    if (c == '<') inTag = true;
                    else if (c == '>') inTag = false;
                    else if (!inTag) result += c;
                }
                return result;
            };

            // Helper: extract meta tag content by name/property
            auto extractMeta = [](const std::string& html, const std::string& attr, const std::string& val) -> std::string {
                std::string needle = "name=\"" + val + "\"";
                if (attr == "property") needle = "property=\"" + val + "\"";
                size_t p = html.find(needle);
                if (p == std::string::npos) {
                    // try single quotes
                    needle = "name='" + val + "'";
                    if (attr == "property") needle = "property='" + val + "'";
                    p = html.find(needle);
                }
                if (p == std::string::npos) return "";
                // find content= on same tag (search backwards to <meta and forwards to >)
                size_t tagStart = html.rfind("<meta", p);
                size_t tagEnd   = html.find('>', p);
                if (tagStart == std::string::npos || tagEnd == std::string::npos) return "";
                std::string tag = html.substr(tagStart, tagEnd - tagStart);
                size_t cp = tag.find("content=\"");
                if (cp == std::string::npos) cp = tag.find("content='");
                if (cp == std::string::npos) return "";
                char q = tag[cp + 8];
                size_t vs = cp + 9;
                size_t ve = tag.find(q, vs);
                return ve == std::string::npos ? "" : tag.substr(vs, ve - vs);
            };

            // Helper: count occurrences of a substring
            auto countOccurrences = [](const std::string& text, const std::string& needle) -> int {
                int count = 0;
                size_t pos = 0;
                while ((pos = text.find(needle, pos)) != std::string::npos) { count++; pos += needle.size(); }
                return count;
            };

            // Helper: extract all href links
            auto extractLinks = [](const std::string& html) -> std::vector<std::string> {
                std::vector<std::string> links;
                size_t pos = 0;
                while ((pos = html.find("href=\"", pos)) != std::string::npos) {
                    pos += 6;
                    size_t end = html.find('"', pos);
                    if (end != std::string::npos) {
                        std::string href = html.substr(pos, end - pos);
                        if (!href.empty() && href[0] != '#') links.push_back(href);
                    }
                }
                return links;
            };

            // Helper: extract all h1/h2/h3 headings
            auto extractHeadings = [&](const std::string& html, const std::string& tag) -> std::vector<std::string> {
                std::vector<std::string> headings;
                size_t pos = 0;
                std::string open = "<" + tag;
                std::string close = "</" + tag + ">";
                while ((pos = html.find(open, pos)) != std::string::npos) {
                    size_t end = html.find('>', pos);
                    if (end == std::string::npos) break;
                    size_t cpos = html.find(close, end);
                    if (cpos == std::string::npos) break;
                    std::string content = html.substr(end + 1, cpos - end - 1);
                    // strip tags
                    std::string text;
                    bool inT = false;
                    for (char c : content) {
                        if (c == '<') inT = true;
                        else if (c == '>') inT = false;
                        else if (!inT) text += c;
                    }
                    if (!text.empty()) headings.push_back(text);
                    pos = cpos + close.size();
                }
                return headings;
            };

            // Helper: strip all HTML tags to get plain text
            auto stripHtml = [](const std::string& html) -> std::string {
                std::string text;
                bool inTag = false;
                for (char c : html) {
                    if (c == '<') inTag = true;
                    else if (c == '>') inTag = false;
                    else if (!inTag) text += c;
                }
                return text;
            };

            // Helper: JSON-escape a string
            auto jsonEscape = [](const std::string& s) -> std::string {
                std::string out;
                for (char c : s) {
                    if (c == '"') out += "\\\"";
                    else if (c == '\\') out += "\\\\";
                    else if (c == '\n') out += "\\n";
                    else if (c == '\r') out += "\\r";
                    else if (c == '\t') out += "\\t";
                    else out += c;
                }
                return out;
            };

            std::string json = "[\n";
            bool first = true;

            for (const auto& url : urls) {
                // Parse host and path from URL
                std::string host, path = "/";
                int port = 80;
                std::string u = url;
                if (u.substr(0, 8) == "https://") { port = 443; u = u.substr(8); }
                else if (u.substr(0, 7) == "http://") { u = u.substr(7); }
                size_t slash = u.find('/');
                if (slash != std::string::npos) { host = u.substr(0, slash); path = u.substr(slash); }
                else { host = u; }
                // Handle port in host
                size_t colon = host.find(':');
                if (colon != std::string::npos) { port = std::stoi(host.substr(colon + 1)); host = host.substr(0, colon); }

                // Fetch HTML
                std::string html;
                try {
                    httplib::Client cli(host, port);
                    cli.set_follow_location(true);
                    cli.set_connection_timeout(10);
                    cli.set_read_timeout(10);
                    auto res = cli.Get(path.c_str());
                    if (res && res->status == 200) html = res->body;
                } catch (...) {}

                if (html.empty()) {
                    // fallback: curl
                    std::string cmd = "curl -sL --max-time 10 \"" + url + "\"";
                    FILE* pipe = popen(cmd.c_str(), "r");
                    if (pipe) {
                        char buf[4096];
                        while (fgets(buf, sizeof(buf), pipe)) html += buf;
                        pclose(pipe);
                    }
                }

                // Extract SEO signals
                std::string title       = extractTag(html, "title");
                std::string desc        = extractMeta(html, "name", "description");
                std::string keywords    = extractMeta(html, "name", "keywords");
                std::string ogTitle     = extractMeta(html, "property", "og:title");
                std::string ogDesc      = extractMeta(html, "property", "og:description");
                std::string canonical;
                {
                    size_t cp = html.find("rel=\"canonical\"");
                    if (cp == std::string::npos) cp = html.find("rel='canonical'");
                    if (cp != std::string::npos) {
                        size_t ts = html.rfind("<link", cp);
                        size_t te = html.find('>', cp);
                        if (ts != std::string::npos && te != std::string::npos) {
                            std::string tag = html.substr(ts, te - ts);
                            size_t hp = tag.find("href=\"");
                            if (hp != std::string::npos) {
                                size_t hs = hp + 6, he = tag.find('"', hs);
                                if (he != std::string::npos) canonical = tag.substr(hs, he - hs);
                            }
                        }
                    }
                }

                auto h1s = extractHeadings(html, "h1");
                auto h2s = extractHeadings(html, "h2");
                auto h3s = extractHeadings(html, "h3");
                auto links = extractLinks(html);
                std::string plainText = stripHtml(html);
                int wordCount = 0;
                bool inWord = false;
                for (char c : plainText) {
                    if (std::isalnum(c)) { if (!inWord) { wordCount++; inWord = true; } }
                    else inWord = false;
                }
                int imgCount = countOccurrences(html, "<img");
                int imgAlt   = countOccurrences(html, "alt=\"");
                int internalLinks = 0, externalLinks = 0;
                for (auto& lnk : links) {
                    if (lnk.find("http") == 0 && lnk.find(host) == std::string::npos) externalLinks++;
                    else internalLinks++;
                }

                // Build JSON entry
                if (!first) json += ",\n";
                first = false;
                json += "  {\n";
                json += "    \"url\": \"" + jsonEscape(url) + "\",\n";
                json += "    \"title\": \"" + jsonEscape(title) + "\",\n";
                json += "    \"description\": \"" + jsonEscape(desc) + "\",\n";
                json += "    \"keywords\": \"" + jsonEscape(keywords) + "\",\n";
                json += "    \"og_title\": \"" + jsonEscape(ogTitle) + "\",\n";
                json += "    \"og_description\": \"" + jsonEscape(ogDesc) + "\",\n";
                json += "    \"canonical\": \"" + jsonEscape(canonical) + "\",\n";
                json += "    \"word_count\": " + std::to_string(wordCount) + ",\n";
                json += "    \"img_count\": " + std::to_string(imgCount) + ",\n";
                json += "    \"img_with_alt\": " + std::to_string(imgAlt) + ",\n";
                json += "    \"internal_links\": " + std::to_string(internalLinks) + ",\n";
                json += "    \"external_links\": " + std::to_string(externalLinks) + ",\n";
                // h1/h2/h3 arrays
                auto jsonArr = [&jsonEscape](const std::vector<std::string>& v) {
                    std::string s = "[";
                    for (size_t i = 0; i < v.size(); ++i) {
                        if (i) s += ", ";
                        s += "\"" + jsonEscape(v[i]) + "\"";
                    }
                    return s + "]";
                };
                json += "    \"h1\": " + jsonArr(h1s) + ",\n";
                json += "    \"h2\": " + jsonArr(h2s) + ",\n";
                json += "    \"h3\": " + jsonArr(h3s) + "\n";
                json += "  }";

                std::cout << "seo.index: crawled " << url << " (" << wordCount << " words)" << std::endl;
            }

            json += "\n]\n";
            std::ofstream out(outPath);
            out << json;
            return std::make_shared<GemValue>(outPath);
        }, true };

        // seo.analyze(index_path) — load index JSON and print SEO report to stdout
        methods["analyze"] = { [](std::vector<std::shared_ptr<GemValue>> args) -> std::shared_ptr<GemValue> {
            if (args.empty()) return std::make_shared<GemValue>(false);
            std::ifstream in(args[0]->toString());
            if (!in.good()) return std::make_shared<GemValue>(false);
            std::string json((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

            std::cout << "\n=== Gem SEO Analysis Report ===" << std::endl;

            // Simple extraction of url/title/description/word_count per entry
            size_t pos = 0;
            int entry = 0;
            auto field = [&](const std::string& key) -> std::string {
                size_t kp = json.find("\"" + key + "\": \"", pos);
                if (kp == std::string::npos) kp = json.find("\"" + key + "\":", pos);
                if (kp == std::string::npos) return "";
                size_t vs = json.find('"', kp + key.size() + 4);
                if (vs == std::string::npos) return "";
                size_t ve = json.find('"', vs + 1);
                return ve == std::string::npos ? "" : json.substr(vs + 1, ve - vs - 1);
            };
            auto numField = [&](const std::string& key) -> int {
                size_t kp = json.find("\"" + key + "\": ", pos);
                if (kp == std::string::npos) return 0;
                size_t vs = kp + key.size() + 4;
                size_t ve = json.find_first_of(",\n}", vs);
                try { return std::stoi(json.substr(vs, ve - vs)); } catch (...) { return 0; }
            };

            while ((pos = json.find("\"url\":", pos)) != std::string::npos) {
                entry++;
                std::string url   = field("url");
                std::string title = field("title");
                std::string desc  = field("description");
                int words = numField("word_count");
                int imgs  = numField("img_count");
                int alts  = numField("img_with_alt");
                int ilinks = numField("internal_links");
                int elinks = numField("external_links");

                std::cout << "\n[" << entry << "] " << url << std::endl;
                std::cout << "  Title:       " << (title.empty() ? "MISSING" : title)
                          << (title.size() > 60 ? " [TOO LONG >60]" : "") << std::endl;
                std::cout << "  Description: " << (desc.empty() ? "MISSING" : desc.substr(0, 80))
                          << (desc.size() > 160 ? " [TOO LONG >160]" : "") << std::endl;
                std::cout << "  Words:       " << words
                          << (words < 300 ? " [LOW <300]" : "") << std::endl;
                std::cout << "  Images:      " << imgs << "  (with alt: " << alts
                          << (imgs > 0 && alts < imgs ? " [MISSING ALT]" : "") << ")" << std::endl;
                std::cout << "  Links:       internal=" << ilinks << "  external=" << elinks << std::endl;
                pos++;
            }
            std::cout << "\nTotal pages analyzed: " << entry << std::endl;
            return std::make_shared<GemValue>((double)entry);
        }, true };
    }
};

// GemMobl — Mobile & Cross-Platform PWA builtin
class GemMobl : public GemSys {
public:
    std::string deviceName;

    GemMobl() : GemSys() {
        name = "mobl";

        // mobl phone("device_name") — instantiate a phone object
        methods["phone"] = { [](std::vector<std::shared_ptr<GemValue>> args) -> std::shared_ptr<GemValue> {
            std::string dev = args.empty() ? "phone" : args[0]->toString();
            auto phone = std::make_shared<GemMobl>();
            phone->deviceName = dev;
            phone->name = dev;
            return std::make_shared<GemValue>(std::static_pointer_cast<GemObject>(phone));
        }, true };

        // phone.dictate(spoken_text) — NLP parse via ai.prompt → JSON {title, note, tags}
        methods["dictate"] = { [](std::vector<std::shared_ptr<GemValue>> args) -> std::shared_ptr<GemValue> {
            if (args.empty()) return std::make_shared<GemValue>("{}");
            std::string spoken = args[0]->toString();
            // Build a structured JSON from the spoken text (stub; real impl uses ai.prompt)
            std::string json = "{\"title\": \"Note\", \"note\": \"" + spoken + "\", \"tags\": []}";
            return std::make_shared<GemValue>(json);
        }, true };

        // phone.make_feature(lat, lon, text) — GPS + dictation → GeoJSON Feature
        methods["make_feature"] = { [](std::vector<std::shared_ptr<GemValue>> args) -> std::shared_ptr<GemValue> {
            if (args.size() < 3) return std::make_shared<GemValue>("{}");
            std::string lat = args[0]->toString();
            std::string lon = args[1]->toString();
            std::string text = args[2]->toString();
            std::string feature =
                "{\"type\":\"Feature\","
                "\"geometry\":{\"type\":\"Point\",\"coordinates\":[" + lon + "," + lat + "]},"
                "\"properties\":{\"note\":\"" + text + "\"}}";
            return std::make_shared<GemValue>(feature);
        }, true };
    }
};

#include "gem_drvr.hpp"
#include "gem_astro.hpp"
#include "gem_art.hpp"

#endif
