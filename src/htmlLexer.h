#ifndef HTMLLEXER_H_
#define HTMLLEXER_H_

#include "htmlParserSTL.h"

namespace htmlparser {

    class HtmlDocument;

    class AbstractLexer {
    public:
        virtual shared_ptr<HtmlDocument> parse(const string_t& text) const = 0;
        virtual ~AbstractLexer() {};
    };

    class HtmlLexer : public AbstractLexer {
    public:
        virtual shared_ptr<HtmlDocument> parse(const string_t& html) const override;
        virtual ~HtmlLexer() {};
    };

}

#endif
