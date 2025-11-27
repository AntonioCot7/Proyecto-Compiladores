#ifndef STRUCT_REGISTRY_H
#define STRUCT_REGISTRY_H

#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

// Registro global, simple, de nombres de campos por struct
namespace StructRegistry {
    void register_struct(const string& name, const std::vector<string>& fields);
    const std::vector<string>& get_field_names(const std::string& name);
    bool has_struct(const std::string& name);
}

#endif // STRUCT_REGISTRY_H
