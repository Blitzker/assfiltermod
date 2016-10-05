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

#include "stdafx.h"

#include <codecvt>
#include "Tools.h"

// Find(oldString) and replace(newString) in a string(line)
void FindReplace(std::string& line, std::string& oldString, std::string& newString)
{
    const size_t oldSize = oldString.length();

    // do nothing if line is shorter than the string to find
    if (oldSize > line.length()) return;

    const size_t newSize = newString.length();
    for (size_t pos = 0; ; pos += newSize)
    {
        // Locate the substring to replace
        pos = line.find(oldString, pos);
        if (pos == std::string::npos) return;
        if (oldSize == newSize)
        {
            // if they're same size, use std::string::replace
            line.replace(pos, oldSize, newString);
        }
        else
        {
            // if not same size, replace by erasing and inserting
            line.erase(pos, oldSize);
            line.insert(pos, newString);
        }
    }
}

std::wstring s2ws(const std::string& str)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.from_bytes(str);
}

std::string ws2s(const std::wstring& wstr)
{
    using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

    return converterX.to_bytes(wstr);
}

void ParseSrtLine(std::string &srtLine, DWORD colorPrimary, DWORD colorOutline)
{
    // Replace srt styles to ass styles
    FindReplace(srtLine, std::string("<b>"), std::string("{\\b1}"));
    FindReplace(srtLine, std::string("</b>"), std::string("{\\b0}"));
    FindReplace(srtLine, std::string("<i>"), std::string("{\\i1}"));
    FindReplace(srtLine, std::string("</i>"), std::string("{\\i0}"));
    FindReplace(srtLine, std::string("<u>"), std::string("{\\u1}"));
    FindReplace(srtLine, std::string("</u>"), std::string("{\\u0}"));

    // Split the string at every LF (\n)
    std::vector<std::string> vecSrtLines;
    std::istringstream f(srtLine);
    std::string strTmp;
    while (std::getline(f, strTmp))
        vecSrtLines.push_back(strTmp);

    srtLine.clear();

    // Default font color
    char origFontAssTag[1024]{};
    _snprintf_s(origFontAssTag, _TRUNCATE, "{\\1c&H%6X&}{\\3c&H%6X&}", colorPrimary, colorOutline);

    // Check if there is a font color tag
    for (int cnt = 0; cnt < vecSrtLines.size(); cnt++)
    {
        size_t found = vecSrtLines[cnt].find("<font color=");
        if (found != std::string::npos)
        {
            std::string fntStr;         // Hold the full font color HTML tag
            char fntColor[100]{};       // Hold the color data
                                        // Look for hex color tag starting with # between quotes
            if (vecSrtLines[cnt].find("<font color=\"#") != std::string::npos)
            {
                fntStr = vecSrtLines[cnt].substr(found);
                sscanf_s(fntStr.c_str(), "<font color=\"#%6s\">", &fntColor, (unsigned int)sizeof(fntColor));
                fntStr = fntColor;
                fntStr.insert(0, "<font color=\"#").append("\">");
            }
            // look for text color tag between quotes
            else if (vecSrtLines[cnt].find("<font color=\"") != std::string::npos)
            {
                fntStr = vecSrtLines[cnt].substr(found);
                sscanf_s(fntStr.c_str(), "<font color=\"%[a-zA-Z]\">", &fntColor, (unsigned int)sizeof(fntColor));
                fntStr = fntColor;
                MatchColorSrt(fntColor, sizeof(fntColor));
                fntStr.insert(0, "<font color=\"").append("\">");
            }
            // Look for hex color tag starting with #
            else
            {
                fntStr = vecSrtLines[cnt].substr(found);
                sscanf_s(fntStr.c_str(), "<font color=#%6s>", &fntColor, (unsigned int)sizeof(fntColor));
                fntStr = fntColor;
                fntStr.insert(0, "<font color=#").append(">");
            }

            // font color tag in srt is RGB and we need BGR
            char tmpColor[7];
            strcpy_s(tmpColor, sizeof(tmpColor), fntColor);
            tmpColor[0] = fntColor[4];
            tmpColor[1] = fntColor[5];
            tmpColor[4] = fntColor[0];
            tmpColor[5] = fntColor[1];
            std::string assStr;
            assStr.append("{\\1c&H").append(tmpColor).append("&}{\\3c&H").append("000000").append("&}");
            FindReplace(vecSrtLines[cnt], fntStr, assStr);
        }
        srtLine.append(vecSrtLines[cnt]).append("\\N");
    }
    FindReplace(srtLine, std::string("</font>"), std::string(origFontAssTag));
    FindReplace(srtLine, std::string("\r"), std::string(""));
}

// Match color name to its hex counterpart
void MatchColorSrt(char *fntColor, size_t size)
{
    _strlwr_s(fntColor, size);
    for (int i = 0; i < _countof(color_tag); i++)
    {
        if (strcmp(fntColor, color_tag[i].color) == 0)
        {
            strcpy_s(fntColor, size, color_tag[i].hex);
            break;
        }
        // Return with color "white" if the color is not found in the array
        if (i == _countof(color_tag) - 1)
            strcpy_s(fntColor, size, "FFFFFF");
    }
}
