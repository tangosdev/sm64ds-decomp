#include "hal/discord_presence.h"

#include <cassert>
#include <string>

int main()
{
    sm64ds::discord::Activity activity;
    activity.details = "Playing Bob-omb Battlefield";
    activity.state = "Adventure \"Co-op\"";
    activity.start_timestamp = 123456789;
    const std::string json = sm64ds::discord::make_activity_json(activity, 42);
    assert(json.find("Playing Bob-omb Battlefield") != std::string::npos);
    assert(json.find("Adventure \\\"Co-op\\\"") != std::string::npos);
    assert(json.find("\"pid\":42") != std::string::npos);
    assert(json.find("https://github.com/tangosdev/sm64ds-decomp") != std::string::npos);
    assert(json.find("SahilKDas") == std::string::npos);
    assert(json.find("\"start\":123456789") != std::string::npos);
    return 0;
}
