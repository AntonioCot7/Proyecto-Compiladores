#include "struct_registry.h"
#include <unordered_map>
#include <vector>

namespace {
    static unordered_map<string, vector<string>> g_struct_fields;
}

namespace StructRegistry {

    void register_struct(const string& name, const vector<string>& fields) {
        g_struct_fields[name] = fields;
    }

    const vector<string>& get_field_names(const string& name) {
        static const vector<string> empty;
        auto it = g_struct_fields.find(name);
        if (it == g_struct_fields.end()) return empty;
        return it->second;
    }

    bool has_struct(const string& name) {
        return g_struct_fields.find(name) != g_struct_fields.end();
    }

}
