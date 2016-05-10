
#include "htmlio.h"

namespace htmlparser {

    namespace htmlio {

        wstring atow(const string& s) {
            wstring buffer;
            buffer.resize(s.size() + 1);
            mbstowcs(&buffer[0], &s[0], s.size());
            return buffer.c_str();
        }

        string wtoa(const wstring& s) {
            string buffer;
            buffer.resize(s.size() * 3 + 1);
            wcstombs(&buffer[0], &s[0], s.size());
            return buffer.c_str();
        }

        void WriteAnsiFile(string_t fileName, const string_t& ss) {
#ifdef _UNICODE_HTMLPARSER
            string buffer = wtoa(ss);
#else
            string buffer = ss;
#endif

            ofstream o(fileName, ios_base::binary);
            o.write(&buffer[0], buffer.size());
        }

        string_t ReadAnsiFile(string_t fileName) {
            ifstream i(fileName, ios_base::binary);
            i.seekg(0, ios_base::end);
            auto pos = i.tellg();
            i.seekg(0, ios_base::beg);

            string buffer;
            buffer.resize((size_t)pos);
            i.read(&buffer[0], pos);

#ifdef _UNICODE_HTMLPARSER
            return atow(buffer);
#else
            return buffer;
#endif
        }


    }

}
