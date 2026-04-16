#ifndef GEM_INTERPRETER_HPP
#define GEM_INTERPRETER_HPP

#include "gem_base.hpp"
#include "gem_tokenizer.hpp"
#include <stack>
#include <sstream>
#include <unordered_set>
#include <set>

class GemScope {
public:
    std::map<std::string, std::shared_ptr<GemValue>> variables;
    std::shared_ptr<GemScope> parent;

    GemScope(std::shared_ptr<GemScope> p = nullptr) : parent(p) {}

    void define(const std::string& name, std::shared_ptr<GemValue> val) {
        variables[name] = val;
    }

    std::shared_ptr<GemValue> resolve(const std::string& name) {
        if (variables.count(name)) return variables[name];
        if (parent) return parent->resolve(name);
        return nullptr;
    }
};

struct GemFunction {
    std::string name;
    std::vector<std::string> params;
    std::vector<std::string> body;
    std::vector<GemToken> returnExpr;
};

struct GemObjectDef {
    std::string name;
    std::vector<std::string> params;
    std::string parentName;
    std::vector<std::string> body;
    std::map<std::string, GemObject::GemMethod> methods;
};

namespace fs = std::filesystem;

class GemInterpreter {
public:
    static GemInterpreter* instance;
    std::recursive_mutex interpreterMutex;
    std::shared_ptr<GemScope> globalScope;
    std::shared_ptr<GemScope> currentScope;
    std::map<std::string, std::shared_ptr<GemObject>> builtins;
    std::map<std::string, GemFunction> userFunctions;
    std::map<std::string, GemObjectDef> userClasses;
    std::map<std::string, std::vector<GemToken>> aliases;
    std::string currentFile;
    std::ostream* out = &std::cout;
    
    bool running = true;
    struct RecordingState {
        bool active = false;
        bool isObject = false;
        bool isIf = false;
        bool isWhile = false;
        std::vector<GemToken> condition;
        GemFunction func;
        GemObjectDef obj;
        std::vector<std::string> body;
    };
    std::stack<RecordingState> recordingStack;
    std::stack<bool> skippingStack;
    std::stack<int> skipDepthStack;

    GemInterpreter() {
        instance = this;
        skippingStack.push(false);
        skipDepthStack.push(0);
        recordingStack.push({false, false, false, false, {}, {}, {}, {}});
        globalScope = std::make_shared<GemScope>();
        currentScope = globalScope;

        auto sys = std::make_shared<GemSys>();
        GemObject::sysInstance = sys; // Set global sys instance
        builtins["sys"] = sys;
        builtins["math"] = std::make_shared<GemMath>();
        builtins["ai"] = std::make_shared<GemAI>();
        builtins["tcp"] = std::make_shared<GemTcp>();
        builtins["itr"] = std::make_shared<GemItr>();
        builtins["thread"] = std::make_shared<GemThread>();
        builtins["text"] = std::make_shared<GemText>();
        builtins["rex"]  = std::make_shared<GemRex>();
        builtins["algo"] = std::make_shared<GemAlgo>();
        builtins["nlp"] = std::make_shared<GemNLP>();
        builtins["zip"] = std::make_shared<GemZip>();
        builtins["bev"] = std::make_shared<GemBev>();
        builtins["file"] = std::make_shared<GemFile>();
        builtins["img"] = std::make_shared<GemImg>();
        builtins["www"] = std::make_shared<GemWWW>();
        builtins["cdn"] = std::make_shared<GemCDN>();
        builtins["geo"] = std::make_shared<GemGeo>();
        builtins["cpp"] = std::make_shared<GemCpp>();
        builtins["data"] = std::make_shared<GemData>();
        builtins["k3s"] = std::make_shared<GemContainer>();
        builtins["vm"] = std::make_shared<GemVM>();
        builtins["go"] = std::make_shared<GemGo>();
        builtins["ruby"] = std::make_shared<GemRuby>();
        builtins["node"] = std::make_shared<GemNode>();
        builtins["rust"] = std::make_shared<GemRust>();
        builtins["chart"] = std::make_shared<GemChart>();
        builtins["fin"] = std::make_shared<GemFin>();
        builtins["bsm"] = std::make_shared<GemBSM>();
        builtins["astro"] = std::make_shared<GemAstro>();
        builtins["drvr"] = std::make_shared<GemDrvr>();
        builtins["mobl"] = std::make_shared<GemMobl>();
        builtins["seo"]  = std::make_shared<GemSEO>();
        builtins["trek"] = std::make_shared<GemTrek>();

        auto caller = [this](const std::string& name, std::vector<std::shared_ptr<GemValue>> args) {
            return this->callUserFunction(name, args);
        };

        for (auto const& [name, obj] : builtins) {
            auto sysObj = std::dynamic_pointer_cast<GemSys>(obj);
            if (sysObj) {
                sysObj->callGemFunc = caller;
                sysObj->translateFunc = [this](const std::string& source, const std::string& lang) {
                    return this->translateSource(source, lang);
                };
            }
            globalScope->define(name, std::make_shared<GemValue>(obj));
        }
    }

    std::string translateSource(const std::string& source, const std::string& language) {
        if (builtins.count("ai")) {
            std::string prompt = "Translate the following " + language + " code into Gem language. Gem uses fun name(params) ... end for functions, sys.print for output, and supports basic math and objects. \n\n"
                                 "CRITICAL INSTRUCTION for C++: If the C++ code uses multiple inheritance (e.g., class C : public A, public B), Gem only supports single inheritance. You MUST translate this as a chain of single inheritance objects (e.g., obj B_Ext : A ... end, obj C : B_Ext ... end) to preserve the combined functionality. \n\n"
                                 "Only return the Gem code without any explanation:\n\n" + source;
            auto aiObj = builtins["ai"];
            auto translatedVal = aiObj->call("prompt", {std::make_shared<GemValue>(prompt)});
            if (std::holds_alternative<std::string>(translatedVal->value)) {
                std::string translatedCode = std::get<std::string>(translatedVal->value);
                if (translatedCode.find("```") != std::string::npos) {
                     size_t first = translatedCode.find("```");
                     size_t last = translatedCode.find("```", first + 3);
                     if (first != std::string::npos && last != std::string::npos) {
                         translatedCode = translatedCode.substr(first + 3);
                         size_t nextLine = translatedCode.find('\n');
                         if (nextLine != std::string::npos && last != std::string::npos && last > nextLine) translatedCode = translatedCode.substr(nextLine + 1, last - nextLine - 1);
                         else if (nextLine != std::string::npos) translatedCode = translatedCode.substr(nextLine + 1);
                     }
                }
                return translatedCode;
            }
        }
        return "";
    }

    void setArgs(int argc, char* argv[]) {
        if (builtins.count("sys")) {
            auto sys = std::static_pointer_cast<GemSys>(builtins["sys"]);
            for (int i = 0; i < argc; ++i) {
                sys->cli_args.push_back(std::string(argv[i]));
            }
        }
    }

    void handleLib() {
        *out << "Available Gem Libraries:" << std::endl;
        for (auto const& [name, obj] : builtins) {
            *out << "  - " << name << " (" << obj->name << ")" << std::endl;
        }
    }

