#include "htmlDOM.h"
#include "HtmlLexer.h"

namespace htmlparser {


    bool HtmlObject::hasSon() const {
        return _sons != nullptr && (_sons->size() != 0) ;
    }
    HtmlObject* HtmlObject::setParent(HtmlObject* parent) {
        HtmlObject* old = _parent;
        _parent = parent;
        return old;
    }

    void HtmlObject::addSon(const Ptr& son) {
        if (_sons == nullptr) {
            _sons = make_shared<Vec>();
        }

        _sons->push_back(son);
        son->setParent(this);
    }

    HtmlObject_No_Sons::~HtmlObject_No_Sons() {};

    void HtmlElement::addAttribute(string_t attribute, string_t value) {
        _attributes.insert(pair<string_t, string_t>(attribute, value));
    }

};
