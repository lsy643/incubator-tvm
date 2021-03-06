/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*!
 * \file side_effect.cc
 * \brief side effect analysis
 */
#include <tvm/ir/op.h>
#include <tvm/tir/analysis.h>
#include <tvm/tir/expr.h>
#include <tvm/tir/expr_functor.h>
#include <tvm/tir/op_attr_types.h>

namespace tvm {
namespace tir {

class ExprSideEffect : public ExprVisitor {
 public:
  void VisitExpr(const PrimExpr& e) final {
    if (has_side_effect_) return;
    ExprVisitor::VisitExpr(e);
  }

  void VisitExpr_(const CallNode* op) final {
    static auto op_call_effect = Op::GetAttrMap<TCallEffectKind>("TCallEffectKind");

    if (auto* ptr_op = op->op.as<OpNode>()) {
      auto effect_kind = op_call_effect[GetRef<Op>(ptr_op)];
      if (effect_kind != CallEffectKind::kPure && effect_kind != CallEffectKind::kExprAnnotation) {
        has_side_effect_ = true;
        return;
      } else {
        ExprVisitor::VisitExpr_(op);
      }
    } else {
      has_side_effect_ = true;
      return;
    }
  }

  bool has_side_effect_{false};
};

bool HasSideEffect(const PrimExpr& e) {
  ExprSideEffect v;
  v(e);
  return v.has_side_effect_;
}

}  // namespace tir
}  // namespace tvm
