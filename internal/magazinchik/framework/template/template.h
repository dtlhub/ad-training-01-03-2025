#ifndef MAGAZINCHIK_TEMPLATE_H
#define MAGAZINCHIK_TEMPLATE_H

#include <string>
#include <unordered_map>
#include <vector>

namespace template_engine {
    std::string load_template(const std::string &filename);
    std::string render_template(const std::string &template_str, const std::unordered_map<std::string, std::string> &context);
    std::string render_template(const std::string &template_str, const std::unordered_map<std::string, std::string> &context,
                                const std::unordered_map<std::string, std::vector<std::unordered_map<std::string, std::string>>> &list_context);
}

#endif //MAGAZINCHIK_TEMPLATE_H