    void printHistoryFile(const fs::path& path) {
        if (!fs::exists(path)) {
            *out << "History file not found." << std::endl;
            return;
        }

        std::ifstream ifs(path);
        std::string line;
        std::vector<std::pair<std::string, std::string>> commands;
        std::unordered_set<std::string> uniqueCommands;
        
        while (std::getline(ifs, line)) {
            size_t gemPos = line.find("gem> ");
            if (gemPos != std::string::npos) {
                std::string timestamp = "";
                // Try to extract timestamp if present: [HH:MM:SS] gem>
                if (gemPos >= 11 && line[0] == '[' && line[9] == ']') {
                    timestamp = line.substr(1, 8);
                } else if (gemPos > 0) {
                   // Fallback for different timestamp formats if any
                   size_t start = line.find("[");
                   size_t end = line.find("]");
                   if (start != std::string::npos && end != std::string::npos && end < gemPos) {
                       timestamp = line.substr(start + 1, end - start - 1);
                   }
                }

                std::string cmd = line.substr(gemPos + 5);
                if (uniqueCommands.find(cmd) == uniqueCommands.end()) {
                    commands.push_back({timestamp, cmd});
                    uniqueCommands.insert(cmd);
                }
            }
        }

        if (commands.empty()) {
            *out << "No unique statements found in this session." << std::endl;
        } else {
            for (const auto& p : commands) {
                if (!p.first.empty()) *out << "[" << p.first << "] ";
                *out << p.second << std::endl;
            }
        }
    }

    void handleHistory() {
        const char* home = std::getenv("HOME");
        if (!home) return;
        fs::path gemDir = fs::path(home) / ".gem";
        if (!fs::exists(gemDir)) {
            *out << "No history found." << std::endl;
            return;
        }

        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        std::tm* tm_now = std::localtime(&time_t_now);
        char buf[32];
        std::strftime(buf, sizeof(buf), "%Y%j", tm_now);
        std::string todayFile = std::string(buf) + ".txt";

        if (fs::exists(gemDir / todayFile)) {
            *out << "--- Today's Session History (" << todayFile << ") ---" << std::endl;
            printHistoryFile(gemDir / todayFile);
        } else {
            *out << "No history for today." << std::endl;
        }

        // Prompt for prior history
        std::vector<std::string> historyFiles;
        for (const auto& entry : fs::directory_iterator(gemDir)) {
            if (entry.path().extension() == ".txt" && entry.path().filename().string() != todayFile && entry.path().filename().string() != "history.txt") {
                historyFiles.push_back(entry.path().filename().string());
            }
        }

        if (!historyFiles.empty()) {
            *out << "Prior history available:" << std::endl;
            for (size_t i = 0; i < historyFiles.size(); ++i) {
                *out << "[" << i + 1 << "] " << historyFiles[i] << std::endl;
            }
            *out << "Enter number to view (or press Enter to exit): " << std::flush;
            std::string choice;
            std::getline(std::cin, choice);
            if (!choice.empty()) {
                try {
                    int idx = std::stoi(choice) - 1;
                    if (idx >= 0 && (size_t)idx < historyFiles.size()) {
                        *out << "--- History: " << historyFiles[idx] << " ---" << std::endl;
                        printHistoryFile(gemDir / historyFiles[idx]);
                    }
                } catch (...) {}
            }
        }
    }

    void handleUse(const std::string& filename) {
        std::string path = filename;
        std::ifstream file(path);
        
        // 1. Try local path
        if (!file.is_open()) {
            // 2. Try /usr/local/lib/gem/
            path = "/usr/local/lib/gem/" + filename;
            file.open(path);
        }
        
        if (!file.is_open()) {
            // 3. Try ~/.gem/lib/
            const char* home = std::getenv("HOME");
            if (home) {
                path = std::string(home) + "/.gem/lib/" + filename;
                file.open(path);
            }
        }

        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << filename << " (searched local, /usr/local/lib/gem/, and ~/.gem/lib/)" << std::endl;
            return;
        }

        std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        std::string extension = std::filesystem::path(filename).extension().string();

