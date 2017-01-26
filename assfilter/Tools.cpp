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

#include <algorithm>
#include <codecvt>
#include <Shlwapi.h>
#include "Tools.h"

// Find(oldString) and replace(newString) in a string(line)
template <typename T>
void FindReplace(T& line, const T& oldString, const T& newString)
{
    const size_t oldSize = oldString.length();

    // do nothing if line is shorter than the string to find
    if (oldSize > line.length()) return;

    const size_t newSize = newString.length();
    for (size_t pos = 0; ; pos += newSize)
    {
        // Locate the substring to replace
        pos = line.find(oldString, pos);
        if (pos == T::npos) return;
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

std::string ConsumeAttribute(const char** ppsz_subtitle, std::string& attribute_value)
{
    const char* psz_subtitle = *ppsz_subtitle;
    char psz_attribute_name[BUFSIZ];
    char psz_attribute_value[BUFSIZ];

    while (*psz_subtitle == ' ')
        psz_subtitle++;

    size_t attr_len = 0;
    char delimiter;

    while (*psz_subtitle && isalpha(*psz_subtitle))
    {
        psz_subtitle++;
        attr_len++;
    }

    if (!*psz_subtitle || attr_len == 0)
        return std::string();

    strncpy_s(psz_attribute_name, psz_subtitle - attr_len, attr_len);
    psz_attribute_name[attr_len] = 0;

    // Skip over to the attribute value
    while (*psz_subtitle && *psz_subtitle != '=')
        psz_subtitle++;

    // Skip the '=' sign
    psz_subtitle++;

    // Aknowledge the delimiter if any
    while (*psz_subtitle && isspace(*psz_subtitle))
        psz_subtitle++;

    if (*psz_subtitle == '\'' || *psz_subtitle == '"')
    {
        // Save the delimiter and skip it
        delimiter = *psz_subtitle;
        psz_subtitle++;
    }
    else
        delimiter = 0;

    // Skip spaces, just in case
    while (*psz_subtitle && isspace(*psz_subtitle))
        psz_subtitle++;

    // Skip the first #
    if (*psz_subtitle == '#')
        psz_subtitle++;

    attr_len = 0;
    while (*psz_subtitle && ((delimiter != 0 && *psz_subtitle != delimiter) ||
        (delimiter == 0 && (isalnum(*psz_subtitle) || *psz_subtitle == '#'))))
    {
        psz_subtitle++;
        attr_len++;
    }

    strncpy_s(psz_attribute_value, psz_subtitle - attr_len, attr_len);
    psz_attribute_value[attr_len] = 0;
    attribute_value.assign(psz_attribute_value);

    // Finally, skip over the final delimiter
    if (delimiter != 0 && *psz_subtitle)
        psz_subtitle++;

    *ppsz_subtitle = psz_subtitle;

    return std::string(psz_attribute_name);
}

std::string GetTag(const char** line, bool b_closing)
{
    const char* psz_subtitle = *line;

    if (*psz_subtitle != '<')
        return std::string();

    // Skip the '<'
    psz_subtitle++;

    if (b_closing && *psz_subtitle == '/')
        psz_subtitle++;

    // Skip potential spaces
    while (*psz_subtitle == ' ')
        psz_subtitle++;

    // Now we need to verify if what comes next is a valid tag:
    if (!isalpha(*psz_subtitle))
        return std::string();

    size_t tag_size = 1;
    while (isalnum(psz_subtitle[tag_size]) || psz_subtitle[tag_size] == '_')
        tag_size++;

    char psz_tagname[BUFSIZ];
    strncpy_s(psz_tagname, psz_subtitle, tag_size);
    psz_tagname[tag_size] = 0;
    psz_subtitle += tag_size;
    *line = psz_subtitle;

    return std::string(psz_tagname);
}

bool IsClosed(const char* psz_subtitle, const char* psz_tagname)
{
    const char* psz_tagpos = StrStrIA(psz_subtitle, psz_tagname);

    if (!psz_tagpos)
        return false;

    // Search for '</' and '>' immediatly before & after (minding the potential spaces)
    const char* psz_endtag = psz_tagpos + strlen(psz_tagname);

    while (*psz_endtag == ' ')
        psz_endtag++;

    if (*psz_endtag != '>')
        return false;

    // Skip back before the tag itself
    psz_tagpos--;
    while (*psz_tagpos == ' ' && psz_tagpos > psz_subtitle)
        psz_tagpos--;

    if (*psz_tagpos-- != '/')
        return false;

    if (*psz_tagpos != '<')
        return false;

    return true;
}

void ParseSrtLine(std::string& srtLine, const AssFSettings& settings)
{
    const char *psz_subtitle = srtLine.data();
    std::string subtitle_output;

    while (*psz_subtitle)
    {
        /* HTML extensions */
        if (*psz_subtitle == '<')
        {
            std::string tagname = GetTag(&psz_subtitle, false);
            if (!tagname.empty())
            {
                // Convert tagname to lowercase
                std::transform(tagname.begin(), tagname.end(), tagname.begin(), ::tolower);
                if (tagname == "br")
                {
                    subtitle_output.append("\\N");
                }
                else if (tagname == "b")
                {
                    subtitle_output.append("{\\b1}");
                }
                else if (tagname == "i")
                {
                    subtitle_output.append("{\\i1}");
                }
                else if (tagname == "u")
                {
                    subtitle_output.append("{\\u1}");
                }
                else if (tagname == "s")
                {
                    subtitle_output.append("{\\s1}");
                }
                else if (tagname == "font")
                {
                    std::string attribute_name;
                    std::string attribute_value;

                    attribute_name = ConsumeAttribute(&psz_subtitle, attribute_value);
                    while (!attribute_name.empty())
                    {
                        // Convert attribute_name to lowercase
                        std::transform(attribute_name.begin(), attribute_name.end(), attribute_name.begin(), ::tolower);
                        if (attribute_name == "face")
                        {
                            subtitle_output.append("{\\fn" + attribute_value + "}");
                        }
                        else if (attribute_name == "family")
                        {
                        }
                        if (attribute_name == "size")
                        {
                            double resy = settings.SrtResY / 288.0;
                            int font_size = (int)std::round(std::stod(attribute_value) * resy);
                            subtitle_output.append("{\\fs" + std::to_string(font_size) + "}");
                        }
                        else if (attribute_name == "color")
                        {
                            MatchColorSrt(attribute_value);

                            // If color is invalid, use WHITE
                            if ((strtoul(attribute_value.c_str(), NULL, 16) == 0) && (attribute_value != "000000"))
                                attribute_value.assign("FFFFFF");

                            // HTML is RGB and we need BGR for libass
                            swapRGBtoBGR(attribute_value);
                            subtitle_output.append("{\\c&H" + attribute_value + "&}");
                        }
                        attribute_name = ConsumeAttribute(&psz_subtitle, attribute_value);
                    }
                }
                else
                {
                    // This is an unknown tag. We need to hide it if it's properly closed, and display it otherwise
                    if (!IsClosed(psz_subtitle, tagname.c_str()))
                    {
                        //subtitle_output.append("<" + tagname + ">");
                    }
                    else
                    {
                    }
                    // In any case, fall through and skip to the closing tag.
                }
                // Skip potential spaces & end tag
                while (*psz_subtitle && *psz_subtitle != '>')
                    psz_subtitle++;
                if (*psz_subtitle == '>')
                    psz_subtitle++;

            }
            else if (!strncmp(psz_subtitle, "</", 2))
            {
                std::string tagname = GetTag(&psz_subtitle, true);
                if (!tagname.empty())
                {
                    std::transform(tagname.begin(), tagname.end(), tagname.begin(), ::tolower);
                    if (tagname == "b")
                    {
                        subtitle_output.append("{\\b0}");
                    }
                    else if (tagname == "i")
                    {
                        subtitle_output.append("{\\i0}");
                    }
                    else if (tagname == "u")
                    {
                        subtitle_output.append("{\\u0}");
                    }
                    else if (tagname == "s")
                    {
                        subtitle_output.append("{\\s0}");
                    }
                    else if (tagname == "font")
                    {
                        double resy = settings.SrtResY / 288.0;
                        int font_size = (int)std::round(settings.FontSize * resy);
                        subtitle_output.append("{\\c}");
                        subtitle_output.append("{\\fn" + ws2s(settings.FontName) + "}");
                        subtitle_output.append("{\\fs" + std::to_string(font_size) + "}");
                    }
                    else
                    {
                        // Unknown closing tag. If it is closing an unknown tag, ignore it. Otherwise, display it
                        //subtitle_output.append("</" + tagname + ">");
                    }
                    while (*psz_subtitle == ' ')
                        psz_subtitle++;
                    if (*psz_subtitle == '>')
                        psz_subtitle++;
                }
            }
            else
            {
                /* We have an unknown tag, just append it, and move on.
                * The rest of the string won't be recognized as a tag, and
                * we will ignore unknown closing tag
                */
                subtitle_output.push_back('<');
                psz_subtitle++;
            }
        }
        /* MicroDVD extensions */
        /* FIXME:
        *  - Currently, we don't do difference between X and x, and we should:
        *    Capital Letters applies to the whole text and not one line
        *  - We don't support Position and Coordinates
        *  - We don't support the DEFAULT flag (HEADER)
        */
        else if (psz_subtitle[0] == '{' && psz_subtitle[2] == ':' && strchr(&psz_subtitle[2], '}'))
        {
            const char *psz_tag_end = strchr(&psz_subtitle[2], '}');
            size_t i_len = psz_tag_end - &psz_subtitle[3];

            if (psz_subtitle[1] == 'Y' || psz_subtitle[1] == 'y')
            {
                if (psz_subtitle[3] == 'i')
                {
                    subtitle_output.append("{\\i1}");
                    psz_subtitle++;
                }
                if (psz_subtitle[3] == 'b')
                {
                    subtitle_output.append("{\\b1}");
                    psz_subtitle++;
                }
                if (psz_subtitle[3] == 'u')
                {
                    subtitle_output.append("{\\u1}");
                    psz_subtitle++;
                }
            }
            else if ((psz_subtitle[1] == 'C' || psz_subtitle[1] == 'c')
                && psz_subtitle[3] == '$' && i_len >= 7)
            {
                /* Yes, they use BBGGRR */
                char psz_color[7];
                psz_color[0] = psz_subtitle[4]; psz_color[1] = psz_subtitle[5];
                psz_color[2] = psz_subtitle[6]; psz_color[3] = psz_subtitle[7];
                psz_color[4] = psz_subtitle[8]; psz_color[5] = psz_subtitle[9];
                psz_color[6] = '\0';
                subtitle_output.append("{\\c&H").append(psz_color).append("&}");
            }
            else if (psz_subtitle[1] == 'F' || psz_subtitle[1] == 'f')
            {
                std::string font_name(&psz_subtitle[3], i_len);
                subtitle_output.append("{\\fn" + font_name + "}");
            }
            else if (psz_subtitle[1] == 'S' || psz_subtitle[1] == 's')
            {
                int size = atoi(&psz_subtitle[3]);
                if (size)
                {
                    double resy = settings.SrtResY / 288.0;
                    int font_size = (int)std::round(size * resy);
                    subtitle_output.append("{\\fs" + std::to_string(font_size) + "}");
                }
            }
            // Hide other {x:y} atrocities, notably {o:x}
            psz_subtitle = psz_tag_end + 1;
        }
        else
        {
            if (*psz_subtitle == '\n' || !_strnicmp(psz_subtitle, "\\n", 2))
            {
                subtitle_output.append("\\N");

                if (*psz_subtitle == '\n')
                    psz_subtitle++;
                else
                    psz_subtitle += 2;
            }
            else if (*psz_subtitle == '\r')
            {
                psz_subtitle++;
            }
            else
            {
                subtitle_output.push_back(*psz_subtitle);
                psz_subtitle++;
            }
        }
    }
    srtLine.assign(subtitle_output);
}

// Match color name to its hex counterpart
void MatchColorSrt(std::string& fntColor)
{
    for (auto i = 0; i < _countof(color_tag); ++i)
    {
        if (strcmp(fntColor.c_str(), color_tag[i].color) == 0)
        {
            fntColor.assign(color_tag[i].hex);
            break;
        }
    }
}

std::string MatchLanguage(std::string& langCode, bool isCode2Chars)
{
    for (int i = 0; i < _countof(iso639_lang); i++)
    {
        if (strcmp(langCode.c_str(), isCode2Chars ? iso639_lang[i].lang2 : iso639_lang[i].lang3) == 0)
            return std::string(iso639_lang[i].language);

        // Language unknown
        if (i == _countof(iso639_lang) - 1)
            return std::string("Unknown");
    }

    return std::string("ERROR!!!");
}
