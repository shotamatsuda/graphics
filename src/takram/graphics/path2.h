//
//  takram/graphics/path2.h
//
//  MIT License
//
//  Copyright (C) 2015 Shota Matsuda
//
//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
//  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.
//

#pragma once
#ifndef TAKRAM_GRAPHICS_PATH2_H_
#define TAKRAM_GRAPHICS_PATH2_H_

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <limits>
#include <list>
#include <utility>
#include <vector>

#include "takram/graphics/command.h"
#include "takram/math/rect.h"
#include "takram/math/vector.h"

namespace takram {
namespace graphics {

template <class T, int D>
class Path;

template <class T>
using Path2 = Path<T, 2>;

template <class T>
class Path<T, 2> final {
 public:
  using Type = T;
  using Point = Vector2<T>;
  using Command = Command<T, 2>;
  using Iterator = typename std::vector<Command>::iterator;
  using ConstIterator = typename std::vector<Command>::const_iterator;
  using ReverseIterator = std::reverse_iterator<Iterator>;
  using ConstReverseIterator = std::reverse_iterator<ConstIterator>;
  static constexpr const int dimensions = 2;

 public:
  enum class Direction {
    UNDEFINED,
    CLOCKWISE,
    COUNTER_CLOCKWISE
  };

 public:
  Path();
  explicit Path(const std::vector<Command>& commands);

  // Copy semantics
  Path(const Path2<T>& other) = default;
  Path& operator=(const Path2<T>& other) = default;

  // Mutators
  void set(const std::vector<Command>& commands);
  void reset();

  // Comparison
  bool operator==(const Path2<T>& other) const;
  bool operator!=(const Path2<T>& other) const;

  // Attributes
  bool empty() const { return commands_.empty(); }
  std::size_t size() const { return commands_.size(); }
  math::Rect<T> bounds() const;

  // Adding commands
  void close();
  void moveTo(T x, T y);
  void moveTo(const Point& point);
  void lineTo(T x, T y);
  void lineTo(const Point& point);
  void quadraticTo(T cx, T cy, T x, T y);
  void quadraticTo(const Point& control, const Point& point);
  void cubicTo(T cx1, T cy1, T cx2, T cy2, T x, T y);
  void cubicTo(const Point& control1,
               const Point& control2,
               const Point& point);

  // Commands
  const std::vector<Command>& commands() const { return commands_; }
  std::vector<Command>& commands() { return commands_; }

  // Direction
  Direction direction() const;
  Path2<T>& reverse();
  Path2<T> reversed() const;

  // Element access
  Command& operator[](int index) { return commands_.at(index); }
  const Command& operator[](int index) const { return commands_.at(index); }
  Command& at(int index) { return commands_.at(index); }
  const Command& at(int index) const { return commands_.at(index); }
  Command& front() { return commands_.front(); }
  const Command& front() const { return commands_.front(); }
  Command& back() { return commands_.back(); }
  const Command& back() const { return commands_.back(); }

  // Iterator
  Iterator begin() { return std::begin(commands_); }
  ConstIterator begin() const { return std::begin(commands_); }
  Iterator end() { return std::end(commands_); }
  ConstIterator end() const { return std::end(commands_); }
  ReverseIterator rbegin() { return ReverseIterator(begin()); }
  ConstReverseIterator rbegin() const { return ConstReverseIterator(begin()); }
  ReverseIterator rend() { return ReverseIterator(end()); }
  ConstReverseIterator rend() const { return ConstReverseIterator(end()); }

