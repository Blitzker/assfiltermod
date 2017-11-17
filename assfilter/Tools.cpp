/*
 *   Copyright(C) 2016-2017 Blitzker
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

void tokenize(const std::wstring& str, std::vector<std::wstring>& tokens, const std::wstring& delimiters, bool trimEmpty)
{
    std::wstring::size_type pos, lastPos = 0, length = str.length();

    using value_type = typename std::vector<std::wstring>::value_type;
    using size_type = typename std::vector<std::wstring>::size_type;

    while (lastPos < length + 1)
    {
        pos = str.find_first_of(delimiters, lastPos);
        if (pos == std::wstring::npos)
            pos = length;

        if (pos != lastPos || !trimEmpty)
            tokens.push_back(value_type(str.data() + lastPos, (size_type)pos - lastPos));

        lastPos = pos + 1;
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

void ConvertCPToUTF8(UINT CP, std::string& codepage_str)
{
    int size = MultiByteToWideChar(CP, MB_PRECOMPOSED, codepage_str.c_str(),
        (int)codepage_str.length(), nullptr, 0);

    std::wstring utf16_str(size, '\0');
    MultiByteToWideChar(CP, MB_PRECOMPOSED, codepage_str.c_str(),
        (int)codepage_str.length(), &utf16_str[0], size);

    int utf8_size = WideCharToMultiByte(CP_UTF8, 0, utf16_str.c_str(),
        (int)utf16_str.length(), nullptr, 0,
        nullptr, nullptr);

    std::string utf8_str(utf8_size, '\0');
    WideCharToMultiByte(CP_UTF8, 0, utf16_str.c_str(),
        (int)utf16_str.length(), &utf8_str[0], utf8_size,
        nullptr, nullptr);
    codepage_str.assign(utf8_str);
}

UINT GetLanguageCP(const std::wstring& langCode, bool isCode2Chars)
{
    for (auto i = 0; i < _countof(iso639_lang); ++i)
    {
        if (wcscmp(langCode.c_str(), isCode2Chars ? iso639_lang[i].lang2 : iso639_lang[i].lang3) == 0)
            return iso639_lang[i].codepage;

        // Language unknown
        if (i == _countof(iso639_lang) - 1)
            return 0;
    }

    return 0;
}

std::wstring ExtractYuvMatrix(const std::wstring& file)
{
    std::string lineIn;
    char inBuffer[BUFSIZ];
    std::ifstream f(file, std::ios::in);
    size_t line = 0;

    // Don't scan more than the first 30 lines
    while (!f.eof() && line < 30)
    {
        f.getline(inBuffer, sizeof(inBuffer) - 1);
        lineIn.assign(inBuffer);
        ++line;

        if (lineIn.empty())
            continue;

        if (lineIn.find("YCbCr Matrix:") == 0)
        {
            lineIn.erase(0, 13);
            trim(lineIn);
            break;
        }
        lineIn.clear();
    }
    if (lineIn.empty())
        lineIn.assign("None");

    return s2ws(lineIn);
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

std::wstring MatchLanguage(const std::wstring& langCode, bool isCode2Chars)
{
    for (auto i = 0; i < _countof(iso639_lang); ++i)
    {
        if (wcscmp(langCode.c_str(), isCode2Chars ? iso639_lang[i].lang2 : iso639_lang[i].lang3) == 0)
            return std::wstring(iso639_lang[i].language);

        // Language unknown
        if (i == _countof(iso639_lang) - 1)
            return std::wstring(L"Unknown");
    }

    return std::wstring(L"ERROR!!!");
}

ASS_Track* srt_read_file(ASS_Library* library, const std::wstring& fname, const AssFSettings& settings, const UINT codePage)
{
    // Convert SRT to ASS
    std::ifstream srtFile(fname, std::ios::in);
    std::string lineIn;
    std::string lineOut;
    char inBuffer[1024];
    char outBuffer[1024];
    int start[4], end[4];
    ASS_Track* track = ass_new_track(library);
    double resx = settings.SrtResX / 384.0;
    double resy = settings.SrtResY / 288.0;

    // Generate a standard ass header
    _snprintf_s(outBuffer, _TRUNCATE, "[Script Info]\n"
        "; Script generated by ParseSRT\n"
        "Title: ParseSRT generated file\n"
        "ScriptType: v4.00+\n"
        "WrapStyle: 0\n"
        "ScaledBorderAndShadow: %s\n"
        "YCbCr Matrix: TV.709\n"
        "PlayResX: %u\n"
        "PlayResY: %u\n"
        "[V4+ Styles]\n"
        "Format: Name, Fontname, Fontsize, PrimaryColour, SecondaryColour, OutlineColour, "
        "BackColour, Bold, Italic, Underline, StrikeOut, ScaleX, ScaleY, Spacing, Angle, "
        "BorderStyle, Outline, Shadow, Alignment, MarginL, MarginR, MarginV, Encoding\n"
        "Style: Default,%s,%u,&H%X,&H%X,&H%X,&H%X,0,0,0,0,%u,%u,%u,0,1,%u,%u,%u,%u,%u,%u,1"
        "\n\n[Events]\n"
        "Format: Layer, Start, End, Style, Name, MarginL, MarginR, MarginV, Effect, Text\n\n",
        settings.ScaledBorderAndShadow ? "yes" : "no", settings.SrtResX, settings.SrtResY,
        ws2s(settings.FontName).c_str(), (int)std::round(settings.FontSize * resy), settings.ColorPrimary,
        settings.ColorSecondary, settings.ColorOutline, settings.ColorShadow,
        settings.FontScaleX, settings.FontScaleY, settings.FontSpacing, settings.FontOutline,
        settings.FontShadow, settings.LineAlignment, (int)std::round(settings.MarginLeft * resx),
        (int)std::round(settings.MarginRight * resx), (int)std::round(settings.MarginVertical * resy));
    ass_process_data(track, outBuffer, static_cast<int>(strnlen_s(outBuffer, sizeof(outBuffer))));

    while (!srtFile.eof())
    {
        srtFile.getline(inBuffer, sizeof(inBuffer) - 1);
        lineIn.assign(inBuffer);
        if (lineIn.empty())
            continue;

        // Read the timecodes
        if (sscanf_s(inBuffer, "%d:%2d:%2d%*1[,.]%3d --> %d:%2d:%2d%*1[,.]%3d", &start[0], &start[1],
            &start[2], &start[3], &end[0], &end[1], &end[2], &end[3]) == 8)
        {
            lineOut.clear();
            srtFile.getline(inBuffer, sizeof(inBuffer) - 1);
            lineIn.assign(inBuffer);
            while (!lineIn.empty())
            {
                lineOut.append(lineIn);
                srtFile.getline(inBuffer, sizeof(inBuffer) - 1);
                lineIn.assign(inBuffer);
                if (!lineIn.empty())
                    lineOut.append("\\N");
            }
            //lineOut.append("\n");

            // Convert to UTF-8
            if (codePage != 0)
                ConvertCPToUTF8(codePage, lineOut);

            ParseSrtLine(lineOut, settings);

            _snprintf_s(outBuffer, _TRUNCATE, "Dialogue: 0,%d:%02d:%02d.%02d,%d:%02d:%02d.%02d,"
                "Default,,0,0,0,,{\\blur%u}%s%s",
                start[0], start[1], start[2],
                (int)floor((double)start[3] / 10.0), end[0], end[1],
                end[2], (int)floor((double)end[3] / 10.0), settings.FontBlur, ws2s(settings.CustomTags).c_str(), lineOut.c_str());
            ass_process_data(track, outBuffer, static_cast<int>(strnlen_s(outBuffer, sizeof(outBuffer))));
        }
    }
    return track;
}

std::wstring ParseFontsPath(std::wstring fontsDir, const std::wstring& name)
{
    if (fontsDir.empty())
        return std::wstring(L"");

    // Check to replace {FILE_DIR} tag only if its at the start of the string
    if (fontsDir.find(L"{FILE_DIR}") == 0)
    {
        size_t pos = name.find_last_of(L'\\');
        std::wstring temp(name.substr(0, pos + 1));
        FindReplace(fontsDir, std::wstring(L"{FILE_DIR}"), temp);
    }

    if (fontsDir.at(fontsDir.length() - 1) != L'\\')
        fontsDir.push_back(L'\\');

    if (!dirExists(fontsDir))
        return std::wstring(L"");

    return fontsDir;
}

std::vector<std::wstring> FindMatchingSubs(const std::wstring& fileName)
{
    std::vector<std::wstring> names;
    std::wstring search_path = fileName + L"*";
    WIN32_FIND_DATAW fd;
    HANDLE hFind = FindFirstFileW(search_path.c_str(), &fd);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                std::wstring fn(fd.cFileName);
                std::transform(fn.begin(), fn.end(), fn.begin(), ::towlower);
                if ((fn.find(L".ass", fn.find_last_of(L'.')) != std::wstring::npos) || (fn.find(L".srt", fn.find_last_of(L'.')) != std::wstring::npos))
                {
                    std::wstring fp(fileName.substr(0, fileName.find_last_of(L'\\') + 1));
                    fp.append(fd.cFileName);
                    names.push_back(fp);
                }
            }
        } while (FindNextFileW(hFind, &fd));
        FindClose(hFind);
    }
    return names;
}

std::vector<std::wstring> ListFontsInFolder(const std::wstring& folder)
{
    std::vector<std::wstring> names;
    std::wstring search_path = folder + L"\\*.*";
    WIN32_FIND_DATA fd;
    HANDLE hFind = FindFirstFileW(search_path.c_str(), &fd);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            // read all (real) files in current folder
            // , delete '!' read other 2 default folder . and ..
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                std::wstring fn(fd.cFileName);
                std::transform(fn.begin(), fn.end(), fn.begin(), ::towlower);
                if ((fn.find(L".otf", fn.find_last_of(L'.')) != std::wstring::npos) || (fn.find(L".ttf", fn.find_last_of(L'.')) != std::wstring::npos))
                {
                    std::wstring tmp = folder;
                    tmp.append(fd.cFileName);
                    names.push_back(tmp);
                }
            }
        } while (FindNextFileW(hFind, &fd));
        FindClose(hFind);
    }
    return names;
}
