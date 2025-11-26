#include "struct_registry.h"
#include <unordered_map>
#include <vector>

namespace {
    static std::unordered_map<std::string, std::vector<std::string>> g_struct_fields;
}

namespace StructRegistry {

    void register_struct(const std::string& name, const std::vector<std::string>& fields) {
        g_struct_fields[name] = fields;
    }

    const std::vector<std::string>& get_field_names(const std::string& name) {
        static const std::vector<std::string> empty;
        auto it = g_struct_fields.find(name);
        if (it == g_struct_fields.end()) return empty;
        return it->second;
    }

    bool has_struct(const std::string& name) {
        return g_struct_fields.find(name) != g_struct_fields.end();
    }

}
