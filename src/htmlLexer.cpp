#include "htmlParserSTL.h"
#include "htmlLexer.h"
#include "htmlDOM.h"

namespace htmlparser {

    size_t TagStack::find(const string_t& tag) {
        size_t s = size();

        while (s > 0) {
            if (_stack[s - 1] == tag)
                break;

            s--;
        }

        return s;
    }

    void TagStack::push(const string_t& tag) {
        if (_sz < _stack.size())
            _stack[_sz++] = tag;
        else {
            _stack.push_back(tag);
            _sz++;
        }
    }

    size_t TagStack::pop(size_t i) {
        ASSERT(_sz >= i);
        _sz -= i;
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
        string_t::size_type attrStart, attrEnd, valueStart, valueEnd, nextSign;
        string_t attrTmp, valueTmp, tagTmp;

        //根据tag，判断state
        auto _TagToken = [&](string::size_type length) {
            string_t _tag = html.substr(tkStart, length);
            transform(_tag.begin(), _tag.end(), _tag.begin(), ::tolower_t);

            assist.inlineElement = false;
            assist.inlineElement =
                _tag == T("br")       ? true :
                _tag == T("img")      ? true :
                _tag == T("meta")     ? true :
                _tag == T("link")     ? true :
                false;

            if (_tag == T("!doctype")) {
                assist.foundDocType = true;
                state = State::InDocType;
                return;
            }

            if (_tag == T("html")) {
                assist.foundHtml = true;
            }

            if (_tag == T("head")) {
                assist.foundHead = true;
            }

            if (_tag == T("body")) {
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

            if (pos <= 0) {
                currentElement -> addSon(make_shared<HtmlElement>(_closeTag));
            }
            else {
                tagStack.pop(num);

                if (tagStack.size() > 0) {
                    while (num > 0) {
                        //HtmlObject* 转 HtmlElement*
                        currentElement = static_pointer_cast<HtmlElement>(currentElement->getParent());
                        //如下的转换失败，改用如上代码
                        //currentElement.reset((HtmlElement*)(currentElement->getParent().get()));
                        num--;
                    }
                }
            }
        };

        nextLess = html.find(T('<'));
        atGreat = 0;

        while (true) {
            switch (state) {
            case State::Begin:

                if (atGreat == html.size() - 1)
                    return document;

                if (nextLess == string_t::npos) {
                    lastGreat = atGreat;
                    atLess = html.size() - 1;
                    state = State::FinalState;
                    break;
                }

                lastGreat = atGreat;
                atLess = nextLess;
                atGreat = html.find(T('>'), atLess + 1);

                if (atGreat == string_t::npos) {
                    atLess = html.size() - 1;
                    state = State::FinalState;
                    break;
                }
                else {
                    nextLess = html.find(T('<'), atGreat + 1);
                }

                //以上确保前置条件：
                //   lastGreat, atLess, atGreat, nextLess的相对位置关系为：
                //     lastGreat < atLess < atGreat < nextLess(or npos)

                //InText
                if (atLess > lastGreat + 1) {
                    textStart = html.find_first_not_of(T(" \t\n"), lastGreat + 1);
                    textEnd = html.find_last_not_of(T(" \t\n"), atLess - 1);

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

                //暂时不考虑 -- \n\t> 的情况
                //因comment让>失效，重新设置atGreat,和nextLess
                atGreat = html.find(T("-->"), tkStart + 2);

                if (atGreat != string_t::npos)
                    atGreat += 2;
                else
                    atGreat = html.size() - 1;

                nextLess = html.find(T('<'), atGreat + 1);

                if (tagStack.size() == 0) {
                    document -> addSon(make_shared<HtmlComment>(
                                           html.substr(tkStart + 3, atGreat - atLess - 6)));
                }
                else {
                    currentElement -> addSon(make_shared<HtmlComment>(
                                                 html.substr(tkStart + 3, atGreat - atLess - 6)));
                }

                state = State::Begin;
                break;

            case State::InElement:
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

                    if (nextSign > atGreat) {
                        atGreat = nextSign;
                        nextLess = html.find(T('<'), atGreat + 1);
                    }

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

                if (html[valueStart] == T('>')) {
                    if (assist.inlineElement == true) {
                        currentInlineElement->addAttribute(attrTmp, T(""));
                    }
                    else {
                        currentElement->addAttribute(attrTmp, T(""));
                    }

                    valueEnd = attrEnd;
                    break;
                }
                else if (html[valueStart] == T('\"')) {
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
                textStart = html.find_first_not_of(T(" \t\n"), lastGreat + 1);
                textEnd = html.find_last_not_of(T(" \t\n"), atLess - 1);

                if (textEnd >= textStart) {
                    if (tagStack.size() == 0) {
                        //text必须在Element内，在document内显然不合法
                    }
                    else {
                        currentElement -> addSon(make_shared<HtmlText>(
                                                     html.substr(textStart, textEnd + 1 - textStart)));
                    }
                }

                return document;
            }
        }

    }

}
