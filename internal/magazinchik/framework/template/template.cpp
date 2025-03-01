#include "template.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>
#include <vector>

namespace template_engine {
    std::string load_template(const std::string &filename) {
        std::ifstream file(filename);
        if (!file) {
            std::cerr << "Ошибка: Не удалось открыть " << filename << std::endl;
            return "";
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    std::string render_template(const std::string &template_str, const std::unordered_map<std::string, std::string> &context) {
        std::string result = template_str;
        for (const auto &[key, value] : context) {
            std::string placeholder = "{{" + key + "}}";
            size_t pos = 0;
            while ((pos = result.find(placeholder, pos)) != std::string::npos) {
                result.replace(pos, placeholder.length(), value);
                pos += value.length();
            }
        }
        return result;
    }

    std::string render_template(const std::string &template_str, const std::unordered_map<std::string, std::string> &context,
                                const std::unordered_map<std::string, std::vector<std::unordered_map<std::string, std::string>>> &list_context) {
        std::string result = template_str;

        std::regex loop_regex(R"(\{%\s*for\s+(\w+)\s+in\s+(\w+)\s*%\}([\s\S]*?)\{%\s*endfor\s*%\})");
        std::smatch match;
        while (std::regex_search(result, match, loop_regex)) {
            std::string loop_block = match[0];
            std::string item_name = match[1];
            std::string list_name = match[2];
            std::string loop_body = match[3];

            std::string rendered_loop;
            if (list_context.count(list_name)) {
                for (const auto &item : list_context.at(list_name)) {
                    std::string temp_body = loop_body;
                    for (const auto &[key, value] : item) {
                        std::string placeholder = "{{" + item_name + "." + key + "}}";
                        size_t pos = 0;
                        while ((pos = temp_body.find(placeholder, pos)) != std::string::npos) {
                            temp_body.replace(pos, placeholder.length(), value);
                            pos += value.length();
                        }
                    }
                    rendered_loop += temp_body;
                }
            }
            result.replace(match.position(0), match.length(0), rendered_loop);
        }

        return render_template(result, context);
    }
}