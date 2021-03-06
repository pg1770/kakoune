#include "color.hh"

#include "exception.hh"

namespace Kakoune
{

Color str_to_color(const String& color)
{
    if (color == "default") return Colors::Default;
    if (color == "black")   return Colors::Black;
    if (color == "red")     return Colors::Red;
    if (color == "green")   return Colors::Green;
    if (color == "yellow")  return Colors::Yellow;
    if (color == "blue")    return Colors::Blue;
    if (color == "magenta") return Colors::Magenta;
    if (color == "cyan")    return Colors::Cyan;
    if (color == "white")   return Colors::White;

    static const Regex rgb_regex{"rgb:[0-9a-fA-F]{6}"};
    if (boost::regex_match(color, rgb_regex))
    {
        unsigned l;
        sscanf(color.c_str() + 4, "%x", &l);
        return { (unsigned char)((l >> 16) & 0xFF),
                 (unsigned char)((l >> 8) & 0xFF),
                 (unsigned char)(l & 0xFF) };
    }
    throw runtime_error("Unable to parse color '" + color + "'");
    return Colors::Default;
}

String color_to_str(Color color)
{
    switch (color.color)
    {
        case Colors::Default: return "default";
        case Colors::Black:   return "black";
        case Colors::Red:     return "red";
        case Colors::Green:   return "green";
        case Colors::Yellow:  return "yellow";
        case Colors::Blue:    return "blue";
        case Colors::Magenta: return "magenta";
        case Colors::Cyan:    return "cyan";
        case Colors::White:   return "white";
        case Colors::RGB:
        {
            char buffer[11];
            sprintf(buffer, "rgb:%02x%02x%02x", color.r, color.g, color.b);
            return buffer;
        }
    }
    kak_assert(false);
    return "default";
}

String option_to_string(Color color)
{
    return color_to_str(color);
}

void option_from_string(const String& str, Color& color)
{
    color = str_to_color(str);
}

}
