#include "discord_presence.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cstdint>
#include <sstream>

namespace sm64ds::discord {
namespace {

constexpr std::uint32_t kHandshake = 0;
constexpr std::uint32_t kFrame = 1;
constexpr const char *kTangoRepository = "https://github.com/tangosdev/sm64ds-decomp";

std::string escape_json(const std::string &value)
{
    std::string escaped;
    escaped.reserve(value.size());
    for (unsigned char ch : value) {
        switch (ch) {
        case '\\': escaped += "\\\\"; break;
        case '"': escaped += "\\\""; break;
        case '\b': escaped += "\\b"; break;
        case '\f': escaped += "\\f"; break;
        case '\n': escaped += "\\n"; break;
        case '\r': escaped += "\\r"; break;
        case '\t': escaped += "\\t"; break;
        default:
            if (ch < 0x20) {
                static const char hex[] = "0123456789abcdef";
                escaped += "\\u00";
                escaped += hex[ch >> 4];
                escaped += hex[ch & 0xf];
            } else {
                escaped += static_cast<char>(ch);
            }
        }
    }
    return escaped;
}

bool write_frame(HANDLE pipe, std::uint32_t opcode, const std::string &json)
{
    const std::uint32_t size = static_cast<std::uint32_t>(json.size());
    std::uint32_t header[2] = {opcode, size};
    DWORD written = 0;
    return WriteFile(pipe, header, sizeof(header), &written, nullptr) &&
           written == sizeof(header) &&
           WriteFile(pipe, json.data(), size, &written, nullptr) && written == size;
}

bool read_frame(HANDLE pipe)
{
    std::uint32_t header[2] = {};
    DWORD read = 0;
    if (!ReadFile(pipe, header, sizeof(header), &read, nullptr) || read != sizeof(header) ||
        header[1] > 1024 * 1024)
        return false;
    std::string payload(header[1], '\0');
    return ReadFile(pipe, payload.data(), header[1], &read, nullptr) && read == header[1];
}

} // namespace

std::string make_activity_json(const Activity &activity, unsigned long process_id)
{
    std::ostringstream json;
    json << "{\"cmd\":\"SET_ACTIVITY\",\"args\":{\"pid\":" << process_id
         << ",\"activity\":{\"details\":\"" << escape_json(activity.details)
         << "\",\"state\":\"" << escape_json(activity.state) << '"';
    if (activity.start_timestamp > 0)
        json << ",\"timestamps\":{\"start\":" << activity.start_timestamp << '}';
    json << ",\"buttons\":[{\"label\":\"SM64DS source\",\"url\":\""
         << kTangoRepository << "\"}]}} ,\"nonce\":\"sm64ds-" << GetTickCount64() << "\"}";
    return json.str();
}

Presence::Presence() : pipe_(INVALID_HANDLE_VALUE) {}
Presence::~Presence() { disconnect(); }

bool Presence::connect(const std::string &application_id)
{
    disconnect();
    if (application_id.empty())
        return false;
    for (int index = 0; index != 10; ++index) {
        const std::string name = "\\\\.\\pipe\\discord-ipc-" + std::to_string(index);
        HANDLE pipe = CreateFileA(name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr,
                                  OPEN_EXISTING, 0, nullptr);
        if (pipe == INVALID_HANDLE_VALUE)
            continue;
        const std::string hello = "{\"v\":1,\"client_id\":\"" +
                                  escape_json(application_id) + "\"}";
        if (write_frame(pipe, kHandshake, hello) && read_frame(pipe)) {
            pipe_ = pipe;
            return true;
        }
        CloseHandle(pipe);
    }
    return false;
}

bool Presence::update(const Activity &activity)
{
    HANDLE pipe = static_cast<HANDLE>(pipe_);
    if (pipe == INVALID_HANDLE_VALUE)
        return false;
    if (!write_frame(pipe, kFrame, make_activity_json(activity, GetCurrentProcessId()))) {
        disconnect();
        return false;
    }
    return true;
}

void Presence::disconnect()
{
    HANDLE pipe = static_cast<HANDLE>(pipe_);
    if (pipe != INVALID_HANDLE_VALUE)
        CloseHandle(pipe);
    pipe_ = INVALID_HANDLE_VALUE;
}

bool Presence::connected() const
{
    return static_cast<HANDLE>(pipe_) != INVALID_HANDLE_VALUE;
}

} // namespace sm64ds::discord
