#include "htmlParserSTL.h"
#include "htmlRander.h"
#include "htmlDOM.h"

namespace htmlparser {

    void Randerable::randerBy(const AbstractRanderer& randerer) {
        randerer.rander(*this);
    }

    //Multiple dispatch
    //@see https://en.wikipedia.org/wiki/Multiple_dispatch#C.2B.2B
    void HtmlRanderer::rander(Randerable& randerable) const {
        if (auto p = dynamic_cast<HtmlElement*>(&randerable)) {
            rander(*p);
        }
        else if (auto p = dynamic_cast<HtmlText*>(&randerable)) {
            rander(*p);
        }
        else if (auto p = dynamic_cast<HtmlComment*>(&randerable)) {
            rander(*p);
        }
        else if (auto p = dynamic_cast<HtmlInlineElement*>(&randerable)) {
            rander(*p);
        }
        else if (auto p = dynamic_cast<HtmlDocument*>(&randerable)) {
            rander(*p);
        }
        else {
            std::cout << "HtmlRanderer: hit no target" << std::endl;
        }
    }

///////////////////////////////////////////////////////////////////////////////
// HtmlCanvasRanderer

    void HtmlCanvasRanderer::rander(const HtmlComment&) const {
        std::cout << "rander HtmlComment by HtmlCanvasRanderer" << std::endl;
    }

    void HtmlCanvasRanderer::rander(const HtmlInlineElement&) const {
        std::cout << "rander HtmlInlineElement by HtmlCanvasRanderer" << std::endl;
    }

    void HtmlCanvasRanderer::rander(const HtmlElement&) const {
        std::cout << "rander HtmlElement by HtmlCanvasRanderer" << std::endl;
    }

    void HtmlCanvasRanderer::rander(const HtmlText&) const {
        std::cout << "rander HtmlText by HtmlCanvasRanderer" << std::endl;
    }

    void HtmlCanvasRanderer::rander(const HtmlDocument&) const {
        std::cout << "rander HtmlDocument by HtmlCanvasRanderer" << std::endl;
    }
    void HtmlCanvasRanderer::draw() const {
        std::cout << "draw" << std::endl;
    };

///////////////////////////////////////////////////////////////////////////////
// Indenter
    void Indenter::indentIncrease() {
        _index++;
        if(_index == _indentStack.size()) {
            string_t temp = _indentStack[_index - 1] + _indentStyle;
            _indentStack.push_back(temp);
        }
    }
    void Indenter::indentDecrease() {
        _index--;
        if(_index < 0)
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
        _buff += _indenter.getIndent() + text.getText() + T("");
    }

    void HtmlTextRanderer::rander(const HtmlDocument& document) const {

        // 此部分放在htmlObject对象中或许更好
        // 一方面可强制检查nullptr，另一方面可以复用
        // 放在这里是为了代码更为清晰
        if (document.hasSon()) {
            for (shared_ptr<HtmlObject> s : (*(document._sons))) {
                s -> randerBy(*this);
            }
        }
    }

    void HtmlTextRanderer::draw() const {
        std::cout << _buff << std::endl;
    };
}
