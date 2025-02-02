#ifndef _CORE_JSONUTIL_H
#define _CORE_JSONUTIL_H

#include "direction.h"
#include <json/json.hpp>
#include <string>
#include <list>


static nlohmann::json parse_jsonc(std::string& s)
{
    using nlohmann::json;
    json j;
    // TODO: change json library to allow trailing commas instead
    // NOTE: we have to compile with -fexceptions just for this
    for (size_t i=0; i<1000; i++) { // fixing max 1000 errors before giving up
        try
        {
            j = json::parse(s, nullptr, true, true);
        }
        catch (json::parse_error& e)
        {
            if (e.id == 101 && e.byte > 0 /*&& e.byte < s.length()*/) {
                //auto pos = s.find_last_not_of(" \t\n\r", e.byte-2); // this may not work with comments
                auto pos = s.find_last_of(',', e.byte-1); // this may do random stuff in invalid files
                if (pos != s.npos/* && s[pos]==','*/) {
                    s[pos]=' ';
                    continue;
                }
            }
            fprintf(stderr, "Could not fix json:\n%s\n", e.what());
            // TODO: re-throw?
        }
        break;
    }
    return j;
}

static void commasplit(const std::string& s, std::list<std::string>& l)
{
    auto sta = s.find_first_not_of(' '); // ltrim
    auto end = s.find(',');
    while (end != std::string::npos) {
        auto p = s.find_last_not_of(' ', end-1)+1; // rtrim
        l.push_back(s.substr(sta,p-sta));
        sta = s.find_first_not_of(' ', end + 1); // ltrim
        end = s.find(',',sta);
    }
    end = s.find_last_not_of(' ')+1; // rtrim
    if (sta < end) l.push_back(s.substr(sta,end-sta));
}
static std::list<std::string> commasplit(const std::string& s)
{
    std::list<std::string> lst;
    commasplit(s, lst);
    return lst;
}
static std::list<std::string> commasplit(std::string&& s)
{
    std::string tmp = s;
    return commasplit(s);
}


static bool to_bool(const nlohmann::json& j, bool dflt)
{
    if (j.type() == nlohmann::json::value_t::boolean) return j.get<bool>();
    else if (j.type() == nlohmann::json::value_t::string) {
        auto s = j.get<std::string>();
        if (strcasecmp(s.c_str(), "true") == 0) return true;
        if (strcasecmp(s.c_str(), "false") == 0) return false;
    }
    return dflt;
}
static std::string to_string(const nlohmann::json& j, const std::string& dflt)
{
    return (j.type() == nlohmann::json::value_t::string) ? j.get<std::string>() : dflt;
}
static std::string to_string(const nlohmann::json& j, const std::string& key, const std::string& dflt)
{
    if (j.type() != nlohmann::json::value_t::object) return dflt;
    auto it = j.find(key);
    if (it == j.end()) return dflt;
    return to_string(it.value(),dflt);
}
static int to_int(const nlohmann::json& j, int dflt)
{
    // TODO: float?
    return (j.type() == nlohmann::json::value_t::number_integer) ? j.get<int>() : 
           (j.type() == nlohmann::json::value_t::number_unsigned) ? (int)j.get<unsigned>() : dflt;
}
static Direction to_direction(const nlohmann::json& j, Direction dflt=Direction::UNDEFINED)
{
    std::string s = to_string(j,"");
    if (s == "left") return Direction::LEFT;
    if (s == "right") return Direction::RIGHT;
    if (s == "up" || s == "top") return Direction::UP;
    if (s == "down" || s == "bottom") return Direction::DOWN;
    return dflt;
}

#endif
