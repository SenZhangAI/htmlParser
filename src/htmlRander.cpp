#include "htmlParserSTL.h"
#include "htmlRander.h"
#include "htmlDOM.h"
#include "htmlio.h"

namespace htmlparser {

    void Randerable::randerBy(const AbstractRanderer& randerer) {
        randerer.rander(*this);
    }

    //Multiple dispatch
    //@see https://en.wikipedia.org/wiki/Multiple_dispatch#C.2B.2B
    void HtmlRanderer::rander(Randerable& randerable) const {
        if (auto p = dynamic_cast<HtmlText*>(&randerable)) {
            rander(*p);
        }
        else if (auto p = dynamic_cast<HtmlComment*>(&randerable)) {
            rander(*p);
        }
        else if (auto p = dynamic_cast<HtmlInlineElement*>(&randerable)) {
            rander(*p);
        }
        else if (auto p = dynamic_cast<HtmlElement*>(&randerable)) {
            rander(*p);
        }
        else if (auto p = dynamic_cast<HtmlDocument*>(&randerable)) {
            rander(*p);
        }
        else if (auto p = dynamic_cast<HtmlDocType*>(&randerable)) {
            rander(*p);
        }
        else {
            std::cout << "HtmlRanderer: hit no target" << std::endl;
        }
    }

    void HtmlRanderer::rander(const HtmlDocument& document) const {
        if (document.hasSon()) {
            for (shared_ptr<HtmlObject> s : (*(document._sons))) {
                s -> randerBy(*this);
            }
        }
    }

///////////////////////////////////////////////////////////////////////////////
// HtmlCanvasRanderer
    HtmlCanvasRanderer::HtmlCanvasRanderer() {
        _boxFmt = htmlio::ReadAnsiFile(T("./_template/_box.html"));
        _arrowPathFmt = htmlio::ReadAnsiFile(T("./_template/_arrowPath.html"));
    }

    string_t HtmlCanvasRanderer::_drawBox(int width, int height, int x, int y, const string_t& elementType,
                                          const string_t& tagname) const {

        stringstream_t buf;
        htmlio::Sprintf(buf, _boxFmt, 0, width, height, x * 2 * width, y * height, elementType, width - 4, height - 10,
                        tagname);
        return buf.str();
    }

    string_t HtmlCanvasRanderer::_drawArrowPath(int width, int height, int xFrom, int yFrom, int xTo, int yTo) const {
        stringstream_t buf;
        int x1 = width * (2 * xFrom + 1);
        int y1 = yFrom * height + height / 2;
        int x2 = x1 + (xTo - xFrom) * width / 2;
        int y2 = y1;
        int x3 = x2;
        int y3 = y1 + (yTo - yFrom) * height;
        int x4 = x1 + (xTo - xFrom) * width - 8;
        int y4 = y3;

        htmlio::Sprintf(buf, _arrowPathFmt, 0, x1, y1, x2, y2, x3, y3, x4, y4);
        return buf.str();
    }

    void HtmlCanvasRanderer::rander(const HtmlComment&) const {
        //do nothing
    }

    void HtmlCanvasRanderer::rander(const HtmlInlineElement& e) const {
        _buff += _drawBox(_WIDTH, _HEIGHT, _x, _y, "inlineElement", e.getTag());
    }

    void HtmlCanvasRanderer::rander(const HtmlElement& element) const {
        int yTmp = _y;


        if (element.hasSon()) {
            _x++;

            for (shared_ptr<HtmlObject> s : * (element._sons)) {
                if (dynamic_cast<HtmlElement*>(&(*s))) {
                    _buff += _drawArrowPath(_WIDTH, _HEIGHT, _x - 1, yTmp, _x, _y);
                    s -> randerBy(*this);

                    _y++;

                }
            }

            _x--;
        }
        _buff += _drawBox(_WIDTH, _HEIGHT, _x, yTmp, "element", element.getTag());

    }

    void HtmlCanvasRanderer::rander(const HtmlText&) const {
        //do nothing
    }

    void HtmlCanvasRanderer::rander(const HtmlDocument& document) const {
        HtmlRanderer::rander(document);
    }
    void HtmlCanvasRanderer::rander(const HtmlDocType&) const {
        //do nothing
    }
    void HtmlCanvasRanderer::draw() const {
        std::cout << _buff << std::endl;
    };

///////////////////////////////////////////////////////////////////////////////
// Indenter
    void Indenter::indentIncrease() {
        _index++;

        if (_index == _indentStack.size()) {
            string_t temp = _indentStack[_index - 1] + _indentStyle;
            _indentStack.push_back(temp);
        }
    }
    void Indenter::indentDecrease() {
        _index--;

        if (_index < 0)
            _index = 0;
    }
///////////////////////////////////////////////////////////////////////////////
// HtmlTextRanderer

    void HtmlTextRanderer::rander(const HtmlComment& comment) const {
        _buff += _indenter.getIndent() + comment.getComment();
    }

    void HtmlTextRanderer::randerTagBegin(const HtmlElement& element) const {
        _buff += _indenter.getIndent() + T("<") + element.getTag();

        HtmlElement::Attributes attr = element.getAttributes();

        for (auto iter = attr.begin(); iter != attr.end(); ++iter) {
            _buff += T(" ") + iter-> first + T(" = \"") + iter -> second + T("\"");
        }

        _buff += T(">");
    }

    void HtmlTextRanderer::rander(const HtmlInlineElement& element) const {
        randerTagBegin(element);
    }

    void HtmlTextRanderer::rander(const HtmlElement& element) const {
        randerTagBegin(element);

        // 此部分放在htmlObject对象中或许更好
        // 一方面可强制检查nullptr，另一方面可以复用
        // 这里是为了代码更为清晰
        if (element.hasSon()) {
            _indenter.indentIncrease();

            for (shared_ptr<HtmlObject> s : * (element._sons)) {
                s -> randerBy(*this);
            }

            _indenter.indentDecrease();
        }

        _buff += _indenter.getIndent() + T("</") + element.getTag() + T(">");
    }

    void HtmlTextRanderer::rander(const HtmlText& text) const {
        _buff += _indenter.getIndent() + text.getText();
    }

    void HtmlTextRanderer::rander(const HtmlDocument& document) const {
        HtmlRanderer::rander(document);
    }

    void HtmlTextRanderer::rander(const HtmlDocType& doctype) const {
        _buff += T("<!DOCTYPE ") + doctype.getValue() + T(">");
    }

    void HtmlTextRanderer::draw() const {
        std::cout << _buff << std::endl;
    };
}
