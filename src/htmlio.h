#ifndef HTMLIO_H_
#define HTMLIO_H_

#include "HtmlParserSTL.h"

namespace htmlparser {

    namespace htmlio {

        wstring atow(const string& s);
        string wtoa(const wstring& s);

        void WriteAnsiFile(string_t fileName, stringstream_t& ss);
        string_t ReadAnsiFile(string_t fileName);

        inline void Sprintf(stringstream_t& s, const string_t& fmt, string_t::size_type pos) {
            s << fmt.substr(pos);
        };

        template<typename T, typename... Args>
        void Sprintf(stringstream_t& s, const string_t& fmt, string_t::size_type pos, T value, Args... args) {
            string_t::size_type next = fmt.find(T("{%%}"), pos);

            if (next != string_t::npos) {
                s << fmt.substr(pos, next - pos);
                s << value;
                Sprintf(s, fmt, next + 4, args...);
            }
            else {
                s << fmt.substr(pos);
            }

        };

    }

}

#endif
