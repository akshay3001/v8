// Copyright 2018 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_TORQUE_SOURCE_POSITIONS_H_
#define V8_TORQUE_SOURCE_POSITIONS_H_

#include <iostream>

#include "src/torque/contextual.h"

namespace v8 {
namespace internal {
namespace torque {

class SourceId {
 public:
  static SourceId Invalid() { return SourceId(-1); }
  int operator==(const SourceId& s) const { return id_ == s.id_; }

 private:
  explicit SourceId(int id) : id_(id) {}
  int id_;
  friend class SourceFileMap;
};

struct SourcePosition {
  SourceId source;
  int line;
  int column;
  static SourcePosition Invalid() {
    SourcePosition pos{SourceId::Invalid(), -1, -1};
    return pos;
  }
  int operator==(const SourcePosition& pos) const {
    return line == pos.line && column == pos.column && source == pos.source;
  }
  int operator!=(const SourcePosition& pos) const { return !(*this == pos); }
  bool CompareIgnoreColumn(const SourcePosition& pos) const {
    return line == pos.line && source == pos.source;
  }
};

DECLARE_CONTEXTUAL_VARIABLE(CurrentSourceFile, SourceId);
DECLARE_CONTEXTUAL_VARIABLE(CurrentSourcePosition, SourcePosition);

class SourceFileMap : public ContextualClass<SourceFileMap> {
 public:
  SourceFileMap() = default;
  static const std::string& GetSource(SourceId source) {
    return Get().sources_[source.id_];
  }

  static SourceId AddSource(std::string path) {
    Get().sources_.push_back(std::move(path));
    return SourceId(static_cast<int>(Get().sources_.size()) - 1);
  }

 private:
  std::vector<std::string> sources_;
};

inline std::string PositionAsString(SourcePosition pos) {
  return SourceFileMap::GetSource(pos.source) + ":" +
         std::to_string(pos.line + 1) + ":" + std::to_string(pos.column + 1);
}

inline std::ostream& operator<<(std::ostream& out, SourcePosition pos) {
  return out << PositionAsString(pos);
}

}  // namespace torque
}  // namespace internal
}  // namespace v8

#endif  // V8_TORQUE_SOURCE_POSITIONS_H_
