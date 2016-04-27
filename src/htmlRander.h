#ifndef HTMLRANDER_H_
#define HTMLRANDER_H_

#include "HtmlParserSTL.h"

namespace htmlparser {

    class AbstractRanderer;
    class HtmlComment;
    class HtmlElement;
    class HtmlEmptyElement;
    class HtmlText;
    class HtmlDocument;

    class Randerable {
    public:
        virtual void randerBy(const AbstractRanderer& rander);
        virtual ~Randerable() {};
    };

    class AbstractRanderer {
    public:
        virtual void rander(Randerable&) const = 0;
        virtual ~AbstractRanderer() {};
    };

    class HtmlRanderer : public AbstractRanderer {
    public:
        virtual void rander(Randerable&) const override final;

        virtual void rander(const HtmlComment&) const = 0;
        virtual void rander(const HtmlEmptyElement&) const = 0;
        virtual void rander(const HtmlElement&) const = 0;
        virtual void rander(const HtmlText&) const = 0;
        virtual void rander(const HtmlDocument&) const = 0;

        virtual void draw() const = 0;

        virtual ~HtmlRanderer() {};
    };


    class HtmlCanvasRanderer : public HtmlRanderer {
    public:
        virtual void rander(const HtmlComment&) const override;
        virtual void rander(const HtmlEmptyElement&) const override;
        virtual void rander(const HtmlElement&) const override;
        virtual void rander(const HtmlText&) const override;
        virtual void rander(const HtmlDocument&) const override;

        virtual void draw() const override;
    };

    class Indenter {
    public:
        typedef vector<string>  Vec;
    private:
        string_t _initIndent = "";
        string_t _indentStyle = "    "; //默认四个空格
        Vec      _indentStack;
        size_t   _index = 0;
    public:
        Indenter() {
            _indentStack.push_back(_initIndent);
        }
        Indenter(const string_t& style) : _initIndent(style) {
            _indentStack.push_back(_initIndent); }
        void setStyle(const string_t& style) { _indentStyle = style; }
        string_t getIndent() const {
            assert(_index >= 0);
            return _indentStack[_index];
        }
        void indentIncrease();
        void indentDecrease();
    };

    class HtmlTextRanderer : public HtmlRanderer {
    private:
        mutable string_t _buff = "";
        mutable Indenter _indenter;
    public:
        virtual void rander(const HtmlComment&) const override;
        virtual void rander(const HtmlEmptyElement&) const override;
        virtual void rander(const HtmlElement&) const override;
        virtual void rander(const HtmlText&) const override;
        virtual void rander(const HtmlDocument&) const override;

        virtual void draw() const override;

    private:
        void randerTagBegin(const HtmlElement&) const;
    };

}

#endif
