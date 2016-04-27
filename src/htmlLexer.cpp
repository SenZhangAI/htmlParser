#include "htmlLexer.h"
#include "htmlDOM.h"

namespace htmlparser {

    shared_ptr<HtmlDocument> HtmlLexer::parse(const string_t& html) const {
        //test
        {
            cout << "Parsing...\n\n" << endl;

            shared_ptr<HtmlDocument> document = make_shared<HtmlDocument>();

            shared_ptr<HtmlText> text = make_shared<HtmlText>("This is Text");
            document->addSon(text);

            shared_ptr<HtmlElement> element = make_shared<HtmlElement>("This-is-Element-Tag");
            element->addAttribute("attribute1", "value1");
            element->addAttribute("attribute2", "value2");

            text = make_shared<HtmlText>("This is text in Element");
            element->addSon(text);

            shared_ptr<HtmlComment> comment = make_shared<HtmlComment>("This is Text");
            element->addSon(text);

            shared_ptr<HtmlEmptyElement> empty_element = make_shared<HtmlEmptyElement>("this-is-EmptyElement-Tag");
            element->addAttribute("attribute3", "value3");
            element->addAttribute("attribute4", "value4");

            element->addSon(empty_element);

            document->addSon(element);
            document->addSon(element);

            return document;
        }

    }

}
