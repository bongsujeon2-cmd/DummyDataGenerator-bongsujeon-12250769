#pragma once
#include "json.hpp"
#include <random>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

class SchemaGenerator {
public:
    SchemaGenerator() : rng_(std::random_device{}()) {}

    // Generate N records in DataMonitor JsonRepository format
    JsonValue generateRepository(const JsonValue& schema, int count) {
        JsonValue root;
        JsonValue entities;
        for (int i = 1; i <= count; ++i) {
            JsonValue entity = generateValue(schema);
            entity["id"] = JsonValue(static_cast<int64_t>(i));
            entities.push_back(entity);
        }
        root["nextId"] = JsonValue(static_cast<int64_t>(count + 1));
        root["entities"] = entities;
        return root;
    }

    // Generate N records as a plain JSON array
    JsonValue generateRawArray(const JsonValue& schema, int count) {
        JsonValue arr;
        for (int i = 0; i < count; ++i)
            arr.push_back(generateValue(schema));
        return arr;
    }

    JsonValue generateValue(const JsonValue& schema) {
        if (!schema.isObject()) return JsonValue{};

        if (schema.contains("enum") && schema.at("enum").isArray()
            && !schema.at("enum").getArray().empty())
            return pickFrom(schema.at("enum").getArray());

        std::string type = "string";
        if (schema.contains("type"))
            type = schema.at("type").getString();
        else if (schema.contains("properties"))
            type = "object";

        if (type == "object")  return generateObject(schema);
        if (type == "array")   return generateArrayType(schema);
        if (type == "string")  return generateString(schema);
        if (type == "integer") return generateInteger(schema);
        if (type == "number")  return generateNumber(schema);
        if (type == "boolean") return generateBoolean();
        return JsonValue{};
    }

private:
    std::mt19937 rng_;

    static const std::vector<std::string>& firstNames() {
        static std::vector<std::string> v = {
            "Alice","Bob","Charlie","Diana","Eve","Frank","Grace","Henry",
            "Iris","Jack","Kate","Leo","Mia","Noah","Olivia","Peter",
            "Quinn","Rachel","Sam","Tina","Uma","Victor","Wendy","Yara","Zoe"
        };
        return v;
    }
    static const std::vector<std::string>& lastNames() {
        static std::vector<std::string> v = {
            "Smith","Johnson","Williams","Jones","Brown","Davis","Miller",
            "Wilson","Moore","Taylor","Anderson","Thomas","Jackson","White",
            "Harris","Martin","Thompson","Garcia","Martinez","Robinson"
        };
        return v;
    }
    static const std::vector<std::string>& domains() {
        static std::vector<std::string> v = {
            "gmail.com","yahoo.com","outlook.com","example.com",
            "test.org","company.com","mail.net","webmail.io"
        };
        return v;
    }
    static const std::vector<std::string>& words() {
        static std::vector<std::string> v = {
            "alpha","beta","gamma","delta","epsilon","zeta","theta",
            "red","blue","green","bright","dark","fast","slow",
            "big","small","new","old","good","bold","swift","prime"
        };
        return v;
    }
    static const std::vector<std::string>& productNames() {
        static std::vector<std::string> v = {
            "Widget Pro","Gadget Plus","Super Tool","Mega Device",
            "Ultra Gear","Prime Item","Elite Pack","Smart Module",
            "Power Unit","Core Kit","Basic Set","Standard Model",
            "Advanced Kit","Pro Series","Turbo Edition","Mini Pack"
        };
        return v;
    }
    static const std::vector<std::string>& cities() {
        static std::vector<std::string> v = {
            "Seoul","Tokyo","New York","London","Paris","Berlin","Sydney",
            "Toronto","Singapore","Dubai","Amsterdam","Barcelona","Rome","Oslo"
        };
        return v;
    }

    template<typename T>
    T randInt(T lo, T hi) {
        if (lo > hi) std::swap(lo, hi);
        std::uniform_int_distribution<T> d(lo, hi);
        return d(rng_);
    }
    double randDouble(double lo, double hi) {
        if (lo > hi) std::swap(lo, hi);
        std::uniform_real_distribution<double> d(lo, hi);
        return d(rng_);
    }
    template<typename C>
    const typename C::value_type& pick(const C& c) {
        return c[randInt<size_t>(0, c.size() - 1)];
    }
    const JsonValue& pickFrom(const JsonValue::Array& arr) {
        return arr[randInt<size_t>(0, arr.size() - 1)];
    }

