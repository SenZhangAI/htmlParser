#ifndef HTMLRANDER_H_
#define HTMLRANDER_H_

#include "HtmlParserSTL.h"

namespace htmlparser {

    class AbstractRanderer;
    class HtmlComment;
    class HtmlElement;
    class HtmlInlineElement;
    class HtmlText;
    class HtmlDocument;
    class HtmlDocType;

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
        virtual void rander(const HtmlInlineElement&) const = 0;
        virtual void rander(const HtmlElement&) const = 0;
        virtual void rander(const HtmlText&) const = 0;
        virtual void rander(const HtmlDocument&) const = 0;
        virtual void rander(const HtmlDocType&) const = 0;

        virtual void draw() const = 0;

        virtual ~HtmlRanderer() {};
    };


    class HtmlCanvasRanderer : public HtmlRanderer {
    public:
        virtual void rander(const HtmlComment&) const override;
        virtual void rander(const HtmlInlineElement&) const override;
        virtual void rander(const HtmlElement&) const override;
        virtual void rander(const HtmlText&) const override;
        virtual void rander(const HtmlDocument&) const override;
        virtual void rander(const HtmlDocType&) const override;

        virtual void draw() const override;
    };

    class Indenter {
    public:
        typedef vector<string>  Vec;
    private:
        string_t _initIndent = T("\n");
        string_t _indentStyle = T("    "); //默认四个空格
        Vec      _indentStack;
        size_t   _index = 0;
    public:
        Indenter() {
            _indentStack.push_back(_initIndent);
        }
        Indenter(const string_t& initIndent) : _initIndent(initIndent) {
            _indentStack.push_back(_initIndent);
        }
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
        mutable string_t _buff = T("");
        mutable Indenter _indenter;
    public:
        HtmlTextRanderer() {
        }
        HtmlTextRanderer(const string_t& initIndent, const string_t& indentStyle) :
            _indenter(initIndent) {
            _indenter.setStyle(indentStyle);
        }

        virtual void rander(const HtmlComment&) const override;
        virtual void rander(const HtmlInlineElement&) const override;
        virtual void rander(const HtmlElement&) const override;
        virtual void rander(const HtmlText&) const override;
        virtual void rander(const HtmlDocument&) const override;
        virtual void rander(const HtmlDocType&) const override;

        virtual void draw() const override;

    private:
        void randerTagBegin(const HtmlElement&) const;
    };

}

#endif
