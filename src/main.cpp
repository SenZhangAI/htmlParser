#include "htmlDOM.h"
#include "htmlParserSTL.h"
#include "htmlRander.h"
#include "HtmlLexer.h"

using namespace htmlparser;

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

void WriteAnsiFile(string_t fileName, stringstream_t& ss) {
#ifdef _UNICODE_TINYMOE
    string buffer = wtoa(ss.str());
#else
    string buffer = ss.str();
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

#ifdef _UNICODE_TINYMOE
    return atow(buffer);
#else
    return buffer;
#endif
}

int main(int argc, char const* argv[]) {

    if (argc < 2)
        std::cout << "Usage: use Filename as argument" << std::endl;

    string_t file = ReadAnsiFile(argv[1]);

    //test {
        HtmlLexer lex;
        shared_ptr<HtmlDocument> document = lex.parse(file);

        //HtmlCanvasRanderer canvasRander;
        //HtmlTextRanderer textRander;
        //document->randerBy(textRander);
        //textRander.draw();

        HtmlTextRanderer textRander2(T("\n"),T("--->"));
        document->randerBy(textRander2);
        textRander2.draw();
    //}

    return 0;
}
