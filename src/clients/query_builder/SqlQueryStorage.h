#pragma once

#include <variant>
#include <string>
#include <unordered_map>
#include <optional>

namespace marine_navi::clients::query_builder {

using BaseArgVar = std::variant<
    int64_t,
    double,
    std::string,
>;

using ComposedArgVar = std::variant<
    BaseArgVar
    std::optional<BaseArgVar>,
    std::vector<BaseArgVar>,
    std::vector<std::vector<BaseArgVar> >;

class SqlTemplate {
public:
    SqlTemplate(const std::string& query): name_(ParseName(query)), tokens_(ParseTokens(query)) {}
    const std::string& GetName() const { return name_; }
    std::string MakeQuery(const std::vector<ArgVar>& args) const;

private:
    using TokenType = std::variant<int, std::string>;

    static std::string ParseName(const std::string& query);
    static std::vector<TokenType> ParseTokens(const std::string& query);

private:
    std::string name_;
    std::vector<TokenType> tokens_;
};

class SqlQueryStorage {
public:
    SqlQueryStorage(const std::string& path_to_dir): storage_(LoadTemplates(path_to_dir)) {}
    const Template& GetTemplate(const std::string& name) const;

private:
    static std::unordered_map<std::string, Template> LoadTemplates(const std::string& path_to_dir);

private:
    const std::unordered_map<std::string, Template> storage_;
};

} // namespace marine_navi::clients::query_builder
