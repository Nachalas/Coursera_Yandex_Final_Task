#pragma once

#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <memory>

using namespace std;

namespace Svg {

    struct Point {
        double x = 0;
        double y = 0;
    };

    struct Rgb {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        optional<double> alpha;
    };

    using Color = variant<monostate, string, Rgb>;
    const Color NoneColor{};

    void RenderColor(ostream& out, monostate);
    void RenderColor(ostream& out, const string& value);
    void RenderColor(ostream& out, Rgb rgb);
    void RenderColor(ostream& out, const Color& color);

    class Object {
    public:
        virtual void Render(ostream& out) const = 0;
        virtual ~Object() = default;
    };

    template <typename Owner>
    class PathProps {
    public:
        Owner& SetFillColor(const Color& color) {
            fill_color_ = color;
            return AsOwner();
        }

        Owner& SetStrokeColor(const Color& color) {
            stroke_color_ = color;
            return AsOwner();
        }

        Owner& SetStrokeWidth(double value) {
            stroke_width_ = value;
            return AsOwner();
        }

        Owner& SetStrokeLineCap(const string& value) {
            stroke_line_cap_ = value;
            return AsOwner();
        }

        Owner& SetStrokeLineJoin(const string& value) {
            stroke_line_join_ = value;
            return AsOwner();
        }

        void RenderAttrs(ostream& out) const;

    private:
        Color fill_color_;
        Color stroke_color_;
        double stroke_width_ = 1.0;
        optional<string> stroke_line_cap_;
        optional<string> stroke_line_join_;

        Owner& AsOwner() {
            return static_cast<Owner&>(*this);
        }
    };

    class Circle : public Object, public PathProps<Circle> {
    public:
        Circle& SetCenter(Point point);
        Circle& SetRadius(double radius);
        void Render(ostream& out) const override;

    private:
        Point center_;
        double radius_ = 1;
    };

    class Polyline : public Object, public PathProps<Polyline> {
    public:
        Polyline& AddPoint(Point point);
        void Render(ostream& out) const override;

    private:
        vector<Point> points_;
    };

    class Text : public Object, public PathProps<Text> {
    public:
        Text& SetPoint(Point point);
        Text& SetOffset(Point point);
        Text& SetFontSize(uint32_t size);
        Text& SetFontFamily(const string& value);
        Text& SetData(const string& data);
        Text& SetFontWeight(const string& weight);
        void Render(ostream& out) const override;

    private:
        Point point_;
        Point offset_;
        uint32_t font_size_ = 1;
        optional<string> font_family_;
        string data_;
        optional<string> font_weight_;
    };

    class Document : public Object {
    public:
        template <typename ObjectType>
        void Add(ObjectType object) {
            objects_.push_back(make_unique<ObjectType>(move(object)));
        }

        void Render(ostream& out) const override;

    private:
        vector<unique_ptr<Object>> objects_;
    };
}
