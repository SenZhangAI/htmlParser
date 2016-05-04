#include "htmlParserSTL.h"
#include "htmlLexer.h"
#include "htmlDOM.h"

namespace htmlparser {

    size_t TagStack::find(const string_t& tag) {
        std::cout << "find: " << tag << std::endl;
        //find
        size_t s = size();

        while (s > 0) {
            if (_stack[s - 1] == tag)
                break;

            s--;
        }

        return s;
    }

    void TagStack::push(const string_t& tag) {
        std::cout << "push: " << tag << std::endl;
        std::cout << "Origin _sz:" << _sz << std::endl;

        if (_sz < _stack.size())
            _stack[_sz++] = tag;
        else {
            _stack.push_back(tag);
            _sz++;
        }

        std::cout << "Now _sz:" << _sz << std::endl;
        std::cout << "Stack:" << std::endl;
        for (size_t i = 0; i < _sz; ++i) {
            std::cout << _stack[i] << std::endl;
        }
    }

    size_t TagStack::pop(size_t i) {
        ASSERT(_sz > i);
        std::cout << "Origin _sz:" << _sz << std::endl;
        _sz -= i;
        std::cout << "Now _sz:" << _sz << std::endl;
        std::cout << "Stack:" << std::endl;

        for (size_t i = 0; i < _sz; ++i) {
            std::cout << _stack[i] << std::endl;
        }

        return _sz;
    }

