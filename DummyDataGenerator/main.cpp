#include <iostream>
#include <string>
#include <limits>
#include "json.hpp"
#include "SchemaGenerator.h"

// -- Console helpers ----------------------------------------------------------

static void clearInput() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static void separator(std::string_view title, std::string_view sub = "") {
    std::cout << "\n============================================================\n";
    std::cout << "  " << title;
    if (!sub.empty()) std::cout << "  |  " << sub;
    std::cout << "\n============================================================\n";
}

static int getChoice(int minVal, int maxVal) {
    int choice;
    while (true) {
        std::cout << ">> ";
        if (std::cin >> choice && choice >= minVal && choice <= maxVal) {
            clearInput();
            return choice;
        }
        std::cin.clear();
        clearInput();
        std::cout << "  Invalid. Enter " << minVal << "-" << maxVal << ": ";
    }
}

static void pause() {
    std::cout << "\n  Press Enter to continue...";
    std::cin.get();
}

// -- Core operation -----------------------------------------------------------

static bool doGenerate(const JsonValue& schema, int count,
                       const std::string& outPath, bool repoFmt) {
    SchemaGenerator gen;
    JsonValue result = repoFmt
        ? gen.generateRepository(schema, count)
        : gen.generateRawArray(schema, count);

    if (!result.saveToFile(outPath)) {
        std::cout << "  [ERROR] Cannot write: " << outPath << "\n";
        return false;
    }
    std::cout << "  Generated " << count << " record(s)  ->  " << outPath << "\n";
    return true;
}

// -- CLI mode -----------------------------------------------------------------

static void printUsage(const char* prog) {
    std::cout << "\nUsage: " << prog << " [options]\n"
              << "  -s <file>   Schema JSON file\n"
              << "  -n <count>  Number of records to generate (default: 10)\n"
              << "  -o <file>   Output file path (default: output.json)\n"
              << "  -r          Raw array format instead of repository format\n"
              << "\n  No arguments: starts interactive menu\n\n"
              << "Output formats:\n"
              << "  Repository (default): { \"nextId\": N, \"entities\": [...] }\n"
              << "  Raw array  (-r flag): [ {...}, {...}, ... ]\n\n"
              << "Supported schema keywords:\n"
              << "  type: object, array, string, integer, number, boolean\n"
              << "  string formats: email, name, first-name, last-name,\n"
              << "                  sentence, product, city, word\n"
              << "  string: minLength, maxLength, enum\n"
              << "  integer/number: minimum, maximum\n"
              << "  array: items, minItems, maxItems\n"
              << "  object: properties\n\n";
}

static std::string findArg(int argc, char* argv[], const std::string& flag) {
    for (int i = 1; i + 1 < argc; ++i)
        if (flag == argv[i]) return argv[i + 1];
    return {};
}

static bool hasFlag(int argc, char* argv[], const std::string& flag) {
    for (int i = 1; i < argc; ++i)
        if (flag == argv[i]) return true;
    return false;
}

// -- Interactive mode ---------------------------------------------------------

static void interactiveMode() {
    std::string schemaPath;
    JsonValue   schema;
    int         count      = 10;
    std::string outputPath = "output.json";
    bool        repoFmt    = true;

    while (true) {
        std::string statusSchema = schemaPath.empty() ? "(not loaded)" : schemaPath;
        std::string statusFmt    = repoFmt ? "Repository / DataMonitor" : "Raw Array";

        separator("DummyDataGenerator  v1.0");
        std::cout << "  Schema : " << statusSchema << "\n"
                  << "  Count  : " << count        << "\n"
                  << "  Output : " << outputPath   << "\n"
                  << "  Format : " << statusFmt    << "\n\n"
                  << "  [1] Load schema from file\n"
                  << "  [2] Set record count\n"
                  << "  [3] Set output file\n"
                  << "  [4] Toggle output format\n"
                  << "  [5] Preview loaded schema\n"
                  << "  [6] Generate!\n"
                  << "  [0] Exit\n";

        int ch = getChoice(0, 6);
        if (ch == 0) { std::cout << "\n  Goodbye.\n\n"; break; }

        if (ch == 1) {
            std::cout << "  Schema file path: ";
            std::getline(std::cin, schemaPath);
            schema = JsonValue::parseFile(schemaPath);
            if (schema.isNull()) {
                std::cout << "  [ERROR] Failed to parse: " << schemaPath << "\n";
                schemaPath.clear();
            } else {
                std::cout << "  Schema loaded OK.\n";
            }
            pause();
        }
        else if (ch == 2) {
            std::cout << "  Record count: ";
            while (!(std::cin >> count) || count < 1) {
                std::cin.clear(); clearInput();
                std::cout << "  Enter a positive integer: ";
            }
            clearInput();
        }
        else if (ch == 3) {
            std::cout << "  Output file path: ";
            std::getline(std::cin, outputPath);
            if (outputPath.empty()) outputPath = "output.json";
        }
        else if (ch == 4) {
            repoFmt = !repoFmt;
            std::cout << "  Format set to: "
                      << (repoFmt ? "Repository / DataMonitor" : "Raw Array") << "\n";
        }
        else if (ch == 5) {
            if (schema.isNull())
                std::cout << "  (no schema loaded)\n";
            else
                std::cout << "\n" << schema.stringify(2) << "\n";
            pause();
        }
        else if (ch == 6) {
            if (schema.isNull()) {
                std::cout << "  [ERROR] Load a schema first (option 1).\n";
                pause();
                continue;
            }
            doGenerate(schema, count, outputPath, repoFmt);
            pause();
        }
    }
}

// -- Entry point --------------------------------------------------------------

int main(int argc, char* argv[]) {
    if (argc == 1) { interactiveMode(); return 0; }
    if (hasFlag(argc, argv, "-h") || hasFlag(argc, argv, "--help")) {
        printUsage(argv[0]); return 0;
    }

    std::string schemaPath = findArg(argc, argv, "-s");
    std::string countStr   = findArg(argc, argv, "-n");
    std::string outputPath = findArg(argc, argv, "-o");
    bool        repoFmt    = !hasFlag(argc, argv, "-r");

    if (schemaPath.empty()) {
        std::cerr << "Error: -s <schema.json> is required.\n";
        printUsage(argv[0]);
        return 1;
    }

    JsonValue schema = JsonValue::parseFile(schemaPath);
    if (schema.isNull()) {
        std::cerr << "Error: Cannot parse schema file: " << schemaPath << "\n";
        return 1;
    }

    int count = 10;
    if (!countStr.empty()) {
        try { count = std::stoi(countStr); } catch (...) {}
    }
    if (count < 1) count = 1;

    if (outputPath.empty()) outputPath = "output.json";

    SchemaGenerator gen;
    JsonValue result = repoFmt
        ? gen.generateRepository(schema, count)
        : gen.generateRawArray(schema, count);

    if (!result.saveToFile(outputPath)) {
        std::cerr << "Error: Cannot write: " << outputPath << "\n";
        return 1;
    }
    std::cout << "Generated " << count << " record(s)  ->  " << outputPath << "\n";
    return 0;
}
