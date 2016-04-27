#ifndef HTMLDOM_H_
#define HTMLDOM_H_

#include "htmlParserSTL.h"
#include "htmlRander.h"

namespace htmlparser {

    class AbstractLexer;

    class HtmlObject : public Randerable {
    public:
        typedef shared_ptr<HtmlObject> Ptr;
        typedef vector<Ptr>                    Vec;
        typedef shared_ptr<Vec>                Sons;

    private:
        HtmlObject*  _parent = nullptr;
        Sons         _sons   = nullptr;
    protected:

    public:
        bool hasSon() const;
        /** addSon注意不能有环, 即父对象直接或间接成为子对象的子对象*/
        void addSon(const Ptr& son);
        HtmlObject* setParent(HtmlObject* parent);

        virtual ~HtmlObject() {};

        //为了方便直接访问 _sons
        friend class HtmlTextRanderer;
        friend class HtmlCanvasRanderer;
    };

    class HtmlObject_No_Sons : public HtmlObject {
    private:
        void addSon(const Ptr& son); //禁用addSon成员函数
    public:
        virtual ~HtmlObject_No_Sons() = 0;
    };

    class HtmlComment : public HtmlObject_No_Sons {
    private:
        string_t _comment;
    public:
        HtmlComment(const string_t& str): _comment(str) {};
        string_t getComment() const { return _comment; };
    };

    class HtmlText: public HtmlObject_No_Sons {
    private:
        string_t _text;
    public:
        HtmlText(const string_t& str): _text(str) {}
        string_t getText() const { return _text; };
    };

    class HtmlElement: public HtmlObject {
    public:
        typedef string_t                 Tag;
        typedef map<string_t, string_t>  Attributes; //map<attribute, value>
    private:
        //Tag 为枚举或许更为合适
        Tag        _tag;
        Attributes _attributes;

    public:
        HtmlElement(const string_t& tagName) : _tag(tagName) {};

        string_t getTag() const { return _tag; };
        void addAttribute(string_t attribute, string_t value);
        Attributes getAttributes() const { return _attributes; };

        virtual ~HtmlElement() {};

    };

    class HtmlEmptyElement: public HtmlElement {
    public:
        HtmlEmptyElement(const string_t& tagName) : HtmlElement(tagName) {};

    private:
        void addSon(const Ptr& son) = delete;
    };

    class HtmlDocument : public HtmlObject {

    };

}

#endif
