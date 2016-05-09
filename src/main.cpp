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

    HtmlCanvasRanderer canvasRander;
    //HtmlTextRanderer textRander;
    //document->randerBy(textRander);
    //textRander.draw();

    document -> randerBy(canvasRander);
    canvasRander.draw();

    //HtmlTextRanderer textRander2(T("\n"),T("--->"));
    //document->randerBy(textRander2);
    //textRander2.draw();
    //}

    return 0;
}
