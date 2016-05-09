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

    //生成Element的父子关系结构的SVG图片
    class HtmlSVGRanderer : public HtmlRanderer {
    private:
        const static int _WIDTH = 120;
        const static int _HEIGHT = 50;

        string_t _boxFmt;
        string_t _arrowPathFmt;

        mutable string_t _buff = T("");
        mutable int _x = 0; // row
        mutable int _y = 0; // column

        //x 与 y 控制grid的位置，可理解为行，列，取值 0...n
        //而实际尺寸由width和height控制
        string_t _drawBox(int width, int height, int x, int y, const string_t& elementType, const string_t& tagname) const;
        string_t _drawArrowPath(int width, int height, int xFrom, int yFrom, int xTo, int yTo) const;
    public:
        HtmlSVGRanderer();

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
        Indenter(const string_t& init, const string_t& style) : _initIndent(init), _indentStyle(style) {
            _indentStack.push_back(_initIndent);
        }

        string_t getIndent() const {
            ASSERT(_index >= 0);
            return _indentStack[_index];
        }
        void indentIncrease();
        void indentDecrease();
    };

    //生成对应的Html源码的文本文件
    //例如可用于代码格式化和纠错
    class HtmlTextRanderer : public HtmlRanderer {
    private:
        mutable string_t _buff = T("");
        mutable Indenter _indenter;
    public:
        HtmlTextRanderer() { };
        HtmlTextRanderer(const string_t& initIndent, const string_t& indentStyle) :
            _indenter(initIndent, indentStyle) { };

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

    //生成Html内置的code代码并高亮
    class HtmlCodeRanderer : public HtmlRanderer {
    private:
        mutable string_t _buff = T("");
        mutable Indenter _indenter;
    public:
        HtmlCodeRanderer() : _indenter(T("\n"), T("")) { };
        HtmlCodeRanderer(const string_t& initIndent, const string_t& indentStyle) :
            _indenter(initIndent, indentStyle) { };

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