    shared_ptr<HtmlDocument> HtmlLexer::parse(const string_t& html) const {

        enum class State {
            Begin,
            InComment,
            InElement,
            InAttributes,
            InCloseTag,
            InDocType,
            FinalState,
        };

        struct AssistFlag {
            bool inlineElement = false;
            bool foundDocType  = false;
            bool foundHtml     = false;
            bool foundHead     = false;
            bool foundBody     = false;
        };

        shared_ptr<HtmlDocument> document = make_shared<HtmlDocument>();
        shared_ptr<HtmlElement> currentElement = nullptr;
        shared_ptr<HtmlElement> nextElement = nullptr;
        shared_ptr<HtmlInlineElement> currentInlineElement = nullptr;

        State state = State::Begin;
        AssistFlag assist;
        TagStack tagStack;
        string_t::size_type tkStart, tkEnd, atLess, atGreat, nextLess, lastGreat;
        string_t::size_type textStart, textEnd;
        string_t::size_type attrStart, attrEnd, valueStart, valueEnd, equalSign, nextSign;
        string_t attrTmp, valueTmp, tagTmp;

        //根据tag，判断state
        auto _TagToken = [&](string::size_type length) {
            string_t _tag = html.substr(tkStart, length);
            transform(_tag.begin(), _tag.end(), _tag.begin(), ::tolower_t);
            std::cout << "_tag: " << _tag << std::endl;


            assist.inlineElement = false;
            assist.inlineElement =
                _tag == T("br")       ? true :
                _tag == T("img")      ? true :
                _tag == T("meta")     ? true :
                _tag == T("link")     ? true :
                false;

            if (_tag == T("!doctype")) {
                std::cout << "found doctype" << std::endl;
                assist.foundDocType = true;
                state = State::InDocType;
                return;
            }

            if (_tag == T("html")) {
                std::cout << "found html" << std::endl;
                assist.foundHtml = true;
            }

            if (_tag == T("head")) {
                std::cout << "found head" << std::endl;
                assist.foundHead = true;
            }

            if (_tag == T("body")) {
                std::cout << "found body" << std::endl;
                assist.foundBody = true;
            }

            state = State::InElement;
            return;
        };

        auto _CloseTag = [&](string::size_type length) {
            string_t _closeTag = html.substr(tkStart, length);
            transform(_closeTag.begin(), _closeTag.end(), _closeTag.begin(), ::tolower_t);

            size_t pos = tagStack.find(_closeTag);
            size_t num = tagStack.size() - pos + 1;

            if (pos < 0) {
                currentElement -> addSon(make_shared<HtmlElement>(_closeTag));
            }
            else {
                while (num > 0) {
                    tagStack.pop(1);
                    //HtmlObject* 转 HtmlElement*
                    currentElement = static_pointer_cast<HtmlElement>(currentElement->getParent());
                    //currentElement.reset((HtmlElement*)(currentElement->getParent().get()));
                    std::cout << "Now currentElement: "<< currentElement -> getTag() << std::endl;
                    num--;
                }
            }
        };

        nextLess = html.find(T('<'));
        atGreat = -1;

        while (nextLess != string_t::npos && atLess != string_t::npos) {
            switch (state) {
            case State::Begin:
                std::cout << "in begin" << std::endl;

                if (atGreat == html.size() - 1)
                    return document;

                atLess = nextLess;
                nextLess = html.find(T('<'), atLess + 1);
                lastGreat = atGreat;
                atGreat = html.find(T('>'), atGreat + 1);

                if (atLess == string_t::npos || atGreat == string_t::npos) {
                    state = State::FinalState;
                    break;
                }

                while (atGreat < atLess) {
                    atGreat = html.find(T('>'), atGreat + 1);
                }

                while (nextLess != string_t::npos && nextLess < atGreat) {
                    atLess = nextLess;
                    nextLess = html.find(T('<'), atLess + 1);
                }

                //以上确保前置条件：
                //   lastGreat, atLess, atGreat, nextLess的相对位置关系为：
                //     lastGreat < atLess < atGreat < nextLess(or npos)
                std::cout << lastGreat << std::endl;
                std::cout << atLess << std::endl;
                std::cout << atGreat << std::endl;
                std::cout << nextLess << std::endl;
                //InText
                if (atLess > lastGreat + 1) {
                    textStart = html.find_first_not_of(T(" \t\n"), lastGreat + 1);
                    textEnd = html.find_last_not_of(T(" \t\n"), atLess - 1);
                    std::cout << "textStart = " << textStart << std::endl;
                    std::cout << "textEnd = " << textEnd << std::endl;

                    if (textEnd >= textStart) {
                        if (tagStack.size() == 0) {
                            //text必须在Element内，在document内显然不合法
                        }
                        else {
                            currentElement -> addSon(make_shared<HtmlText>(
                                                         html.substr(textStart, textEnd + 1 - textStart)));
                        }
                    }
                }

                tkStart = html.find_first_not_of(T(" \t\n"), atLess + 1);

                //空字串<  >
                if (tkStart == atGreat) {
                    break;
                    //back to Begin
                }

                if (html[tkStart] == '/') {
                    state = State::InCloseTag;
                    break;
                }

                if (html[tkStart] == T('!') &&
                        html[tkStart + 1] == T('-') &&
                        html[tkStart + 2] == T('-')) {
                    state = State::InComment;
                    break;
                }

                tkEnd = html.find_first_of(T(" \t\n>"), tkStart);
                _TagToken(tkEnd - tkStart);
                break;

            case State::InComment:
                std::cout << "in comment" << std::endl;

                //暂时不考虑 -- \n\t> 的情况
                //因comment让>失效，重新设置atGreat
                atGreat = html.find(T("-->"), tkStart + 2);

                if (atGreat != string_t::npos)
                    atGreat += 2;
                else
                    atGreat = html.size() - 1;

                if (tagStack.size() == 0) {
                    document -> addSon(make_shared<HtmlComment>(
                                           html.substr(atLess, atGreat + 1 - atLess)));
                }
                else {
                    currentElement -> addSon(make_shared<HtmlComment>(
                                                 html.substr(atLess, atGreat + 1 - atLess)));
                }

                state = State::Begin;
                break;

            case State::InElement:
                std::cout << "in element" << std::endl;
                tagTmp = html.substr(tkStart, tkEnd - tkStart);

                if (assist.inlineElement == true) {
                    currentInlineElement = make_shared<HtmlInlineElement>(tagTmp);

                    if (tagStack.size() == 0) {
                        document -> addSon(currentInlineElement);
                    }
                    else {
                        currentElement -> addSon(currentInlineElement);
                    }
                }
                else {
                    nextElement = make_shared<HtmlElement>(tagTmp);

                    if (tagStack.size() == 0) {
                        document -> addSon(nextElement);
                    }
                    else {
                        currentElement -> addSon(nextElement);
                    }

                    currentElement = nextElement;
                    tagStack.push(tagTmp);
                }

                if (html[tkEnd] == T('>')) {
                    state = State::Begin;
                }
                else {
                    valueEnd = tkEnd;
                    state = State::InAttributes;
                }

                break;

            case State::InAttributes:
                std::cout << "in attribute" << std::endl;
                attrStart = html.find_first_not_of(T(" \t\n"), valueEnd + 1);

                //无属性
                if (html[attrStart] == '>') {
                    state = State::Begin;
                    break;
                }

                attrEnd = html.find_first_of(T(" \t\n=>"), attrStart);

                nextSign = html.find_first_not_of(T(" \t\n"), attrEnd);

                attrTmp = html.substr(attrStart, attrEnd - attrStart);

                //属性值为空
                // ... attriTemp>
                if (html[nextSign] == T('>')) {
                    if (assist.inlineElement == true) {
                        currentInlineElement->addAttribute(attrTmp, T(""));
                    }
                    else {
                        currentElement->addAttribute(attrTmp, T(""));
                    }

                    if (nextSign > atGreat)
                        atGreat = nextSign;

                    state = State::Begin;
                    break;
                }

                if (html[nextSign] != T('=')) {
                    if (assist.inlineElement == true) {
                        currentInlineElement->addAttribute(attrTmp, T(""));
                    }
                    else {
                        currentElement->addAttribute(attrTmp, T(""));
                    }

                    valueEnd = attrEnd;
                    break;
                }


                // attribute = ...
                valueStart = html.find_first_not_of(T(" \t\n"), nextSign + 1);

                if (html[valueStart] == T('\"')) {
                    valueStart++;
                    valueEnd = html.find(T('\"'), valueStart);
                }
                else {
                    valueEnd = html.find_first_of(T(" \n\t"), valueStart + 1);
                }

                valueTmp = html.substr(valueStart, valueEnd - valueStart);


                if (assist.inlineElement == true) {
                    currentInlineElement->addAttribute(attrTmp, valueTmp);
                }
                else {
                    currentElement->addAttribute(attrTmp, valueTmp);
                }

                break;

            case State::InCloseTag:
                std::cout << "in close tag" << std::endl;
                tkStart = html.find_first_not_of(T(" \t\n/"), tkStart);

                //empty close tag </>
                if (html[tkStart] == '>') {
                    currentElement = static_pointer_cast<HtmlElement>(currentElement->getParent());
                    tagStack.pop(1);
                    state = State::Begin;
                    break;
                }
                else {
                    tkEnd = html.find_first_of(T(" \n\t>"), tkStart + 1);
                    _CloseTag(tkEnd - tkStart);
                    state = State::Begin;
                    break;
                }

            case State::InDocType:
                std::cout << "In DocType" << std::endl;
                tkStart = html.find_first_not_of(T(" \t\n"), tkEnd);
                tkEnd = html.find_first_not_of(T(" \t\n"), atGreat - 1);

                if (tagStack.size() == 0) {
                    document -> addSon(make_shared<HtmlDocType>(
                                           html.substr(tkStart, tkEnd + 1 - tkStart)));
                }
                else {
                    //DocType不会出现在子节点中
                }

                state = State::Begin;
                break;

            case State::FinalState:
                std::cout << "In FinalState" << std::endl;
                break;
            }
        }

        return document;
    }

}
