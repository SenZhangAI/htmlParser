#include "htmlDOM.h"
#include "htmlParserSTL.h"
#include "htmlRander.h"
#include "HtmlLexer.h"
#include "htmlio.h"

using namespace htmlparser;

int main(int argc, char const* argv[]) {

    if (argc < 2)
        std::cout << "Usage: use Filename as argument" << std::endl;

    string_t file = htmlio::ReadAnsiFile(argv[1]);

    //test {
    HtmlLexer lex;
    shared_ptr<HtmlDocument> document = lex.parse(file);

    //HtmlTextRanderer textRander;
    //document->randerBy(textRander);
    //textRander.draw();

    //HtmlTextRanderer textRander2(T("\n"),T("--->"));
    //document->randerBy(textRander2);
    //textRander2.draw();

    HtmlTextRanderer textRander3(T("\n"), T("  "));
    document->randerBy(textRander3);
    textRander3.draw();

    //HtmlTextRanderer textRander4(T(""),T(""));
    //document->randerBy(textRander4);
    //textRander4.draw();

    HtmlSVGRanderer canvasRander;
    document -> randerBy(canvasRander);
    canvasRander.draw();

    HtmlCodeRanderer codeRander;
    document -> randerBy(codeRander);
    codeRander.draw();

    //}

    return 0;
}
