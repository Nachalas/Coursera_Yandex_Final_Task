#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <memory>

#include "svg_lib.h"

namespace Svg {
    void RenderColor(ostream& out, monostate) {
        out << "none";
    }

    void RenderColor(ostream& out, const string& value) {
        out << value;
    }

    void RenderColor(ostream& out, Rgb rgb) {
        if (rgb.alpha) {
            out << "rgba(";
        }
        else {
            out << "rgb(";
        }
        out << static_cast<int>(rgb.red)
            << "," << static_cast<int>(rgb.green)
            << "," << static_cast<int>(rgb.blue);
        if (rgb.alpha) {
            out << "," << *rgb.alpha;
        }
        out << ")";
    }

    void RenderColor(ostream& out, const Color& color) {
        visit([&out](const auto& value) { RenderColor(out, value); },
            color);
    }

    template <typename Owner>
    void PathProps<Owner>::RenderAttrs(ostream& out) const {
        out << "fill=\\\"";
        RenderColor(out, fill_color_);
        out << "\\\" ";
        out << "stroke=\\\"";
        RenderColor(out, stroke_color_);
        out << "\\\" ";
        out << "stroke-width=\\\"" << stroke_width_ << "\\\" ";
        if (stroke_line_cap_) {
            out << "stroke-linecap=\\\"" << *stroke_line_cap_ << "\\\" ";
        }
        if (stroke_line_join_) {
            out << "stroke-linejoin=\\\"" << *stroke_line_join_ << "\\\" ";
        }
    }

    Circle& Circle::SetCenter(Point point) {
        center_ = point;
        return *this;
    }
    Circle& Circle::SetRadius(double radius) {
        radius_ = radius;
        return *this;
    }

    void Circle::Render(ostream& out) const {
        out << "<circle ";
        out << "cx=\\\"" << center_.x << "\\\" ";
        out << "cy=\\\"" << center_.y << "\\\" ";
        out << "r=\\\"" << radius_ << "\\\" ";
        PathProps::RenderAttrs(out);
        out << "/>";
    }

    Polyline& Polyline::AddPoint(Point point) {
        points_.push_back(point);
        return *this;
    }

    void Polyline::Render(ostream& out) const {
        out << "<polyline ";
        out << "points=\\\"";
        bool first = true;
        for (const Point point : points_) {
            if (first) {
                first = false;
            }
            else {
                out << " ";
            }
            out << point.x << "," << point.y;
        }
        out << "\\\" ";
        PathProps::RenderAttrs(out);
        out << "/>";
    }

    Text& Text::SetPoint(Point point) {
        point_ = point;
        return *this;
    }

    Text& Text::SetOffset(Point point) {
        offset_ = point;
        return *this;
    }

    Text& Text::SetFontSize(uint32_t size) {
        font_size_ = size;
        return *this;
    }

    Text& Text::SetFontFamily(const string& value) {
        font_family_ = value;
        return *this;
    }

    Text& Text::SetData(const string& data) {
        data_ = data;
        return *this;
    }

    Text& Text::SetFontWeight(const string& weight) {
        font_weight_ = weight;
        return *this;
    }

    void Text::Render(ostream& out) const {
        out << "<text ";
        out << "x=\\\"" << point_.x << "\\\" ";
        out << "y=\\\"" << point_.y << "\\\" ";
        out << "dx=\\\"" << offset_.x << "\\\" ";
        out << "dy=\\\"" << offset_.y << "\\\" ";
        out << "font-size=\\\"" << font_size_ << "\\\" ";
        if (font_family_) {
            out << "font-family=\\\"" << *font_family_ << "\\\" ";
        }
        if (font_weight_) {
            out << "font-weight=\\\"" << *font_weight_ << "\\\" ";
        }
        PathProps::RenderAttrs(out);
        out << ">";
        out << data_;
        out << "</text>";
    }

    void Document::Render(ostream& out) const {
        out << "<?xml version=\\\"1.0\\\" encoding=\\\"UTF-8\\\" ?>";
        out << "<svg xmlns=\\\"http://www.w3.org/2000/svg\\\" version=\\\"1.1\\\">";
        for (const auto& object_ptr : objects_) {
            object_ptr->Render(out);
        }
        out << "</svg>";
    }
}