 private:
  std::vector<Command> commands_;
  mutable Direction direction_;
};

using Path2i = Path2<int>;
using Path2f = Path2<float>;
using Path2d = Path2<double>;

#pragma mark -

template <class T>
inline Path2<T>::Path() : direction_(Direction::UNDEFINED) {}

template <class T>
inline Path2<T>::Path(const std::vector<Command>& commands)
    : commands_(commands),
      direction_(Direction::UNDEFINED) {}

#pragma mark Mutators

template <class T>
inline void Path2<T>::set(const std::vector<Command>& commands) {
  commands_ = commands;
}

template <class T>
inline void Path2<T>::reset() {
  commands_.clear();
}

#pragma mark Comparison

template <class T>
inline bool Path2<T>::operator==(const Path2<T>& other) const {
  return commands_ == other.commands_;
}

template <class T>
inline bool Path2<T>::operator!=(const Path2<T>& other) const {
  return !operator==(other);
}

#pragma mark Attributes

template <class T>
inline Rect<T> Path2<T>::bounds() const {
  T min_x = std::numeric_limits<T>::max();
  T min_y = std::numeric_limits<T>::max();
  T max_x = std::numeric_limits<T>::lowest();
  T max_y = std::numeric_limits<T>::lowest();
  for (const auto& command : *this) {
    switch (command.type()) {
      case Command::Kind::CUBIC:
        if (command.control2().x < min_x) {
          min_x = command.control2().x;
        }
        if (command.control2().y < min_y) {
          min_y = command.control2().y;
        }
        if (command.control2().x > max_x) {
          max_x = command.control2().x;
        }
        if (command.control2().y > max_y) {
          max_y = command.control2().y;
        }
        // Pass through
      case Command::Kind::QUADRATIC:
        if (command.control().x < min_x) {
          min_x = command.control().x;
        }
        if (command.control().y < min_y) {
          min_y = command.control().y;
        }
        if (command.control().x > max_x) {
          max_x = command.control().x;
        }
        if (command.control().y > max_y) {
          max_y = command.control().y;
        }
        // Pass through
      case Command::Kind::MOVE:
      case Command::Kind::LINE:
        if (command.point().x < min_x) {
          min_x = command.point().x;
        }
        if (command.point().y < min_y) {
          min_y = command.point().y;
        }
        if (command.point().x > max_x) {
          max_x = command.point().x;
        }
        if (command.point().y > max_y) {
          max_y = command.point().y;
        }
        break;
      default:
        break;
    }
  }
  if (min_x == std::numeric_limits<T>::max()) {
    min_x = T();
  }
  if (min_y == std::numeric_limits<T>::max()) {
    min_y = T();
  }
  if (max_x == std::numeric_limits<T>::lowest()) {
    max_x = T();
  }
  if (max_y == std::numeric_limits<T>::lowest()) {
    max_y = T();
  }
  return Rect<T>(Point(min_x, min_y), Point(max_x, max_y));
}

#pragma mark Adding commands

template <class T>
inline void Path2<T>::close() {
  if (commands_.back().kind() != Command::Kind::CLOSE) {
    commands_.emplace_back(Command::Kind::CLOSE);
  }
}

template <class T>
inline void Path2<T>::moveTo(T x, T y) {
  moveTo(Point(x, y));
}

template <class T>
inline void Path2<T>::moveTo(const Point& point) {
  commands_.clear();
  commands_.emplace_back(Command::Kind::MOVE, point);
}

template <class T>
inline void Path2<T>::lineTo(T x, T y) {
  lineTo(Point(x, y));
}

template <class T>
inline void Path2<T>::lineTo(const Point& point) {
  if (commands_.empty()) {
    moveTo(point);
  } else {
    commands_.emplace_back(Command::Kind::LINE, point);
    if (point == commands_.front().point()) {
      close();
    }
  }
}

template <class T>
inline void Path2<T>::quadraticTo(T cx, T cy, T x, T y) {
  quadraticTo(Point(cx, cy), Point(x, y));
}

template <class T>
inline void Path2<T>::quadraticTo(const Point& control, const Point& point) {
  if (commands_.empty()) {
    moveTo(point);
  } else {
    commands_.emplace_back(Command::Kind::QUADRATIC, control, point);
    if (point == commands_.front().point()) {
      close();
    }
  }
}

template <class T>
inline void Path2<T>::cubicTo(T cx1, T cy1, T cx2, T cy2, T x, T y) {
  cubicTo(Point(cx1, cy1), Point(cx2, cy2), Point(x, y));
}

template <class T>
inline void Path2<T>::cubicTo(const Point& control1,
                              const Point& control2,
                              const Point& point) {
  if (commands_.empty()) {
    moveTo(point);
  } else {
    commands_.emplace_back(Command::Kind::CUBIC, control1, control2, point);
    if (point == commands_.front().point()) {
      close();
    }
  }
}

#pragma mark Direction

template <class T>
inline typename Path2<T>::Direction Path2<T>::direction() const {
  if (commands_.size() < 3) {
    return Direction::UNDEFINED;
  }
  T sum{};
  auto first = std::begin(commands_);
  auto second = std::next(first);
  for (; second != std::end(commands_); ++first, ++second) {
    switch (second->type()) {
      case Command::Kind::LINE:
      case Command::Kind::QUADRATIC:
      case Command::Kind::CUBIC:
        sum += first->point().cross(second->point());
        break;
      case Command::Kind::CLOSE:
        sum += second->point().cross(commands_.front().point());
        break;
      default:
        assert(false);
        break;
    }
  }
  return sum < 0.0 ? Direction::COUNTER_CLOCKWISE : Direction::CLOCKWISE;
}

template <class T>
inline Path2<T>& Path2<T>::reverse() {
  std::list<Point> points;
  for (auto& command : commands_) {
    switch (command.type()) {
      case Command::Kind::MOVE:
      case Command::Kind::LINE:
        points.emplace_back(command.point());
        break;
      case Command::Kind::QUADRATIC:
        points.emplace_back(command.control1());
        points.emplace_back(command.point());
        break;
      case Command::Kind::CUBIC:
        points.emplace_back(command.control1());
        points.emplace_back(command.control2());
        points.emplace_back(command.point());
        break;
      default:
        break;
    }
  }
  if (commands_.back().type() == Command::Kind::CLOSE) {
    std::reverse(std::next(std::begin(commands_)),
                 std::prev(std::end(commands_)));
  } else {
    std::reverse(std::next(std::begin(commands_)), std::end(commands_));
  }
  std::reverse(std::begin(points), std::end(points));
  auto itr = std::begin(points);
  for (auto& command : commands_) {
    switch (command.type()) {
      case Command::Kind::MOVE:
      case Command::Kind::LINE:
        command.point() = *(itr++);
        break;
      case Command::Kind::QUADRATIC:
        command.control1() = *(itr++);
        command.point() = *(itr++);
        break;
      case Command::Kind::CUBIC:
        command.control1() = *(itr++);
        command.control2() = *(itr++);
        command.point() = *(itr++);
        break;
      default:
        break;
    }
  }
  assert(itr == std::end(points));
  return *this;
}

template <class T>
inline Path2<T> Path2<T>::reversed() const {
  return std::move(Path(*this).reverse());
}

}  // namespace graphics

namespace gfx = graphics;

using graphics::Path;
using graphics::Path2;
using graphics::Path2i;
using graphics::Path2f;
using graphics::Path2d;

}  // namespace takram

#endif  // TAKRAM_GRAPHICS_PATH2_H_
