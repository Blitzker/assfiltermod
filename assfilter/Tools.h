/*
 *   Copyright(C) 2016 Blitzker
 *
 *   This program is free software : you can redistribute it and / or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <string>

static const struct s_color_tag {
    const char *color;
    const char *hex;
} color_tag[] = {
//    name              hex value
    { "aqua",           "00FFFF" },
    { "azure",          "F0FFFF" },
    { "beige",          "F5F5DC" },
    { "black",          "000000" },
    { "blue",           "0000FF" },
    { "brown",          "A52A2A" },
    { "chartreuse",     "7FFF00" },
    { "chocolate",      "D2691E" },
    { "coral",          "FF7F50" },
    { "crimson",        "DC143C" },
    { "cyan",           "00FFFF" },
    { "fuchsia",        "FF00FF" },
    { "gold",           "FFD700" },
    { "gray",           "808080" },
    { "green",          "008000" },
    { "grey",           "808080" },
    { "indigo",         "4B0082" },
    { "ivory",          "FFFFF0" },
    { "khaki",          "F0E68C" },
    { "lavender",       "E6E6FA" },
    { "lime",           "00FF00" },
    { "linen",          "FAF0E6" },
    { "magenta",        "FF00FF" },
    { "maroon",         "800000" },
    { "navy",           "000080" },
    { "olive",          "808000" },
    { "orange",         "FFA500" },
    { "orchid",         "DA70D6" },
    { "pink",           "FFC0CB" },
    { "plum",           "DDA0DD" },
    { "purple",         "800080" },
    { "red",            "FF0000" },
    { "salmon",         "FA8072" },
    { "sienna",         "A0522D" },
    { "silver",         "C0C0C0" },
    { "snow",           "FFFAFA" },
    { "tan",            "D2B48C" },
    { "teal",           "008080" },
    { "thistle",        "D8BFD8" },
    { "tomato",         "FF6347" },
    { "turquoise",      "40E0D0" },
    { "violet",         "EE82EE" },
    { "white",          "FFFFFF" },
    { "yellow",         "FFFF00" },
};

void FindReplace(std::string& line, std::string& oldString, std::string& newString);
std::wstring s2ws(const std::string& str);
std::string ws2s(const std::wstring& wstr);

void ParseSrtLine(std::string &srtLine, DWORD colorPrimary, DWORD colorOutline);
void MatchColorSrt(char *fntColor, size_t size);
