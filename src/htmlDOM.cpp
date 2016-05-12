#include "htmlDOM.h"
#include "htmlLexer.h"

namespace htmlparser {

    bool HtmlObject::hasSon() const {
        return _sons != nullptr && (_sons->size() != 0) ;
    }

    HtmlObject::Ptr HtmlObject::setParent(Ptr parent) {
        Ptr old = _parent;
        _parent = parent;
        return old;
    }

    HtmlObject::Ptr HtmlObject::getParent() {
        return _parent;
    }

    void HtmlObject::addSon(const Ptr& son) {
        if (_sons == nullptr) {
            _sons = make_shared<Vec>();
        }

        _sons->push_back(son);
        son->setParent(this -> shared_from_this());
    }

    HtmlObject_No_Sons::~HtmlObject_No_Sons() {};

    void HtmlElement::addAttribute(string_t attribute, string_t value) {
        auto iter = _attributes.begin();

        while (iter != _attributes.end()) {
            if (iter -> first == attribute)
                break;

            ++iter;
        }

        if (iter != _attributes.end()) {
            iter -> second = value;
        }
        else {
            _attributes.push_back(pair<string_t, string_t>(attribute, value));
        }

    }

}
