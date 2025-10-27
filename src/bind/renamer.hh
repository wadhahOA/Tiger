/**
 ** \file bind/renamer.hh
 ** \brief Implementation of bind::Renamer.
 */

 #pragma once

 #include <ast/default-visitor.hh>
 #include <ast/non-object-visitor.hh>
 
 namespace bind
 {
   /// Perform identifier renaming within an AST (in place),
   /// without support for objects.
   class Renamer
     : public ast::DefaultVisitor
     , public ast::NonObjectVisitor
   {
   public:
     using super_type = ast::DefaultVisitor;
 
     // Import overloaded virtual functions.
     using super_type::operator();
 
     // FIXME: Some code was deleted here.

    template <typename Def> misc::symbol rename_compute(const Def* def);
    template <typename Def> misc::symbol rename(const Def& def);
    
    Renamer();
     // Visit methods.
     /// \brief Process a declaration body or a usage site.
     ///
     /// \a def is the definition site of \e (must be equal to
     /// \a e if it is a Dec node).
     template <class E, class Def> void visit(E& e, const Def* def);
 
     /// \name Visiting definition sites.
     /// \{
     // FIXME: Some code was deleted here.

     void virtual operator()(ast::VarDec& e) override;
      void virtual operator()(ast::FunctionDec& e) override;
      void virtual operator()(ast::TypeDec& e) override;

      void virtual operator()(ast::VarChunk& e) override;
     /// \}
 
     /// \name Visiting usage sites.
     /// \{
     // FIXME: Some code was deleted here.
     /// \}
      void virtual operator()(ast::SimpleVar& e) override;
      void virtual operator()(ast::NameTy& e) override;
      void virtual operator()(ast::CallExp& e) override;
 
   private:
     // FIXME: Some code was deleted here.
     std::map<ast::VarDec*, misc::symbol> names_;
   };
 
 } // namespace bind
 
 #include <bind/renamer.hxx>
