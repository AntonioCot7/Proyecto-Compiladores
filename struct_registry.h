#ifndef STRUCT_REGISTRY_H
#define STRUCT_REGISTRY_H

#include <string>
#include <vector>
#include <unordered_map>

// Registro global, simple, de nombres de campos por struct
namespace StructRegistry {
    void register_struct(const std::string& name, const std::vector<std::string>& fields);
    const std::vector<std::string>& get_field_names(const std::string& name);
    bool has_struct(const std::string& name);
}

#endif // STRUCT_REGISTRY_H
