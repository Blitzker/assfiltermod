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

#include <fstream>
#include "ass.h"
#include "AssFilterSettings.h"
#include "utf8.h"

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
    const WCHAR *language;
    const WCHAR *lang3;
    const WCHAR *lang2;
    const UINT codepage;
} iso639_lang[] = {
    { L"Albanian",               L"alb", L"sq", 1250 },
    { L"Arabic",                 L"ara", L"ar", 1256 },
    { L"Armenian",               L"arm", L"hy",    0 },
    { L"Basque",                 L"baq", L"eu", 1252 },
    { L"Belarusian",             L"bel", L"be", 1251 },
    { L"Bosnian",                L"bos", L"bs", 1250 },
    { L"Bulgarian",              L"bul", L"bg", 1251 },
    { L"Catalan",                L"cat", L"ca", 1252 },
    { L"Chinese",                L"chi", L"zh",    0 },
    { L"Croatian",               L"hrv", L"hr", 1250 },
    { L"Czech",                  L"cze", L"cs", 1250 },
    { L"Danish",                 L"dan", L"da", 1252 },
    { L"Dutch",                  L"dut", L"nl", 1252 },
    { L"English",                L"eng", L"en", 1252 },
    { L"Finnish",                L"fin", L"fi", 1252 },
    { L"French",                 L"fre", L"fr", 1252 },
    { L"Georgian",               L"geo", L"ka",    0 },
    { L"German",                 L"ger", L"de", 1252 },
    { L"Greek",                  L"gre", L"el", 1253 },
    { L"Haitian",                L"hat", L"ht", 1252 },
    { L"Hebrew",                 L"heb", L"he", 1255 },
    { L"Hungarian",              L"hun", L"hu", 1250 },
    { L"Indonesian",             L"ind", L"id", 1252 },
    { L"Irish",                  L"gle", L"ga", 1252 },
    { L"Italian",                L"ita", L"it", 1252 },
    { L"Japanese",               L"jpn", L"ja",  932 },
    { L"Korean",                 L"kor", L"ko",  949 },
    { L"Kurdish",                L"kur", L"ku",    0 },
    { L"Lao",                    L"lao", L"lo",    0 },
    { L"Latin",                  L"lat", L"la", 1252 },
    { L"Lithuanian",             L"lit", L"lt", 1257 },
    { L"Macedonian",             L"mac", L"mk", 1251 },
    { L"Malay",                  L"may", L"ms", 1252 },
    { L"Malayalam",              L"mal", L"ml",    0 },
    { L"Maltese",                L"mlt", L"mt",    0 },
    { L"Mongolian",              L"mon", L"mn", 1251 },
    { L"Norwegian",              L"nor", L"no", 1252 },
    { L"Persian",                L"per", L"fa", 1256 },
    { L"Polish",                 L"pol", L"pl", 1250 },
    { L"Portuguese",             L"por", L"pt", 1252 },
    { L"Romanian",               L"rum", L"ro", 1250 },
    { L"Russian",                L"rus", L"ru", 1251 },
    { L"Serbian",                L"srp", L"sr", 1250 },
    { L"Slovak",                 L"slo", L"sk", 1250 },
    { L"Slovenian",              L"slv", L"sl", 1250 },
    { L"Somali",                 L"som", L"so",    0 },
    { L"Spanish",                L"spa", L"es", 1252 },
    { L"Swedish",                L"swe", L"sv", 1252 },
    { L"Thai",                   L"tha", L"th",  874 },
    { L"Tibetan",                L"tib", L"bo",    0 },
    { L"Turkish",                L"tur", L"tr", 1254 },
    { L"Ukrainian",              L"ukr", L"uk", 1251 },
    { L"Undetermined",           L"und", L"ud",    0 },
    { L"Vietnamese",             L"vie", L"vi", 1258 },
    { L"Zhuang",                 L"zha", L"za",    0 },

};

template <typename T>
void FindReplace(T& line, const T& oldString, const T& newString);
std::wstring s2ws(const std::string& str);
std::string ws2s(const std::wstring& wstr);

void ConvertCPToUTF8(UINT CP, std::string& codepage_str);
UINT GetLanguageCP(const std::wstring& langCode, bool isCode2Chars = false);

std::wstring ExtractYuvMatrix(const std::wstring& file);
std::string ConsumeAttribute(const char** ppsz_subtitle, std::string& attribute_value);
std::string GetTag(const char** line, bool b_closing);
bool IsClosed(const char* psz_subtitle, const char* psz_tagname);
void ParseSrtLine(std::string& srtLine, const AssFSettings& settings);
void MatchColorSrt(std::string& fntColor);
std::wstring MatchLanguage(const std::wstring& langCode, bool isCode2Chars = false);
ASS_Track* srt_read_file(ASS_Library* library, char* fname, const AssFSettings& settings, const UINT codePage = 0);
std::wstring ParseFontsPath(std::wstring fontsDir, const std::wstring& name);

inline bool dirExists(const std::wstring& dirName)
{
    DWORD ftyp = GetFileAttributesW(dirName.c_str());

    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false;   //something is wrong with your path!

    if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
        return true;    // this is a directory!

    return false;       // this is not a directory!
}

inline bool fexists(const std::wstring& name)
{
    return std::ifstream(name).good();
}

inline bool isFileUTF8(const std::wstring& name)
{
    std::ifstream ifs(name);
    std::istreambuf_iterator<char> it(ifs.rdbuf());
    std::istreambuf_iterator<char> eos;

    return utf8::is_valid(it, eos);
}

// trim from end of string (right)
inline std::string& rtrim(std::string& s, const char* t = " \t\n\r")
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim from beginning of string (left)
inline std::string& ltrim(std::string& s, const char* t = " \t\n\r")
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from both ends of string (left & right)
inline std::string& trim(std::string& s, const char* t = " \t\n\r")
{
    return ltrim(rtrim(s, t), t);
}

inline void swapRGBtoBGR(std::string& color)
{
    std::string tmp = color;

    color[0] = tmp[4];
    color[1] = tmp[5];
    color[4] = tmp[0];
    color[5] = tmp[1];
}
