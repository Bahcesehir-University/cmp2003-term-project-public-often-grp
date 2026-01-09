#include "analyzer.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#include <vector>

static int parseHour(const std::string& timestamp) {
    if (timestamp.size() < 13) return -1;


    size_t spacePos = timestamp.find(' ');
    if (spacePos == std::string::npos) return -1;


    if (spacePos + 2 >= timestamp.size()) return -1;

    char d1 = timestamp[spacePos + 1];
    char d2 = timestamp[spacePos + 2];

    if (!isdigit(d1) || !isdigit(d2)) return -1;


    int hour = (d1 - '0') * 10 + (d2 - '0');

    if (hour < 0 || hour > 23) return -1;
    return hour;
}

void TripAnalyzer::ingestFile(const std::string& csvPath) {
    std::ifstream file(csvPath);
    if (!file.is_open()) {
        return;
    }

    std::string line;

    if (!std::getline(file, line)) return;


    while (std::getline(file, line)) {
        if (line.empty()) continue;


        size_t c1 = line.find(',');
        if (c1 == std::string::npos) continue;


        size_t c2 = line.find(',', c1 + 1);
        if (c2 == std::string::npos) continue;


        std::string zone = line.substr(c1 + 1, c2 - (c1 + 1));
        if (zone.empty()) continue;


        size_t c3 = line.find(',', c2 + 1);
        if (c3 == std::string::npos) continue;


        size_t c4 = line.find(',', c3 + 1);


        if (c4 == std::string::npos) continue;

        std::string timeStr = line.substr(c3 + 1, c4 - (c3 + 1));

        int hour = parseHour(timeStr);
        if (hour == -1) continue;


        m_zoneCounts[zone]++;
        m_zoneHourlyCounts[zone][hour]++;
    }
}

std::vector<ZoneCount> TripAnalyzer::topZones(int k) const {
    std::vector<ZoneCount> result;
    result.reserve(m_zoneCounts.size());

    for (const auto& kv : m_zoneCounts) {
        result.push_back({kv.first, kv.second});
    }


    std::sort(result.begin(), result.end(), [](const ZoneCount& a, const ZoneCount& b) {
        if (a.count != b.count) {
            return a.count > b.count;
        }
        return a.zone < b.zone;
    });

    if (k >= 0 && (size_t)k < result.size()) {
        result.resize(k);
    }
    return result;
}

std::vector<SlotCount> TripAnalyzer::topBusySlots(int k) const {
    std::vector<SlotCount> result;

    for (const auto& zKv : m_zoneHourlyCounts) {
        const std::string& zone = zKv.first;
        const auto& hours = zKv.second;

        for (int h = 0; h < 24; ++h) {
            long long count = hours[h];
            if (count > 0) {
                result.push_back({zone, h, count});
            }
        }
    }


    std::sort(result.begin(), result.end(), [](const SlotCount& a, const SlotCount& b) {
        if (a.count != b.count) {
            return a.count > b.count;
        }
        if (a.zone != b.zone) {
            return a.zone < b.zone;
        }
        return a.hour < b.hour;
    });

    if (k >= 0 && (size_t)k < result.size()) {
        result.resize(k);
    }
    return result;
}

