#pragma once
#include <string>
#include <vector>
#include <unordered_map>

struct ZoneCount {
    std::string zone;
    long long count;
};

struct SlotCount {
    std::string zone;
    int hour;              // 0–23
    long long count;
};

class TripAnalyzer {
private:
    // Data structures to store aggregated counts
    std::unordered_map<std::string, long long> zoneCounts;

    // Custom key for (zone, hour) pair
    using SlotKey = std::pair<std::string, int>;
    struct SlotKeyHash {
        std::size_t operator()(const SlotKey& k) const {
            return std::hash<std::string>{}(k.first) ^ (std::hash<int>{}(k.second) << 1);
        }
    };
    std::unordered_map<SlotKey, long long, SlotKeyHash> slotCounts;

public:
    void ingestFile(const std::string& csvPath);

    std::vector<ZoneCount> topZones(int k = 10) const;

    std::vector<SlotCount> topBusySlots(int k = 10) const;
};
