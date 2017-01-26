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

#include "AssFilterSettings.h"

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

static const struct s_iso639_lang {
    const char *language;
    const char *lang3;
    const char *lang2;
} iso639_lang[] = {
    { "Albanian",               "alb", "sq" },
    { "Arabic",                 "ara", "ar" },
    { "Armenian",               "arm", "hy" },
    { "Basque",                 "baq", "eu" },
    { "Belarusian",             "bel", "be" },
    { "Bosnian",                "bos", "bs" },
    { "Bulgarian",              "bul", "bg" },
    { "Catalan",                "cat", "ca" },
    { "Chinese",                "chi", "zh" },
    { "Croatian",               "hrv", "hr" },
    { "Czech",                  "cze", "cs" },
    { "Danish",                 "dan", "da" },
    { "Dutch",                  "dut", "nl" },
    { "English",                "eng", "en" },
    { "Finnish",                "fin", "fi" },
    { "French",                 "fre", "fr" },
    { "Georgian",               "geo", "ka" },
    { "German",                 "ger", "de" },
    { "Greek",                  "gre", "el" },
    { "Haitian",                "hat", "ht" },
    { "Hebrew",                 "heb", "he" },
    { "Hungarian",              "hun", "hu" },
    { "Indonesian",             "ind", "id" },
    { "Irish",                  "gle", "ga" },
    { "Italian",                "ita", "it" },
    { "Japanese",               "jpn", "ja" },
    { "Korean",                 "kor", "ko" },
    { "Kurdish",                "kur", "ku" },
    { "Lao",                    "lao", "lo" },
    { "Latin",                  "lat", "la" },
    { "Lithuanian",             "lit", "lt" },
    { "Macedonian",             "mac", "mk" },
    { "Malay",                  "may", "ms" },
    { "Malayalam",              "mal", "ml" },
    { "Maltese",                "mlt", "mt" },
    { "Mongolian",              "mon", "mn" },
    { "Norwegian",              "nor", "no" },
    { "Persian",                "per", "fa" },
    { "Polish",                 "pol", "pl" },
    { "Portuguese",             "por", "pt" },
    { "Romanian",               "rum", "ro" },
    { "Russian",                "rus", "ru" },
    { "Serbian",                "srp", "sr" },
    { "Slovak",                 "slo", "sk" },
    { "Slovenian",              "slv", "sl" },
    { "Somali",                 "som", "so" },
    { "Spanish",                "spa", "es" },
    { "Swedish",                "swe", "sv" },
    { "Thai",                   "tha", "th" },
    { "Tibetan",                "tib", "bo" },
    { "Turkish",                "tur", "tr" },
    { "Ukrainian",              "ukr", "uk" },
    { "Undetermined",           "und", "ud" },
    { "Vietnamese",             "vie", "vi" },
    { "Zhuang",                 "zha", "za" },

};

template <typename T>
void FindReplace(T& line, const T& oldString, const T& newString);
std::wstring s2ws(const std::string& str);
std::string ws2s(const std::wstring& wstr);

std::string ConsumeAttribute(const char** ppsz_subtitle, std::string& attribute_value);
std::string GetTag(const char** line, bool b_closing);
bool IsClosed(const char* psz_subtitle, const char* psz_tagname);
void ParseSrtLine(std::string& srtLine, const AssFSettings& settings);
void MatchColorSrt(std::string& fntColor);
std::string MatchLanguage(std::string& langCode, bool isCode2Chars = false);

inline void swapRGBtoBGR(std::string& color)
{
    std::string tmp = color;

    color[0] = tmp[4];
    color[1] = tmp[5];
    color[4] = tmp[0];
    color[5] = tmp[1];
}