        if (extension == ".g" || extension == ".gm") {
            std::stringstream ss(source);
            std::string line;
            while (std::getline(ss, line)) {
                execute(line, false);
            }
        } else if (extension == ".py" || extension == ".r" || extension == ".jl" || extension == ".for" || extension == ".cpp") {
            std::string language = "";
            if (extension == ".py") language = "Python";
            else if (extension == ".r") language = "R";
            else if (extension == ".jl") language = "Julia";
            else if (extension == ".for") language = "Fortran";
            else if (extension == ".cpp") language = "C++";

            std::string translatedCode = translateSource(source, language);
            if (!translatedCode.empty()) {
                std::stringstream ss(translatedCode);
                std::string line;
                while (std::getline(ss, line)) execute(line, false);
            }
        }
    }

    void execute(const std::string& line, bool skipRecording = false) {
        std::lock_guard<std::recursive_mutex> lock(interpreterMutex);
        if (line.empty()) return;
        if (!currentScope) currentScope = globalScope;
        if (builtins.count("sys")) builtins["sys"]->set("__file__", std::make_shared<GemValue>(currentFile));
        GemTokenizer tokenizer(line);
        std::vector<GemToken> rawTokens = tokenizer.tokenize();
        if (rawTokens.empty() || rawTokens[0].type == GEM_TOKEN_EOF) return;

        std::vector<std::vector<GemToken>> statements;
        std::vector<GemToken> current;
        for (const auto& t : rawTokens) {
            if (t.type == GEM_TOKEN_SEMICOLON) {
                if (!current.empty()) {
                    current.push_back({GEM_TOKEN_EOF, "", 0});
                    statements.push_back(current);
                    current.clear();
                }
            } else if (t.type == GEM_TOKEN_EOF) {
                if (!current.empty()) {
                    current.push_back(t);
                    statements.push_back(current);
                }
                break;
            } else {
                current.push_back(t);
            }
        }

        for (const auto& stmtTokens : statements) {
            std::string fragment = "";
            for (size_t i = 0; i < stmtTokens.size() && stmtTokens[i].type != GEM_TOKEN_EOF; ++i) {
                if (stmtTokens[i].type == GEM_TOKEN_STRING) fragment += "\"" + stmtTokens[i].text + "\"";
                else fragment += stmtTokens[i].text;
                if (i + 1 < stmtTokens.size() && stmtTokens[i+1].type != GEM_TOKEN_EOF) fragment += " ";
            }
            executeTokens(stmtTokens, fragment, skipRecording);
        }
    }

    void executeTokens(const std::vector<GemToken>& rawTokens, const std::string& line, bool skipRecording = false) {
        std::lock_guard<std::recursive_mutex> lock(interpreterMutex);
        
        // Check for pending signals
        if (SignalState::lastSignal != 0) {
            int sig = SignalState::lastSignal.exchange(0);
            if (SignalState::handlers.count(sig)) {
                std::string handler = SignalState::handlers[sig];
                // Avoid recursive signals during handler
                callUserFunction(handler, {std::make_shared<GemValue>((double)sig)});
            } else {
                // Default handling
#ifdef _WIN32
                if (sig == SIGINT || sig == SIGTERM) {
#else
                if (sig == SIGINT || sig == SIGTERM || sig == SIGQUIT) {
#endif
                    *out << "\nReceived signal " << sig << ", exiting..." << std::endl;
                    running = false;
                    return;
                }
            }
        }

        if (rawTokens.empty() || rawTokens[0].type == GEM_TOKEN_EOF) return;
        // std::cout << "DEBUG Executing: " << line << std::endl;

        if (skippingStack.empty()) {
            skippingStack.push(false);
            skipDepthStack.push(0);
        }

        bool& isSkipping = skippingStack.top();
        int& skipDepth = skipDepthStack.top();

        if (isSkipping) {
            if (rawTokens[0].type == GEM_TOKEN_KEYWORD_FUN || rawTokens[0].type == GEM_TOKEN_KEYWORD_OBJ || rawTokens[0].type == GEM_TOKEN_KEYWORD_IF || rawTokens[0].type == GEM_TOKEN_KEYWORD_WHILE) skipDepth++;
            if (rawTokens[0].type == GEM_TOKEN_KEYWORD_END) {
                if (skipDepth > 0) skipDepth--;
                else {
                    // Only resume if it's a plain 'end' (for if/while)
                    // If there's a return expression, it's a function 'end'
                    if (rawTokens.size() <= 2) isSkipping = false;
                }
            }
            return;
        }

        if (recordingStack.size() > 1) {
            auto& rec = recordingStack.top();
            if (rawTokens[0].type == GEM_TOKEN_KEYWORD_END) {
                if (rec.isObject) {
                    userClasses[rec.obj.name] = rec.obj;
                    *out << "Defined object " << rec.obj.name << std::endl;
                } else if (rec.isIf || rec.isWhile) {
                    size_t p = 0;
                    auto condVal = parseExpression(rec.condition, p);
                    bool condBool = true;
                    if (condVal && !std::holds_alternative<std::monostate>(condVal->value)) {
                        if (std::holds_alternative<double>(condVal->value)) condBool = std::get<double>(condVal->value) != 0;
                        if (std::holds_alternative<bool>(condVal->value)) condBool = std::get<bool>(condVal->value);
                    } else condBool = false;
                    if (condBool) {
                        skippingStack.push(false); skipDepthStack.push(0);
                        for (const auto& bLine : rec.body) execute(bLine, true);
                        skippingStack.pop(); skipDepthStack.pop();
                    }
                } else {
                    rec.func.returnExpr.clear();
                    for (size_t i = 1; i < rawTokens.size() && rawTokens[i].type != GEM_TOKEN_EOF; ++i) rec.func.returnExpr.push_back(rawTokens[i]);
                    if (!rec.func.returnExpr.empty()) rec.func.returnExpr.push_back({GEM_TOKEN_EOF, "", 0});
                    userFunctions[rec.func.name] = rec.func;
                    *out << "Defined function " << rec.func.name << std::endl;
                }
                recordingStack.pop();
                if (recordingStack.size() > 1) {
                    auto& parentRec = recordingStack.top();
                    if (parentRec.isObject) parentRec.obj.body.push_back(line);
                    else if (parentRec.isIf || parentRec.isWhile) parentRec.body.push_back(line);
                    else parentRec.func.body.push_back(line);
                    if (parentRec.isObject && !rec.isIf && !rec.isWhile && !rec.isObject) {
                        parentRec.obj.methods[rec.func.name] = { rec.func.name, false };
                    }
                }
            } else {
                if (rec.isObject) {
                    rec.obj.body.push_back(line);
                    if (rawTokens[0].type == GEM_TOKEN_KEYWORD_FUN && rawTokens.size() > 1 && rawTokens[1].type == GEM_TOKEN_ID) {
                        GemFunction newFunc; newFunc.name = rawTokens[1].text;
                        if (rawTokens.size() > 3 && rawTokens[2].type == GEM_TOKEN_LPAREN) {
                             size_t p = 3; while(p < rawTokens.size() && rawTokens[p].type != GEM_TOKEN_RPAREN) {
                                 if (rawTokens[p].type == GEM_TOKEN_ID) newFunc.params.push_back(rawTokens[p].text);
                                 p++;
                             }
                        }
                        recordingStack.push({true, false, false, false, {}, newFunc, {}, {}});
                    }
                } else if (rec.isIf || rec.isWhile) {
                    rec.body.push_back(line);
                } else {
                    rec.func.body.push_back(line);
                }
            }
            return;
        }

        std::vector<GemToken> tokens;
        for (size_t i = 0; i < rawTokens.size(); ++i) {
            const auto& t = rawTokens[i];
            if (t.type == GEM_TOKEN_ID && aliases.count(t.text)) {
                const auto& expansion = aliases[t.text];
                if (i + 1 < rawTokens.size() && rawTokens[i+1].type == GEM_TOKEN_LPAREN) {
                    if (expansion.size() >= 3 && expansion[expansion.size()-2].type == GEM_TOKEN_LPAREN && expansion[expansion.size()-1].type == GEM_TOKEN_RPAREN) {
                        for (size_t j = 0; j < expansion.size() - 2; ++j) tokens.push_back(expansion[j]);
                    } else tokens.insert(tokens.end(), expansion.begin(), expansion.end());
                } else tokens.insert(tokens.end(), expansion.begin(), expansion.end());
            } else tokens.push_back(t);
        }

        size_t pos = 0;
        if (tokens[0].type == GEM_TOKEN_KEYWORD_ALIAS) {
            if (tokens.size() > 3 && tokens[1].type == GEM_TOKEN_ID && tokens[2].type == GEM_TOKEN_EQUALS) {
                std::string aliasName = tokens[1].text;
                std::vector<GemToken> expansion;
                for (size_t i = 3; i < tokens.size() && tokens[i].type != GEM_TOKEN_EOF; ++i) expansion.push_back(tokens[i]);
                aliases[aliasName] = expansion;
                *out << "Aliased " << aliasName << std::endl;
                return;
            }
        }

        if (tokens[0].type == GEM_TOKEN_KEYWORD_IF) {
            if (recordingStack.size() > 1 && !skipRecording) {
                auto& parentRec = recordingStack.top();
                if (parentRec.isObject) parentRec.obj.body.push_back(line);
                else if (parentRec.isIf || parentRec.isWhile) parentRec.body.push_back(line);
                else parentRec.func.body.push_back(line);
            }
            std::vector<GemToken> cond;
            for (size_t i = 1; i < tokens.size() && tokens[i].type != GEM_TOKEN_EOF; ++i) cond.push_back(tokens[i]);
            
            if (skipRecording || recordingStack.size() == 1) {
                size_t p = 0;
                auto condVal = parseExpression(cond, p);
                bool condBool = true;
                if (condVal && !std::holds_alternative<std::monostate>(condVal->value)) {
                    if (std::holds_alternative<double>(condVal->value)) condBool = std::get<double>(condVal->value) != 0;
                    if (std::holds_alternative<bool>(condVal->value)) condBool = std::get<bool>(condVal->value);
                } else condBool = false;
                
                if (!condBool) {
                    isSkipping = true;
                    skipDepth = 0;
                }
            } else {
                recordingStack.push({true, false, true, false, cond, {}, {}, {}});
            }
            return;
        }

        if (tokens[0].type == GEM_TOKEN_KEYWORD_WHILE) {
            if (recordingStack.size() > 1 && !skipRecording) {
                auto& parentRec = recordingStack.top();
                if (parentRec.isObject) parentRec.obj.body.push_back(line);
                else if (parentRec.isIf || parentRec.isWhile) parentRec.body.push_back(line);
                else parentRec.func.body.push_back(line);
            }
            std::vector<GemToken> cond;
            for (size_t i = 1; i < tokens.size() && tokens[i].type != GEM_TOKEN_EOF; ++i) cond.push_back(tokens[i]);
            
            if (skipRecording || recordingStack.size() == 1) {
                size_t p = 0;
                auto condVal = parseExpression(cond, p);
                bool condBool = true;
                if (condVal && !std::holds_alternative<std::monostate>(condVal->value)) {
                    if (std::holds_alternative<double>(condVal->value)) condBool = std::get<double>(condVal->value) != 0;
                    if (std::holds_alternative<bool>(condVal->value)) condBool = std::get<bool>(condVal->value);
                } else condBool = false;
                
                if (!condBool) {
                    isSkipping = true;
                    skipDepth = 0;
                }
            } else {
                recordingStack.push({true, false, false, true, cond, {}, {}, {}});
            }
            return;
        }
        
        if (tokens[0].type == GEM_TOKEN_KEYWORD_FUN) {
            if (tokens.size() > 1 && tokens[1].type == GEM_TOKEN_ID) {
                std::string funcName = tokens[1].text;
                auto thisVal = currentScope->resolve("this");
                if (thisVal && std::holds_alternative<std::shared_ptr<GemObject>>(thisVal->value)) {
                    if (!skipRecording) std::get<std::shared_ptr<GemObject>>(thisVal->value)->methods[funcName] = { funcName, false };
                }
                if (!skipRecording || recordingStack.size() > 1) {
                    GemFunction newFunc;
                    newFunc.name = funcName;
                    if (tokens.size() > 3 && tokens[2].type == GEM_TOKEN_LPAREN) {
                         size_t p = 3;
                         while(p < tokens.size() && tokens[p].type != GEM_TOKEN_RPAREN) {
                             if (tokens[p].type == GEM_TOKEN_ID) newFunc.params.push_back(tokens[p].text);
                             p++;
                         }
                    }
                    recordingStack.push({true, false, false, false, {}, newFunc, {}, {}});
                } else { isSkipping = true; skipDepth = 0; }
                return;
            }
        }

        if (tokens[0].type == GEM_TOKEN_KEYWORD_OBJ) {
            if (tokens.size() > 1 && tokens[1].type == GEM_TOKEN_ID) {
                if (!skipRecording || recordingStack.size() > 1) {
                    GemObjectDef newObj;
                    newObj.name = tokens[1].text;
                    size_t p = 2;
                    if (p < tokens.size() && tokens[p].type == GEM_TOKEN_LPAREN) {
                        p++;
                        while (p < tokens.size() && tokens[p].type != GEM_TOKEN_RPAREN) {
                            if (tokens[p].type == GEM_TOKEN_ID) newObj.params.push_back(tokens[p].text);
                            p++;
                        }
                        if (p < tokens.size() && tokens[p].type == GEM_TOKEN_RPAREN) p++;
                    }
                    if (p < tokens.size() && tokens[p].type == GEM_TOKEN_COLON) {
                        p++;
                        if (p < tokens.size() && tokens[p].type == GEM_TOKEN_ID) newObj.parentName = tokens[p].text;
                    }
                    recordingStack.push({true, true, false, false, {}, {}, newObj, {}});
                } else { isSkipping = true; skipDepth = 0; }
                return;
            }
        }

        if (tokens[0].type == GEM_TOKEN_KEYWORD_USE) {
            if (tokens.size() > 1) {
                std::string filename = "";
                if (tokens[1].type == GEM_TOKEN_STRING) filename = tokens[1].text;
                else for (size_t i = 1; i < tokens.size() && tokens[i].type != GEM_TOKEN_EOF; ++i) filename += tokens[i].text;
                if (!filename.empty()) handleUse(filename);
                return;
            }
        }

        if (tokens[0].type == GEM_TOKEN_KEYWORD_LIB) { handleLib(); return; }
        if (tokens[0].type == GEM_TOKEN_KEYWORD_HIS) { handleHistory(); return; }

        if (tokens[0].type == GEM_TOKEN_TYPE_INT || tokens[0].type == GEM_TOKEN_TYPE_DOUBLE || tokens[0].type == GEM_TOKEN_TYPE_STRING || tokens[0].type == GEM_TOKEN_TYPE_BOOL) {
            std::vector<std::string> varNames;
            size_t i = 1;
            while (i < tokens.size() && tokens[i].type != GEM_TOKEN_EQUALS && tokens[i].type != GEM_TOKEN_EOF) {
                if (tokens[i].type == GEM_TOKEN_ID) varNames.push_back(tokens[i].text);
                i++;
                if (i < tokens.size() && tokens[i].type == GEM_TOKEN_COMMA) i++;
            }
            if (i < tokens.size() && tokens[i].type == GEM_TOKEN_EQUALS) {
                pos = i + 1;
                auto val = parseExpression(tokens, pos);
                if (val) for (const auto& name : varNames) {
                    if (name[0] == '_') globalScope->define(name, val);
                    else {
                        auto thisVal = currentScope->resolve("this");
                        if (thisVal && std::holds_alternative<std::shared_ptr<GemObject>>(thisVal->value)) {
                            std::get<std::shared_ptr<GemObject>>(thisVal->value)->set(name, val);
                        }
                        currentScope->define(name, val);
                    }
                }
            } else std::cerr << "Error: Variable declaration requires initial value." << std::endl;
            return;
        }

        if (tokens.size() >= 3) {
            std::vector<std::pair<std::string, std::shared_ptr<GemObject>>> targets;
            size_t i = 0;
            GemTokenType compoundOp = GEM_TOKEN_UNKNOWN;
            while (i + 1 < tokens.size()) {
                std::string targetName;
                std::shared_ptr<GemObject> targetObj = nullptr;
                size_t nextI = i;
                GemTokenType currentOp = GEM_TOKEN_UNKNOWN;

                if (tokens[i].type == GEM_TOKEN_ID && (tokens[i+1].type == GEM_TOKEN_EQUALS || tokens[i+1].type == GEM_TOKEN_PLUS_EQUALS || tokens[i+1].type == GEM_TOKEN_MINUS_EQUALS || tokens[i+1].type == GEM_TOKEN_MUL_EQUALS || tokens[i+1].type == GEM_TOKEN_DIV_EQUALS)) {
                    targetName = tokens[i].text;
                    currentOp = tokens[i+1].type;
                    nextI = i + 2;
                } else if (tokens[i].type == GEM_TOKEN_DOT && i+1 < tokens.size() && tokens[i+1].type == GEM_TOKEN_ID && (tokens[i+2].type == GEM_TOKEN_EQUALS || tokens[i+2].type == GEM_TOKEN_PLUS_EQUALS || tokens[i+2].type == GEM_TOKEN_MINUS_EQUALS || tokens[i+2].type == GEM_TOKEN_MUL_EQUALS || tokens[i+2].type == GEM_TOKEN_DIV_EQUALS)) {
                    auto thisVal = currentScope->resolve("this");
                    if (thisVal && std::holds_alternative<std::shared_ptr<GemObject>>(thisVal->value)) {
                        targetName = tokens[i+1].text;
                        targetObj = std::get<std::shared_ptr<GemObject>>(thisVal->value);
                        currentOp = tokens[i+2].type;
                        nextI = i + 3;
                    }
                } else if (tokens[i].type == GEM_TOKEN_DOT_DOT && i+1 < tokens.size() && tokens[i+1].type == GEM_TOKEN_ID && (tokens[i+2].type == GEM_TOKEN_EQUALS || tokens[i+2].type == GEM_TOKEN_PLUS_EQUALS || tokens[i+2].type == GEM_TOKEN_MINUS_EQUALS || tokens[i+2].type == GEM_TOKEN_MUL_EQUALS || tokens[i+2].type == GEM_TOKEN_DIV_EQUALS)) {
                    auto thisVal = currentScope->resolve("this");
                    if (thisVal && std::holds_alternative<std::shared_ptr<GemObject>>(thisVal->value)) {
                        auto obj = std::get<std::shared_ptr<GemObject>>(thisVal->value);
                        if (obj->parent) {
                            targetName = tokens[i+1].text;
                            targetObj = obj->parent;
                            currentOp = tokens[i+2].type;
                            nextI = i + 3;
                        }
                    }
                } else if (tokens[i].type == GEM_TOKEN_ID && i+1 < tokens.size() && tokens[i+1].type == GEM_TOKEN_DOT && i+2 < tokens.size() && tokens[i+2].type == GEM_TOKEN_ID && (tokens[i+3].type == GEM_TOKEN_EQUALS || tokens[i+3].type == GEM_TOKEN_PLUS_EQUALS || tokens[i+3].type == GEM_TOKEN_MINUS_EQUALS || tokens[i+3].type == GEM_TOKEN_MUL_EQUALS || tokens[i+3].type == GEM_TOKEN_DIV_EQUALS)) {
                    auto objVal = currentScope->resolve(tokens[i].text);
                    if (objVal && std::holds_alternative<std::shared_ptr<GemObject>>(objVal->value)) {
                        targetName = tokens[i+2].text;
                        targetObj = std::get<std::shared_ptr<GemObject>>(objVal->value);
                        currentOp = tokens[i+3].type;
                        nextI = i + 4;
                    }
                }

                if (!targetName.empty()) {
                    targets.push_back({targetName, targetObj});
                    i = nextI;
                    if (currentOp != GEM_TOKEN_EQUALS) { compoundOp = currentOp; break; }
                } else break;
            }

            if (!targets.empty() && i < tokens.size()) {
                size_t p = i;
                // If RHS is a single bare identifier that resolves to nothing, treat as string literal
                size_t rhsEnd = p;
                while (rhsEnd < tokens.size() && tokens[rhsEnd].type != GEM_TOKEN_EOF) rhsEnd++;
                bool rhsIsBareId = (rhsEnd == p + 1 && tokens[p].type == GEM_TOKEN_ID
                                    && !currentScope->resolve(tokens[p].text)
                                    && !builtins.count(tokens[p].text)
                                    && !userFunctions.count(tokens[p].text)
                                    && !userClasses.count(tokens[p].text));
                auto val = rhsIsBareId
                    ? std::make_shared<GemValue>(tokens[p].text)
                    : parseExpression(tokens, p);
                if (val) {
                    if (compoundOp != GEM_TOKEN_UNKNOWN && targets.size() == 1) {
                        auto target = targets[0];
                        auto currentVal = target.second ? target.second->get(target.first) : currentScope->resolve(target.first);
                        if (!currentVal) currentVal = std::make_shared<GemValue>(0.0);
                        
                        if (std::holds_alternative<double>(currentVal->value) && std::holds_alternative<double>(val->value)) {
                            double l = std::get<double>(currentVal->value);
                            double r = std::get<double>(val->value);
                            if (compoundOp == GEM_TOKEN_PLUS_EQUALS) val = std::make_shared<GemValue>(l + r);
                            else if (compoundOp == GEM_TOKEN_MINUS_EQUALS) val = std::make_shared<GemValue>(l - r);
                            else if (compoundOp == GEM_TOKEN_MUL_EQUALS) val = std::make_shared<GemValue>(l * r);
                            else if (compoundOp == GEM_TOKEN_DIV_EQUALS) val = std::make_shared<GemValue>(l / r);
                        } else if (compoundOp == GEM_TOKEN_PLUS_EQUALS) {
                            val = std::make_shared<GemValue>(currentVal->toString() + val->toString());
                        }
                    }
                    for (const auto& target : targets) {
                        if (target.second) target.second->set(target.first, val);
                        else { 
                            if (target.first[0] == '_') globalScope->define(target.first, val); 
                            else {
                                // Try to update existing variable in the scope chain
                                auto existing = currentScope->resolve(target.first);
                                if (existing) {
                                    existing->value = val->value;
                                } else {
                                    // Warn: implicit declaration (no type keyword used)
                                    std::cerr << "Warning: implicit declaration of '" << target.first << "'" << std::endl;
                                    auto thisVal = currentScope->resolve("this");
                                    if (thisVal && std::holds_alternative<std::shared_ptr<GemObject>>(thisVal->value)) {
                                        std::get<std::shared_ptr<GemObject>>(thisVal->value)->set(target.first, val);
                                    }
                                    currentScope->define(target.first, val);
                                }
                            }
                        }
                    }
                    return;
                }
            }
        }
        
        if (tokens[0].type == GEM_TOKEN_ID && tokens[0].text == "help") {
            auto sysVal = globalScope->resolve("sys");
            if (sysVal && std::holds_alternative<std::shared_ptr<GemObject>>(sysVal->value)) {
                std::vector<std::shared_ptr<GemValue>> args;
                size_t startIdx = 1;
                if (tokens.size() > 1 && tokens[1].type == GEM_TOKEN_LPAREN) startIdx = 2;
                
                if (tokens.size() > startIdx) {
                    if (tokens[startIdx].type == GEM_TOKEN_STRING) args.push_back(std::make_shared<GemValue>(tokens[startIdx].text));
                    else if (tokens[startIdx].type == GEM_TOKEN_ID) args.push_back(std::make_shared<GemValue>(tokens[startIdx].text));
                }
                std::get<std::shared_ptr<GemObject>>(sysVal->value)->call("help", args);
            }
            return;
        }

        if (tokens[0].type == GEM_TOKEN_ID && tokens[0].text == "exit") running = false;
        else if (tokens[0].type == GEM_TOKEN_KEYWORD_END) return;
        else if (tokens[0].type == GEM_TOKEN_DOT || tokens[0].type == GEM_TOKEN_DOT_DOT) {
            size_t p = 0;
            auto val = parseExpression(tokens, p);
            if (val && !std::holds_alternative<std::monostate>(val->value)) *out << "=> " << val->toString() << std::endl;
        }
        else {
            auto val = parseExpression(tokens, pos);
            if (val && !std::holds_alternative<std::monostate>(val->value)) *out << "=> " << val->toString() << std::endl;
        }
    }

    std::shared_ptr<GemValue> parseExpression(const std::vector<GemToken>& tokens, size_t& pos) {
        std::lock_guard<std::recursive_mutex> lock(interpreterMutex);
        auto left = parseCompare(tokens, pos);
        return left;
    }

    std::shared_ptr<GemValue> parseCompare(const std::vector<GemToken>& tokens, size_t& pos) {
        auto left = parseAddSub(tokens, pos);
        if (!left) return std::make_shared<GemValue>();

        while (pos < tokens.size()) {
            if (tokens[pos].type == GEM_TOKEN_GREATER || tokens[pos].type == GEM_TOKEN_LESS || tokens[pos].type == GEM_TOKEN_COMPARE_EQUALS) {
                GemTokenType op = tokens[pos].type;
                pos++;
                auto right = parseAddSub(tokens, pos);
                if (left && right) {
                    if (std::holds_alternative<double>(left->value) && std::holds_alternative<double>(right->value)) {
                        double l = std::get<double>(left->value);
                        double r = std::get<double>(right->value);
                        if (op == GEM_TOKEN_GREATER) left = std::make_shared<GemValue>((bool)(l > r));
                        else if (op == GEM_TOKEN_LESS) left = std::make_shared<GemValue>((bool)(l < r));
                        else if (op == GEM_TOKEN_COMPARE_EQUALS) left = std::make_shared<GemValue>((bool)(l == r));
                    } else if (op == GEM_TOKEN_COMPARE_EQUALS) {
                        // std::cout << "DEBUG Comparison: [" << left->toString() << "] == [" << right->toString() << "]" << std::endl;
                        left = std::make_shared<GemValue>((bool)(left->toString() == right->toString()));
                    }
                }
            } else break;
        }
        return left;
    }

    std::shared_ptr<GemValue> parseAddSub(const std::vector<GemToken>& tokens, size_t& pos) {
        auto left = parseMul(tokens, pos);
        if (!left) return std::make_shared<GemValue>();

        while (pos < tokens.size()) {
            if (tokens[pos].type == GEM_TOKEN_PLUS || tokens[pos].type == GEM_TOKEN_MINUS) {
                GemTokenType op = tokens[pos].type;
                pos++;
                auto right = parseMul(tokens, pos);
                if (left && right) {
                    if (std::holds_alternative<double>(left->value) && std::holds_alternative<double>(right->value)) {
                        double l = std::get<double>(left->value);
                        double r = std::get<double>(right->value);
                        if (op == GEM_TOKEN_PLUS) left = std::make_shared<GemValue>(l + r);
                        else if (op == GEM_TOKEN_MINUS) left = std::make_shared<GemValue>(l - r);
                    } else if (op == GEM_TOKEN_PLUS) {
                        // String concatenation
                        left = std::make_shared<GemValue>(left->toString() + right->toString());
                    }
                }
            } else break;
        }
        return left ? left : std::make_shared<GemValue>();
    }

    std::shared_ptr<GemValue> parseMul(const std::vector<GemToken>& tokens, size_t& pos) {
        auto left = parsePrimary(tokens, pos);
        if (!left) return std::make_shared<GemValue>();

        while (pos < tokens.size()) {
            if (tokens[pos].type == GEM_TOKEN_MUL || tokens[pos].type == GEM_TOKEN_DIV) {
                GemTokenType op = tokens[pos].type;
                pos++;
                auto right = parsePrimary(tokens, pos);
                if (left && right && std::holds_alternative<double>(left->value) && std::holds_alternative<double>(right->value)) {
                    double l = std::get<double>(left->value);
                    double r = std::get<double>(right->value);
                    if (op == GEM_TOKEN_MUL) left = std::make_shared<GemValue>(l * r);
                    else if (op == GEM_TOKEN_DIV) left = std::make_shared<GemValue>(l / r);
                }
            } else break;
        }
        return left ? left : std::make_shared<GemValue>();
    }

    std::shared_ptr<GemValue> parsePrimary(const std::vector<GemToken>& tokens, size_t& pos) {
        auto left = parseTerm(tokens, pos);
        if (!left) return std::make_shared<GemValue>();

        while (pos < tokens.size()) {
            if (tokens[pos].type == GEM_TOKEN_DOT) {
                pos++;
                if (pos >= tokens.size() || tokens[pos].type != GEM_TOKEN_ID) break;
                std::string member = tokens[pos].text;
                pos++;
                
                if (pos < tokens.size() && tokens[pos].type == GEM_TOKEN_LPAREN) {
                    pos++;
                    std::vector<std::shared_ptr<GemValue>> args;
                    if (pos < tokens.size() && tokens[pos].type != GEM_TOKEN_RPAREN) {
                        do {
                            auto arg = parseExpression(tokens, pos);
                            args.push_back(arg ? arg : std::make_shared<GemValue>());
                            if (pos < tokens.size() && tokens[pos].type == GEM_TOKEN_COMMA) pos++;
                        } while (pos < tokens.size() && tokens[pos].type != GEM_TOKEN_RPAREN);
                    }
                    if (pos < tokens.size() && tokens[pos].type == GEM_TOKEN_RPAREN) pos++;

                    if (std::holds_alternative<std::shared_ptr<GemObject>>(left->value)) {
                        left = std::get<std::shared_ptr<GemObject>>(left->value)->call(member, args);
                    } else if (member == "length" || member == "size" || member == "count") {
                        if (std::holds_alternative<std::vector<double>>(left->value)) left = std::make_shared<GemValue>((double)std::get<std::vector<double>>(left->value).size());
                        else if (std::holds_alternative<std::vector<std::string>>(left->value)) left = std::make_shared<GemValue>((double)std::get<std::vector<std::string>>(left->value).size());
                        else if (std::holds_alternative<std::vector<std::shared_ptr<GemObject>>>(left->value)) left = std::make_shared<GemValue>((double)std::get<std::vector<std::shared_ptr<GemObject>>>(left->value).size());
                        else if (std::holds_alternative<std::string>(left->value)) left = std::make_shared<GemValue>((double)std::get<std::string>(left->value).length());
                        else left = std::make_shared<GemValue>();
                    } else return std::make_shared<GemValue>(); 
                } else {
                    if (std::holds_alternative<std::shared_ptr<GemObject>>(left->value)) {
                        left = std::get<std::shared_ptr<GemObject>>(left->value)->get(member);
                    } else if (member == "length" || member == "size" || member == "count") {
                        if (std::holds_alternative<std::vector<double>>(left->value)) left = std::make_shared<GemValue>((double)std::get<std::vector<double>>(left->value).size());
                        else if (std::holds_alternative<std::vector<std::string>>(left->value)) left = std::make_shared<GemValue>((double)std::get<std::vector<std::string>>(left->value).size());
                        else if (std::holds_alternative<std::vector<std::shared_ptr<GemObject>>>(left->value)) left = std::make_shared<GemValue>((double)std::get<std::vector<std::shared_ptr<GemObject>>>(left->value).size());
                        else if (std::holds_alternative<std::string>(left->value)) left = std::make_shared<GemValue>((double)std::get<std::string>(left->value).length());
                        else left = std::make_shared<GemValue>();
                    }
                }
            } else if (tokens[pos].type == GEM_TOKEN_LBRACKET) {
                pos++;
                auto indexVal = parseExpression(tokens, pos);
                if (pos < tokens.size() && tokens[pos].type == GEM_TOKEN_RBRACKET) pos++;

                if (indexVal) {
                    if (std::holds_alternative<double>(indexVal->value)) {
                        int idx = (int)std::get<double>(indexVal->value);
                        if (std::holds_alternative<std::vector<double>>(left->value)) {
                            const auto& v = std::get<std::vector<double>>(left->value);
                            if (idx >= 0 && idx < (int)v.size()) left = std::make_shared<GemValue>(v[idx]);
                            else left = std::make_shared<GemValue>();
                        } else if (std::holds_alternative<std::vector<std::string>>(left->value)) {
                            const auto& v = std::get<std::vector<std::string>>(left->value);
                            if (idx >= 0 && idx < (int)v.size()) left = std::make_shared<GemValue>(v[idx]);
                            else left = std::make_shared<GemValue>();
                        } else if (std::holds_alternative<std::vector<std::shared_ptr<GemObject>>>(left->value)) {
                            const auto& v = std::get<std::vector<std::shared_ptr<GemObject>>>(left->value);
                            if (idx >= 0 && idx < (int)v.size()) left = std::make_shared<GemValue>(v[idx]);
                            else left = std::make_shared<GemValue>();
                        } else if (std::holds_alternative<std::string>(left->value)) {
                            const auto& s = std::get<std::string>(left->value);
                            if (idx >= 0 && idx < (int)s.length()) left = std::make_shared<GemValue>(std::string(1, s[idx]));
                            else left = std::make_shared<GemValue>();
                        }
                    } else if (std::holds_alternative<std::string>(indexVal->value)) {
                        std::string key = std::get<std::string>(indexVal->value);
                        if (std::holds_alternative<std::map<std::string, std::shared_ptr<GemValue>>>(left->value)) {
                            auto& m = std::get<std::map<std::string, std::shared_ptr<GemValue>>>(left->value);
                            if (m.count(key)) left = m[key];
                            else left = std::make_shared<GemValue>();
                        }
                    }
                }
            } else break;
        }
        return left ? left : std::make_shared<GemValue>();
    }

    std::shared_ptr<GemValue> parseTerm(const std::vector<GemToken>& tokens, size_t& pos) {
        if (pos >= tokens.size()) return std::make_shared<GemValue>();

        if (tokens[pos].type == GEM_TOKEN_NUMBER) {
            double v = tokens[pos].numberVal;
            pos++;
            return std::make_shared<GemValue>(v);
        } else if (tokens[pos].type == GEM_TOKEN_STRING) {
            std::string s = tokens[pos].text;
            pos++;
            return std::make_shared<GemValue>(s);
        } else if (tokens[pos].type == GEM_TOKEN_DOT) {
            pos++;
            if (pos < tokens.size() && tokens[pos].type == GEM_TOKEN_ID) {
                std::string member = tokens[pos].text;
                pos++;
                
                auto thisVal = currentScope->resolve("this");
                if (thisVal && std::holds_alternative<std::shared_ptr<GemObject>>(thisVal->value)) {
                    auto obj = std::get<std::shared_ptr<GemObject>>(thisVal->value);
                    
                    if (pos < tokens.size() && tokens[pos].type == GEM_TOKEN_LPAREN) {
                        pos++;
                        std::vector<std::shared_ptr<GemValue>> args;
                        if (pos < tokens.size() && tokens[pos].type != GEM_TOKEN_RPAREN) {
                            do {
                                auto arg = parseExpression(tokens, pos);
                                args.push_back(arg ? arg : std::make_shared<GemValue>());
                                if (pos < tokens.size() && tokens[pos].type == GEM_TOKEN_COMMA) pos++;
                            } while (pos < tokens.size() && tokens[pos].type != GEM_TOKEN_RPAREN);
                        }
                        if (pos < tokens.size() && tokens[pos].type == GEM_TOKEN_RPAREN) pos++;
                        return obj->call(member, args);
                    }
                    return obj->get(member);
                }
            }
            return std::make_shared<GemValue>();
        } else if (tokens[pos].type == GEM_TOKEN_DOT_DOT) {
            pos++;
            if (pos < tokens.size() && tokens[pos].type == GEM_TOKEN_ID) {
                std::string member = tokens[pos].text;
                pos++;
                
                auto thisVal = currentScope->resolve("this");
                if (thisVal && std::holds_alternative<std::shared_ptr<GemObject>>(thisVal->value)) {
                    auto obj = std::get<std::shared_ptr<GemObject>>(thisVal->value);
                    if (obj->parent) {
                        auto parent = obj->parent;
                        if (pos < tokens.size() && tokens[pos].type == GEM_TOKEN_LPAREN) {
                            pos++;
                            std::vector<std::shared_ptr<GemValue>> args;
                            if (pos < tokens.size() && tokens[pos].type != GEM_TOKEN_RPAREN) {
                                do {
                                    auto arg = parseExpression(tokens, pos);
                                    args.push_back(arg ? arg : std::make_shared<GemValue>());
                                    if (pos < tokens.size() && tokens[pos].type == GEM_TOKEN_COMMA) pos++;
                                } while (pos < tokens.size() && tokens[pos].type != GEM_TOKEN_RPAREN);
                            }
                            if (pos < tokens.size() && tokens[pos].type == GEM_TOKEN_RPAREN) pos++;
                            return parent->call(member, args);
                        }
                        return parent->get(member);
                    }
                }
            }
            return std::make_shared<GemValue>();
        } else if (tokens[pos].type == GEM_TOKEN_ID) {
            std::string name = tokens[pos].text;
            pos++;
            
            if (pos < tokens.size() && tokens[pos].type == GEM_TOKEN_LPAREN) {
                 if (userClasses.count(name) || userFunctions.count(name)) {
                     bool isClass = userClasses.count(name);
                     pos++; // (
                     std::vector<std::shared_ptr<GemValue>> args;
                     if (pos < tokens.size() && tokens[pos].type != GEM_TOKEN_RPAREN) {
                        do {
                            auto arg = parseExpression(tokens, pos);
                            args.push_back(arg ? arg : std::make_shared<GemValue>());
                            if (pos < tokens.size() && tokens[pos].type == GEM_TOKEN_COMMA) pos++;
                        } while (pos < tokens.size() && tokens[pos].type != GEM_TOKEN_RPAREN);
                     }
                     if (pos < tokens.size() && tokens[pos].type == GEM_TOKEN_RPAREN) pos++;
                     if (isClass) return callUserClass(name, args);
                     return callUserFunction(name, args);
                 }
            }

            auto res = currentScope->resolve(name);
            return res ? res : std::make_shared<GemValue>();
        } else if (tokens[pos].type == GEM_TOKEN_KEYWORD_OBJ) {
            pos++;
            std::shared_ptr<GemObject> parent = GemObject::sysInstance;
            if (pos < tokens.size() && tokens[pos].type == GEM_TOKEN_LPAREN) {
                pos++;
                if (pos < tokens.size() && tokens[pos].type == GEM_TOKEN_ID) {
                    auto pVal = currentScope->resolve(tokens[pos].text);
                    if (pVal && std::holds_alternative<std::shared_ptr<GemObject>>(pVal->value)) parent = std::get<std::shared_ptr<GemObject>>(pVal->value);
                    pos++;
                }
                if (pos < tokens.size() && tokens[pos].type == GEM_TOKEN_RPAREN) pos++;
            }
            return std::make_shared<GemValue>(std::make_shared<GemObject>("Generic", parent));
        } else if (tokens[pos].type == GEM_TOKEN_LBRACKET) {
            pos++;
            std::vector<std::shared_ptr<GemValue>> elements;
            while (pos < tokens.size() && tokens[pos].type != GEM_TOKEN_RBRACKET) {
                auto val = parseExpression(tokens, pos);
                if (val) elements.push_back(val);
                if (pos < tokens.size() && tokens[pos].type == GEM_TOKEN_COMMA) pos++;
            }
            if (pos < tokens.size() && tokens[pos].type == GEM_TOKEN_RBRACKET) pos++;
            
            // Try to specialize the vector type for performance/interop
            bool allDouble = true;
            bool allString = true;
            bool allObject = true;
            if (elements.empty()) return std::make_shared<GemValue>(std::vector<double>{});
            
            for (auto& e : elements) {
                if (!std::holds_alternative<double>(e->value)) allDouble = false;
                if (!std::holds_alternative<std::string>(e->value)) allString = false;
                if (!std::holds_alternative<std::shared_ptr<GemObject>>(e->value)) allObject = false;
            }
            
            if (allDouble) {
                std::vector<double> vd;
                for (auto& e : elements) vd.push_back(std::get<double>(e->value));
                return std::make_shared<GemValue>(vd);
            } else if (allString) {
                std::vector<std::string> vs;
                for (auto& e : elements) vs.push_back(std::get<std::string>(e->value));
                return std::make_shared<GemValue>(vs);
            } else if (allObject) {
                std::vector<std::shared_ptr<GemObject>> vo;
                for (auto& e : elements) vo.push_back(std::get<std::shared_ptr<GemObject>>(e->value));
                return std::make_shared<GemValue>(vo);
            } else {
                return std::make_shared<GemValue>(elements);
            }
        }
        return std::make_shared<GemValue>();
    }

    std::shared_ptr<GemValue> callUserClass(const std::string& name, const std::vector<std::shared_ptr<GemValue>>& args) {
        if (!userClasses.count(name)) return std::make_shared<GemValue>();
        GemObjectDef& def = userClasses[name];
        std::shared_ptr<GemObject> parent = GemObject::sysInstance;
        std::map<std::string, GemObject::GemMethod> combinedMethods = def.methods;

        if (!def.parentName.empty()) {
            auto pVal = globalScope->resolve(def.parentName);
            if (pVal && std::holds_alternative<std::shared_ptr<GemObject>>(pVal->value)) parent = std::get<std::shared_ptr<GemObject>>(pVal->value);
            else if (userClasses.count(def.parentName)) {
                auto& pDef = userClasses[def.parentName];
                // Inherit methods from parent definition
                for (auto const& [mName, mDef] : pDef.methods) {
                    if (!combinedMethods.count(mName)) combinedMethods[mName] = mDef;
                }
                auto pInstVal = callUserClass(def.parentName, {});
                if (std::holds_alternative<std::shared_ptr<GemObject>>(pInstVal->value)) parent = std::get<std::shared_ptr<GemObject>>(pInstVal->value);
            }
        }
        auto obj = std::make_shared<GemObject>(name, parent);
        obj->methods = combinedMethods;
        auto objVal = std::make_shared<GemValue>(obj);
        auto classScope = std::make_shared<GemScope>(globalScope);
        classScope->define("this", objVal);
        for(size_t i=0; i<def.params.size() && i<args.size(); ++i) classScope->define(def.params[i], args[i]);
        auto oldScope = currentScope;
        currentScope = classScope;
        skippingStack.push(false);
        skipDepthStack.push(0);
        for(const auto& line : def.body) execute(line, true);
        skippingStack.pop();
        skipDepthStack.pop();
        currentScope = oldScope;
        return objVal;
    }

    std::shared_ptr<GemValue> callUserFunction(const std::string& name, const std::vector<std::shared_ptr<GemValue>>& args, std::shared_ptr<GemObject> thisObj = nullptr) {
        std::lock_guard<std::recursive_mutex> lock(interpreterMutex);
        if (!userFunctions.count(name)) return std::make_shared<GemValue>();
        GemFunction& func = userFunctions[name];
        auto funcScope = std::make_shared<GemScope>(globalScope);
        if (thisObj) funcScope->define("this", std::make_shared<GemValue>(thisObj));
        for(size_t i=0; i<func.params.size() && i<args.size(); ++i) {
            funcScope->define(func.params[i], args[i]);
        }
        auto oldScope = currentScope;
        currentScope = funcScope;
        skippingStack.push(false);
        skipDepthStack.push(0);
        
        for(const auto& line : func.body) {
            execute(line, true);
        }
        
        skippingStack.pop();
        skipDepthStack.pop();
        
        // IMPORTANT: Resolve return expression in the funcScope BEFORE restoring oldScope!
        std::shared_ptr<GemValue> retVal = std::make_shared<GemValue>();
        if (!func.returnExpr.empty()) {
            size_t pos = 0;
            retVal = parseExpression(func.returnExpr, pos);
        }
        currentScope = oldScope;
        return retVal;
    }
};

inline std::shared_ptr<GemValue> GemObject::call(const std::string& methodName, std::vector<std::shared_ptr<GemValue>> args) {
    if (methods.count(methodName)) {
        auto& m = methods[methodName];
        if (m.isNative) return std::get<std::function<std::shared_ptr<GemValue>(std::vector<std::shared_ptr<GemValue>>)>>(m.func)(args);
        else {
            std::string funcName = std::get<std::string>(m.func);
            if (GemInterpreter::instance) return GemInterpreter::instance->callUserFunction(funcName, args, shared_from_this());
        }
    }
    if (parent) return parent->call(methodName, args);
    return std::make_shared<GemValue>();
}

GemInterpreter* GemInterpreter::instance = nullptr;

#endif
