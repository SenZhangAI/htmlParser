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
        else if (auto p = dynamic_cast<HtmlEmptyElement*>(&randerable)) {
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

    void HtmlCanvasRanderer::rander(const HtmlEmptyElement&) const {
        std::cout << "rander HtmlEmptyElement by HtmlCanvasRanderer" << std::endl;
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
// HtmlTextRanderer

    void HtmlTextRanderer::rander(const HtmlComment& comment) const {
        buff += "<--" + comment.getComment();
        buff += "-->\n";
    }

    void HtmlTextRanderer::randerTagBegin(const HtmlElement& element) const {
        buff += "<" + element.getTag();

        HtmlElement::Attributes attr = element.getAttributes();

        for (auto iter = attr.begin(); iter != attr.end(); ++iter) {
            buff += " " + iter-> first + " = \"" + iter -> second + "\"";
        }

        buff += ">\n";
    }

    void HtmlTextRanderer::rander(const HtmlEmptyElement& element) const {
        randerTagBegin(element);
    }

    void HtmlTextRanderer::rander(const HtmlElement& element) const {
        randerTagBegin(element);

        // 此部分放在htmlObject对象中或许更好
        // 一方面可强制检查nullptr，另一方面可以复用
        // 这里是为了代码更为清晰
        if (element.hasSon()) {
            for (shared_ptr<HtmlObject> s : * (element._sons)) {
                s -> randerBy(*this);
            }
        }

        buff += "</" + element.getTag() + ">\n";
    }

    void HtmlTextRanderer::rander(const HtmlText& text) const {
        buff += text.getText() + "\n";
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
        std::cout << buff << std::endl;
    };
}
