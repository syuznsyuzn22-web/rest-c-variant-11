#pragma once
#include <string>
#include <stdexcept>
#include <cmath>
#include <cctype>

namespace TaskVariant {

struct Input {
    double length;
    double width;
    double height;
    double physical_weight;
    double distance_km;
    int fragility_index;
    double base_tariff_per_km;
};

struct Output {
    double volumetric_weight;
    double billable_weight;
    double final_delivery_cost;
};

static double extract_double(const std::string& json, const std::string& key) {
    std::string search = "\"" + key + "\":";
    size_t pos = json.find(search);
    if (pos == std::string::npos) throw std::runtime_error("Missing key: " + key);
    pos += search.length();
    while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t')) pos++;
    size_t end = pos;
    while (end < json.length() && (std::isdigit(static_cast<unsigned char>(json[end])) || json[end] == '.' || json[end] == '-' || json[end] == 'e' || json[end] == 'E' || json[end] == '+')) end++;
    return std::stod(json.substr(pos, end - pos));
}

static int extract_int(const std::string& json, const std::string& key) {
    std::string search = "\"" + key + "\":";
    size_t pos = json.find(search);
    if (pos == std::string::npos) throw std::runtime_error("Missing key: " + key);
    pos += search.length();
    while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\t')) pos++;
    size_t end = pos;
    while (end < json.length() && (std::isdigit(static_cast<unsigned char>(json[end])) || json[end] == '-')) end++;
    return std::stoi(json.substr(pos, end - pos));
}

inline Input parse(const std::string& json) {
    Input input;
    input.length = extract_double(json, "length");
    input.width = extract_double(json, "width");
    input.height = extract_double(json, "height");
    input.physical_weight = extract_double(json, "physical_weight");
    input.distance_km = extract_double(json, "distance_km");
    input.fragility_index = extract_int(json, "fragility_index");
    input.base_tariff_per_km = extract_double(json, "base_tariff_per_km");
    return input;
}

// ИСПРАВЛЕНО: теперь возвращает std::string (пустую строку при успехе)
inline std::string validate(const Input& input) {
    if (input.length < 1.0 || input.length > 300.0) return "length must be in [1.0, 300.0]";
    if (input.width < 1.0 || input.width > 300.0) return "width must be in [1.0, 300.0]";
    if (input.height < 1.0 || input.height > 300.0) return "height must be in [1.0, 300.0]";
    if (input.physical_weight < 0.01 || input.physical_weight > 2000.0) return "physical_weight must be in [0.01, 2000.0]";
    if (input.distance_km < 1.0 || input.distance_km > 12000.0) return "distance_km must be in [1.0, 12000.0]";
    if (input.fragility_index < 1 || input.fragility_index > 3) return "fragility_index must be in [1, 3]";
    if (input.base_tariff_per_km <= 0.0) return "base_tariff_per_km must be > 0.0";
    return ""; // Пустая строка означает, что ошибок нет
}

inline Output calculate(const Input& input) {
    Output output;
    output.volumetric_weight = (input.length * input.width * input.height) / 5000.0;
    output.billable_weight = (input.physical_weight > output.volumetric_weight) ? input.physical_weight : output.volumetric_weight;
    output.final_delivery_cost = input.distance_km * input.base_tariff_per_km * (1.0 + (input.fragility_index - 1) * 0.5) * (output.billable_weight * 0.1);
    return output;
}

inline std::string to_json(const Output& output) {
    return "{\"volumetric_weight\":" + std::to_string(output.volumetric_weight) +
           ",\"billable_weight\":" + std::to_string(output.billable_weight) +
           ",\"final_delivery_cost\":" + std::to_string(output.final_delivery_cost) + "}";
}

} // namespace TaskVariant
