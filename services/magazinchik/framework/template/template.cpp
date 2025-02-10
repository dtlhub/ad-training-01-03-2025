#include "template.h"
#include <fstream>
#include <sstream>
#include <iostream>

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
}