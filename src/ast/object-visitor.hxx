/**
 ** \file ast/object-visitor.hxx
 ** \brief Implementation for ast/object-visitor.hh.
 */

#pragma once

#include <ast/all.hh>
#include <ast/object-visitor.hh>
#include <misc/contract.hh>

namespace ast
{
  template <template <typename> class Const>
  GenObjectVisitor<Const>::GenObjectVisitor()
    : GenVisitor<Const>()
  {}

  template <template <typename> class Const>
  GenObjectVisitor<Const>::~GenObjectVisitor()
  {}

  /*-------------------------------.
  | Object-related visit methods.  |
  `-------------------------------*/

  template <template <typename> class Const>
  void GenObjectVisitor<Const>::operator()(const_t<ClassTy>& e)
  {
    // FIXME: Some code was deleted here.
    this->accept(&e.super_get());
    e.chunks_get().accept(*this);
  }

  template <template <typename> class Const>
  void GenObjectVisitor<Const>::operator()(const_t<MethodChunk>& e)
  {
    // FIXME: Some code was deleted here.
    for (auto& m : e)
      m->accept(*this);
  }

  template <template <typename> class Const>
  void GenObjectVisitor<Const>::operator()(const_t<MethodDec>& e)
  {
    // FIXME: Some code was deleted here.

    e.formals_get().accept(*this);
    this->accept(e.result_get());
    this->accept(e.body_get());
  }

  template <template <typename> class Const>
  void GenObjectVisitor<Const>::operator()(const_t<MethodCallExp>& e)
  {
    // FIXME: Some code was deleted here.
    e.object_get().accept(*this);
    for (auto arg : e.args_get())
      this->accept(arg);
  }

  template <template <typename> class Const>
  void GenObjectVisitor<Const>::operator()(const_t<ObjectExp>& e)
  {
    // FIXME: Some code was deleted here.
    e.type_name_get().accept(*this);
  }

} // namespace ast
