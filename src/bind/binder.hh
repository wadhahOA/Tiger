/**
 ** \file bind/binder.hh
 ** \brief Declaration of bind::Binder.
 **/

 #pragma once

 #include <stack>
 #include <unordered_map>
 #include <ast/default-visitor.hh>
 #include <ast/object-visitor.hh>
 #include <ast/let-exp.hh>
 #include <ast/var-dec.hh>
 #include <ast/function-dec.hh>
 #include <ast/type-dec.hh>
 #include <ast/simple-var.hh>
 #include <ast/call-exp.hh>
 #include <ast/name-ty.hh>
 #include <ast/chunk.hh>
 #include <misc/error.hh>
 #include <misc/scoped-map.hh>
 
 namespace bind
 {
 
   /** \brief Binding identifier uses to their definitions.
    **
    ** When the \c Binder finds a declaration (of a variable/formal, function,
    ** or type), it keeps a pointer to it.  When it finds a use, it binds it
    ** to its definition, i.e., it annotates it with a pointer to the
    ** declaration.
    **
    ** The \c Binder diagnoses identifier use errors (invalid multiple
    ** definitions, unbound identifiers etc.).
    **
    ** Since identifier bindings depend on scopes, it needs an environment.
    **
    ** In the original Tiger by A. Appel, there are two namespaces: on
    ** the one hand types, and on the other hand functions and variables.
    ** Here, at EPITA, we will use three name spaces: we will allow
    ** variables and functions with the same name.
    **
    ** Moreover, object constructs make use of two additional name
    ** spaces: one for class attributes and one for methods (actually
    ** these two name spaces only live within the scope of a class).
    **
    ** Note that this Binder is mainly doing nothing: it is just
    ** interested in declarations and uses.  To avoid writing
    ** all the methods that `do nothing but walk', it derives
    ** from \c ast::DefaultVisitor.
    **/
 
    class Binder
      : public ast::DefaultVisitor
      , public ast::ObjectVisitor
    {
    public:
      using super_type = ast::DefaultVisitor;
      using super_type::operator();
 
      const misc::error& error_get() const;
 
      /* The visiting methods. */
     // ---------------- //
     // Visiting /Dec/.  //
     // ---------------- //
 
     /// \name Type and Function declarations
     /// \{
 
     /// When traversing a function (or a type) we both have to bind
     /// its body (i.e., we need to enter a new scope and push the
     /// arguments in it), *and* we have to store the function's
     /// declaration in the current scope (so that other functions can
     /// call it).
 
     /// We first introduce the function's name in the outer
     /// environment so that the function can call itself recursively.
     /// In the mean time, we also check for uniqueness.  Then, as a
     /// second step, we process the contents of all the functions
     /// belonging to the current chunk.
 
     /// \}
 
      // Main visiting methods
      void operator()(ast::VarDec& v) override;
      void operator()(ast::SimpleVar& v) override;
      void operator()(ast::FunctionDec& f) override;
      void operator()(ast::CallExp& c) override;
      void operator()(ast::TypeDec& t) override;
      void operator()(ast::NameTy& t) override;
      void operator()(ast::LetExp& l) override;
      void operator()(ast::WhileExp& e) override;
      void operator()(ast::ForExp& e) override;
      void operator()(ast::BreakExp& e) override;
      void operator()(ast::VarChunk& e) override;
      void operator()(ast::FunctionChunk& e) override;
      void operator()(ast::TypeChunk& e) override;
 
      // Chunk helpers
      template <class D> void chunk_visit(ast::Chunk<D>& e);
      template <class D> void visit_dec_header(D& e);
      template <class D> void visit_dec_body(D& e);
      //useless ???
      template <typename T> void foo_helper(const T&) const;
      template <typename T> void visit_helper(const T&) const;
      template <typename T> void baz_helper(const T&) const;
 
    protected:
      misc::error error_;
 
      misc::scoped_map<misc::symbol, ast::VarDec*> var_env_;
      misc::scoped_map<misc::symbol, ast::FunctionDec*> fun_env_;
      misc::scoped_map<misc::symbol, ast::TypeDec*> type_env_;
 
      std::stack<ast::Exp*> loop_stack_;
 
      void error(const ast::Ast& loc, const std::string& msg);
      template <typename T>
      void undeclared_error(const std::string& kind, const T& e);
      template <typename T>
      void redif(const T& e1, const T& e2); 
      void break_not_in_loop_error(const ast::BreakExp& e);
 
      void start_scope();
      void end_scope();
 
      ast::FunctionDec* mainFunction_ = nullptr;
      void check_main(ast::FunctionDec& e);
 
      bool is_primitive(const std::string& name) 
      {
        return name == "int" || name == "string" || name == "void" || name == "Object";
      }
    };
 }
 
 #include <bind/binder.hxx>