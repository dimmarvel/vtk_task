#include <cstdint>
#include <string>
#include <unordered_map>

namespace api
{
    enum source : uint8_t
    {
        VMC = (1 << 0),
        POS = (1 << 1),
        UNKNOWN = (1 << 2)
    };

    enum TAG_TYPE : uint8_t
    {
        T1 = 0x1,
        T2 = 0x2
    };
    
    struct params_info
    {
        std::string descr;
        std::string format;
        uint8_t max_length = 0;
        source src = UNKNOWN;
    };

    #define NO_LIMIT UINT8_MAX
    const std::unordered_map<uint8_t, params_info> PARAMS_DESCR
    {
    { 0x01, {"Message name",                    "ASCII",            3,          (source)(VMC | POS) }},
    { 0x03, {"Operation number",                "Decimal in ASCII", 8,          (source)(VMC | POS) }}, 
    { 0x04, {"Amount in minor currency unit",   "Decimal in ASCII", 12,         (source)(VMC | POS) }}, 
    { 0x05, {"Keepalive interval in seconds",   "Decimal in ASCII", 3,          (source)(POS)       }}, 
    { 0x06, {"Operation timeout in seconds",    "Decimal in ASCII", 3,          (source)(POS)       }}, 
    { 0x07, {"Event name",                      "ASCII",            5,          (source)(VMC | POS) }},
    { 0x08, {"Event number",                    "Decimal in ASCII", 8,          (source)(VMC | POS) }}, 
    { 0x09, {"Product id",                      "Decimal in ASCII", 6,          (source)(VMC | POS) }}, 
    { 0x0A, {"QR-Code data",                    "ASCII",            NO_LIMIT,   (source)(VMC)       }}, 
    { 0x0B, {"TCP/IP destination",              "Binary ASCII",     NO_LIMIT,   (source)(VMC | POS) }}, 
    { 0x0C, {"Outgoing byte counter",           "Decimal in ASCII", 12,         (source)(VMC)       }}, 
    { 0x0D, {"Simple data block",               "Binary ASCII",     NO_LIMIT,   (source)(VMC | POS) }}, 
    { 0x0E, {"Confirmable data block",          "Binary ASCII",     NO_LIMIT,   (source)(POS)       }}, 
    { 0x0F, {"Product name",                    "ASCII",            NO_LIMIT,   (source)(VMC)       }}, 
    { 0x10, {"POS management data",             "ASCII",            NO_LIMIT,   (source)(VMC | POS) }}, 
    { 0x11, {"Local time",                      "ASCII",            NO_LIMIT,   (source)(VMC | POS) }}, 
    { 0x12, {"System information",              "ASCII",            NO_LIMIT,   (source)(POS)       }}, 
    { 0x13, {"Banking receipt",                 "UTF-8",            NO_LIMIT,   (source)(POS)       }}, 
    { 0x14, {"Display time in milliseconds",    "ASCII",            8,          (source)(VMC)       }}
    }
}