    JsonValue generateObject(const JsonValue& schema) {
        JsonValue obj;
        if (!schema.contains("properties")) return obj;
        for (const auto& [key, propSchema] : schema.at("properties").getObject())
            obj[key] = generateValue(propSchema);
        return obj;
    }

    JsonValue generateArrayType(const JsonValue& schema) {
        int minItems = schema.contains("minItems") ? (int)schema.at("minItems").getInt() : 1;
        int maxItems = schema.contains("maxItems") ? (int)schema.at("maxItems").getInt() : 3;
        if (minItems > maxItems) std::swap(minItems, maxItems);
        int cnt = randInt(minItems, maxItems);
        JsonValue arr;
        if (schema.contains("items"))
            for (int i = 0; i < cnt; ++i)
                arr.push_back(generateValue(schema.at("items")));
        return arr;
    }

    JsonValue generateString(const JsonValue& schema) {
        if (schema.contains("enum") && schema.at("enum").isArray()
            && !schema.at("enum").getArray().empty())
            return pickFrom(schema.at("enum").getArray());

        if (schema.contains("format")) {
            const std::string& fmt = schema.at("format").getString();
            if (fmt == "email")      return JsonValue(makeEmail());
            if (fmt == "name")       return JsonValue(makeName());
            if (fmt == "first-name") return JsonValue(pick(firstNames()));
            if (fmt == "last-name")  return JsonValue(pick(lastNames()));
            if (fmt == "sentence")   return JsonValue(makeSentence());
            if (fmt == "product")    return JsonValue(pick(productNames()));
            if (fmt == "city")       return JsonValue(pick(cities()));
            if (fmt == "word")       return JsonValue(pick(words()));
        }

        int minLen = schema.contains("minLength") ? (int)schema.at("minLength").getInt() : 4;
        int maxLen = schema.contains("maxLength") ? (int)schema.at("maxLength").getInt() : 12;
        return JsonValue(makeWord(minLen, maxLen));
    }

    JsonValue generateInteger(const JsonValue& schema) {
        int64_t lo = schema.contains("minimum") ? schema.at("minimum").getInt() : 0;
        int64_t hi = schema.contains("maximum") ? schema.at("maximum").getInt() : 100;
        return JsonValue(randInt(lo, hi));
    }

    JsonValue generateNumber(const JsonValue& schema) {
        double lo = schema.contains("minimum") ? schema.at("minimum").getNumber() : 0.0;
        double hi = schema.contains("maximum") ? schema.at("maximum").getNumber() : 1000.0;
        double v = randDouble(lo, hi);
        return JsonValue(std::round(v * 100.0) / 100.0);
    }

    JsonValue generateBoolean() {
        return JsonValue(randInt(0, 1) == 1);
    }

    std::string makeEmail() {
        std::string name = pick(firstNames());
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        name += std::to_string(randInt(1, 999));
        return name + "@" + pick(domains());
    }
    std::string makeName() {
        return pick(firstNames()) + " " + pick(lastNames());
    }
    std::string makeSentence() {
        int n = randInt(3, 7);
        std::string s;
        for (int i = 0; i < n; ++i) {
            if (i > 0) s += " ";
            s += pick(words());
        }
        return s;
    }
    std::string makeWord(int minLen, int maxLen) {
        if (minLen > maxLen) std::swap(minLen, maxLen);
        std::vector<const std::string*> candidates;
        for (const auto& w : words())
            if ((int)w.size() >= minLen && (int)w.size() <= maxLen)
                candidates.push_back(&w);
        if (!candidates.empty())
            return *candidates[randInt<size_t>(0, candidates.size() - 1)];
        int len = randInt(minLen, maxLen);
        std::string r(len, ' ');
        for (char& c : r)
            c = static_cast<char>('a' + randInt(0, 25));
        return r;
    }
};
