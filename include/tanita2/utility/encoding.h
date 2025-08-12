/*
 Движок "Танита2".
 Преобразования из wchar_t в char и обратно.
 */
#pragma once
#ifndef _TANITA2_UTILITY_ENCODING_H_
#define _TANITA2_UTILITY_ENCODING_H_

namespace utility
{

// Преобразование std::wstring в std::string. 
inline std::string wstring2string( const std::wstring & str, std::locale loc = std::locale("") )
{
    size_t len = str.length();
    if (0 == len)
        return "";
    std::string result(len, 0);
    std::use_facet<std::ctype<wchar_t> >(loc).narrow(str.c_str(), str.c_str() + len, '?', &*result.begin());
    return result;
}

} // namespace utility

#endif // _TANITA2_UTILITY_ENCODING_H_
