#include "SqlQueryStorage.h"

#include <algorithm>
#include <assert.h>
#include <filesystem>
#include <iomanip>
#include <sstream>

#include <wx/log.h>

namespace marine_navi::clients::query_builder {

namespace {

std::string BaseArgVarToString(const BaseArgVar& var) {
    return std::visit([](const auto& v) -> std::string {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, int64_t>) {    
            std::stringstream ss;
            ss << v;
            return ss.str();
        } else if constexpr (std::is_same_v<T, double>) {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(3) << v;
            return ss.str();
        } else if constexpr (std::is_same_v<T, std::string>) {
            return "'" + v + "'";
        }
    }, var);
}

std::string OptionalArgVarToString(const std::optional<BaseArgVar>& opt) {
    if (!opt.has_value()) {
        return "NULL";
    }
    return BaseArgVarToString(opt.value());
}

std::string VectorBaseArgVarToString(const std::vector<BaseArgVar>& vec) {
    std::stringstream ss;
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) ss << ",";
        ss << BaseArgVarToString(vec[i]);
    }
    return ss.str();
}

std::string VectorVectorBaseArgVarToString(const std::vector<std::vector<BaseArgVar>>& vecvec) {
    std::stringstream ss;
    for (size_t i = 0; i < vecvec.size(); ++i) {
        if (i > 0) ss << ",";
        ss << "(";
        for (size_t j = 0; j < vecvec[i].size(); ++j) {
            if (j > 0) ss << ",";
            ss << BaseArgVarToString(vecvec[i][j]);
        }
        ss << ")";
    }
    return ss.str();
}

std::string ComposedArgToString(const ComposedArgVar& arg) {
    return std::visit([](const auto& v) {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, BaseArgVar>) {
            return BaseArgVarToString(v);
        } else if constexpr (std::is_same_v<T, std::optional<BaseArgVar>>) {
            return OptionalArgVarToString(v);
        } else if constexpr (std::is_same_v<T, std::vector<BaseArgVar>>) {
            return VectorBaseArgVarToString(v);
        } else if constexpr (std::is_same_v<T, std::vector<std::vector<BaseArgVar>>>) {
            return VectorVectorBaseArgVarToString(v);
        }
    }, arg);
}

} // namespace

const std::string& SqlTemplate::GetName() const { return name_ };

std::string SqlTemplate::MakeQuery(const std::vector<ArgVar>& args) const {
    std::stringstream ss;
    for (const auto& token : tokens) {
        if (std::holds_alternative<int>(token)) {
            int id = std::get<int>(token);
            assert(id < static_cast<int>(args.size()) && id >= 0);
            ss << ComposedArgToString(args[id]);
        } else {
            ss << std::get<std::string>(token);
        }
    }
    const std::string query = ss.str();
    wxLogInfo(wxT("Make new query with name '%s' and value '%s'"), name_, query_);
    return query;
}

std::string SqlTemplate::ParseName(const std::string& query) {
    std::string first_line = query.substr(0, query.find('\n'));
    assert(first_line.substr(0, 3) == "-- ");
    name_ = first_line.substr(3, first_line.size() - 4);
}

std::string SqlTemplate::ParseTokens(const std::string& query) {
    std::string buffer;
    std::vector<TokenType> tokens;
    for (std::size_t i = 0; i < query.size(); ++i) {
        if (query[i] == '$') {
            std::size_t start = i + 1;
            std::size_t end = start;
            while (end < query.size() && std::isdigit(static_cast<unsigned char>(query[end]))) {
                end++;
            }
            if (end > start) {
                if (!buffer.empty()) {
                    tokens.push_back(buffer);
                    buffer.clear();
                }
                std::string numStr = query.substr(start, end - start);
                int index = std::stoi(numStr);
                tokens.push_back(index - 1);
                i = end - 1;
            } else {
                buffer.push_back('$');
            }
        } else {
            buffer.push_back(query[i]);
        }
    }
    if (!buffer.empty()) {
        tokens.push_back(buffer);
    }
    return tokens;
}

const Template& SqlQueryStorage::GetTemplate(const std::string& name) const {
    return storage_.at(name);
}

std::unordered_map<std::string, Template> SqlQueryStorage::LoadTemplates(const std::string& path_to_dir) {
    std::unordered_map<std::string, Template> templates;
    for (const auto& entry : std::filesystem::directory_iterator(path_to_dir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".sql") {
            std::ifstream file(entry.path());
            if (!file.is_open()) {
                continue;
            }
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string sql = buffer.str();
            Template t(sql);
            templates.emplace(t.GetName(), std::move(t));
        }
    }

    return templates;
}

} // namespace marine_navi::clients::query_builder
