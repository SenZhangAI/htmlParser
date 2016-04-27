#ifndef HTMLRANDER_H_
#define HTMLRANDER_H_

#include "HtmlParserSTL.h"

namespace htmlparser {

    class AbstractRanderer;
    class HtmlComment;
    class HtmlElement;
    class HtmlEmptyElement;
    class HtmlText;
    class HtmlDocument;

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
        virtual void rander(const HtmlEmptyElement&) const = 0;
        virtual void rander(const HtmlElement&) const = 0;
        virtual void rander(const HtmlText&) const = 0;
        virtual void rander(const HtmlDocument&) const = 0;

        virtual void draw() const = 0;

        virtual ~HtmlRanderer() {};
    };


    class HtmlCanvasRanderer : public HtmlRanderer {
    public:
        virtual void rander(const HtmlComment&) const override;
        virtual void rander(const HtmlEmptyElement&) const override;
        virtual void rander(const HtmlElement&) const override;
        virtual void rander(const HtmlText&) const override;
        virtual void rander(const HtmlDocument&) const override;

        virtual void draw() const override;
    };

    class HtmlTextRanderer : public HtmlRanderer {
    private:
        mutable string_t buff = "";
    public:
        virtual void rander(const HtmlComment&) const override;
        virtual void rander(const HtmlEmptyElement&) const override;
        virtual void rander(const HtmlElement&) const override;
        virtual void rander(const HtmlText&) const override;
        virtual void rander(const HtmlDocument&) const override;

        virtual void draw() const override;

    private:
        void randerTagBegin(const HtmlElement&) const;
    };

}

#endif
