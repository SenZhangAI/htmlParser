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


    class TagStack {
    private:
        vector<string_t> _stack;
        size_t _sz  = 0;
    public:
        size_t size() { return _sz; };
        size_t find(const string_t& tag);
        void push(const string_t& tag);
        size_t pop(size_t i);
    };

    class HtmlLexer : public AbstractLexer {
    public:
        virtual shared_ptr<HtmlDocument> parse(const string_t& html) const override;
        virtual ~HtmlLexer() {};
    };

}

#endif
