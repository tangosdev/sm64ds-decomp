#pragma once

#include <string>

namespace sm64ds::discord {

struct Activity {
    std::string details;
    std::string state;
    long long start_timestamp = 0;
};

std::string make_activity_json(const Activity &activity, unsigned long process_id);

class Presence {
public:
    Presence();
    ~Presence();

    Presence(const Presence &) = delete;
    Presence &operator=(const Presence &) = delete;

    bool connect(const std::string &application_id);
    bool update(const Activity &activity);
    void disconnect();
    bool connected() const;

private:
    void *pipe_;
};

} // namespace sm64ds::discord
