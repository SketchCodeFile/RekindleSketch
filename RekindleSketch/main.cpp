#include <algorithm>
#include <cctype>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "RekindleSketch.h"
#include "parm.h"

static std::string to_lower_copy(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

//==============================================================================
// Helper Functions
//==============================================================================

static std::vector<uint32_t> read_dat_packets(const std::string& filepath) {
    std::vector<uint32_t> packets;
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filepath << std::endl;
        return packets;
    }

    const std::streamsize file_size = file.tellg();
    if (file_size <= 0) {
        return packets;
    }

    if (file_size % static_cast<std::streamsize>(sizeof(uint32_t)) != 0) {
        std::cerr << "Warning: File size of " << filepath
                  << " is not a multiple of 32 bits; trailing bytes will be ignored."
                  << std::endl;
    }

    const size_t packet_count = static_cast<size_t>(file_size / static_cast<std::streamsize>(sizeof(uint32_t)));
    packets.resize(packet_count);

    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(packets.data()),
              static_cast<std::streamsize>(packet_count * sizeof(uint32_t)));

    if (!file) {
        std::cerr << "Error: Failed to read 32-bit flow IDs from " << filepath << std::endl;
        packets.clear();
    }

    return packets;
}

static std::vector<std::string> get_dat_files(const std::string& folder_path) {
    std::vector<std::string> dat_files;

    if (folder_path.empty()) {
        std::cerr << "Error: No data folder specified. Pass path as argument or set DATA_FOLDER_PATH in parm.h" << std::endl;
        return dat_files;
    }

    std::string normalized_folder = folder_path;
    while (!normalized_folder.empty()) {
        char last_char = normalized_folder.back();
        if (last_char == '/' || last_char == '\\') {
            normalized_folder.pop_back();
        } else {
            break;
        }
    }

    std::string command = "dir /b /a-d \"" + normalized_folder + "\\*.dat\"";
    FILE* pipe = _popen(command.c_str(), "r");
    if (pipe == nullptr) {
        std::cerr << "Error: Failed to enumerate .dat files in " << folder_path << std::endl;
        return dat_files;
    }

    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        std::string filename(buffer);
        while (!filename.empty() && (filename.back() == '\n' || filename.back() == '\r')) {
            filename.pop_back();
        }
        if (filename.empty()) {
            continue;
        }

        if (to_lower_copy(filename).size() >= 4 &&
            to_lower_copy(filename).substr(filename.size() - 4) == ".dat") {
            dat_files.push_back(normalized_folder + "\\" + filename);
        }
    }

    const int exit_code = _pclose(pipe);
    if (exit_code != 0 && dat_files.empty()) {
        std::cerr << "Error: Invalid folder path or no .dat files found in " << folder_path << std::endl;
    }

    std::sort(dat_files.begin(), dat_files.end());
    return dat_files;
}

//==============================================================================
// Main Entry Point
//==============================================================================

int main(int argc, char* argv[]) {
    RekindleSketch sketch;

    std::string folder_path = DATA_FOLDER_PATH;
    if (argc > 1) {
        folder_path = argv[1];
    }
    if (folder_path.empty()) {
        std::cerr << "Error: No data folder specified. Pass path as argument or set DATA_FOLDER_PATH in parm.h" << std::endl;
        return 1;
    }

    std::vector<std::string> dat_files = get_dat_files(folder_path);

    if (dat_files.empty()) {
        std::cerr << "Error: No .dat files found" << std::endl;
        return 1;
    }

    int processed_files = 0;

    for (size_t i = 0; i < dat_files.size() &&
         (TOTAL_WINDOWS == 0 || processed_files < TOTAL_WINDOWS); i++) {
        std::vector<uint32_t> packets = read_dat_packets(dat_files[i]);
        if (packets.empty()) continue;

        for (uint32_t packet : packets) {
            sketch.update(std::to_string(packet));
        }

        processed_files++;
        sketch.end_window();
        sketch.sliding_window_query(DALTA);
    }

    // Handle edge case: query at the end if needed
    if (processed_files == TOTAL_WINDOWS && TOTAL_WINDOWS > 0) {
        int current_window = sketch.current_window();
        int end_window = current_window - 1;
        int query_interval = (end_window - R) % S;

        if (current_window > R && query_interval == 0) {
            const std::deque<RekindleSketch::SlidingWindowResult>& results = sketch.get_sliding_window_results();
            bool already_queried = !results.empty() && results.back().end_window == end_window;
            if (!already_queried) {
                sketch.sliding_window_query(DALTA);
            }
        }
    }

    return 0;
}
