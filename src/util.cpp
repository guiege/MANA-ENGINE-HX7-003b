#include "Util.h"

bool intersect(const rect& r1, const rect& r2)
{
    return (r1.x < r2.x + r2.width &&
            r1.x + r1.width > r2.x &&
            r1.y < r2.y + r2.height &&
            r1.y + r1.height > r2.y);
}


void writeHurtboxesToFile(const std::map<int, std::vector<rect>>& hurtboxes, const std::string& filename) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return;
    }

    for (const auto& [frame, rects] : hurtboxes) {
        for (const auto& r : rects) {
            file << frame << " " << r.type << " " << r.x << " " << r.y << " " << r.width << " " << r.height << "\n";
        }
    }

    file.close();
    std::cout << "Hurtboxes written to file: " << filename << std::endl;
}

void readHurtboxesFromFile(std::map<int, std::vector<rect>>& hurtboxes, const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        int frame, type, x, y, width, height;
        if (iss >> frame >> type >> x >> y >> width >> height) {
            hurtboxes[frame].emplace_back(rect{x, y, width, height, type});
        } else {
            std::cerr << "Error: Invalid line format in file " << filename << std::endl;
        }
    }

    file.close();
    std::cout << "Hurtboxes read from file: " << filename << std::endl;
}

void readHurtboxesFromFile(std::map<int, std::vector<rect>>& hurtboxes, std::map<int, std::vector<rect>>& hitboxes, std::vector<rect>& pushboxes, const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return;
    }
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        std::istringstream iss(line);
        int frame, type, x, y, width, height;
        if (iss >> frame >> type >> x >> y >> width >> height) {
            if (frame < 0 || type < 0 || type > 2) {
                std::cerr << "Error: Invalid frame or type at line " << lineNumber << std::endl;
                continue;
            }
            if(type == 0)
                hurtboxes[frame].emplace_back(rect{x, y, width, height, type});
            else if(type == 1)
                hitboxes[frame].emplace_back(rect{x, y, width, height, type});
            else if(type == 2) {
                if (frame >= pushboxes.size()) {
                    pushboxes.resize(frame + 1);
                }
                pushboxes[frame] = rect{x, y, width, height, type};
            }
        } else {
            std::cerr << "Error: Invalid line format at line " << lineNumber << ": " << line << std::endl;
        }
    }
    file.close();
    std::cout << "Finished reading file: " << filename << std::endl;
}

void sortBoxesByArea(std::vector<rect>& boxes) {
    std::sort(boxes.begin(), boxes.end(), [](const rect& a, const rect& b) {
        return a.area() < b.area();
    });
}

void sortAllBoxesInFrames(std::map<int, std::vector<rect>>& hurtboxes) {
    for (auto& frame : hurtboxes) {
        std::vector<rect>& boxes = frame.second;
        if (!boxes.empty()) {
            sortBoxesByArea(boxes);
        }
    }
}

int extractIntFromVal(const std::string& input){
    size_t start = input.find('(');
    size_t end = input.find(')');
    int value = 0;

    if (start != std::string::npos && end != std::string::npos && end > start) {
        std::string inner = input.substr(start + 1, end - start - 1);
        value = stoi(inner);
    }
    return value;
}