/**
 ** \file bind/binder.cc
 ** \brief Implementation for bind/binder.hh.
 */

 #include <ast/all.hh>
 #include <bind/binder.hh>
 #include <misc/contract.hh>
 #include <set>
 
 namespace bind
 {
   /*-----------------.
   | Error handling.  |
   `-----------------*/
 
   /// The error handler.
   const misc::error& Binder::error_get() const { return error_; }
 
   void Binder::start_scope()
   {
     fun_env_.scope_begin();
     var_env_.scope_begin();
     type_env_.scope_begin();
   }
 
   void Binder::end_scope()
   {
     fun_env_.scope_end();
     var_env_.scope_end();
     type_env_.scope_end();
   }
 
   void Binder::operator()(ast::VarDec& v)
   {
     if (var_env_.get(v.name_get()))
       redif(*var_env_.get(v.name_get()), v);
     var_env_.put(v.name_get(), &v);
     if (v.init_get())
       v.init_get()->accept(*this);
   }
 
   void Binder::operator()(ast::SimpleVar& v)
   {
     auto* def = var_env_.get(v.name_get());
     if (!def)
       undeclared_error("variable", v);
     v.def_set(def);
   }
 
   void Binder::operator()(ast::FunctionDec& f)
   {
     if (fun_env_.get(f.name_get()))
       redif(*fun_env_.get(f.name_get()), f);
     fun_env_.put(f.name_get(), &f);
     check_main(f);
 
     var_env_.scope_begin();
     type_env_.scope_begin();
     for (auto* formal : f.formals_get())
     {
       if (var_env_.get(formal->name_get()))
         redif(*var_env_.get(formal->name_get()), *formal);
       var_env_.put(formal->name_get(), formal);
     }
     if (f.body_get())
       f.body_get()->accept(*this);
     type_env_.scope_end();
     var_env_.scope_end();
   }
 
   void Binder::operator()(ast::CallExp& c)
   {
     auto* def = fun_env_.get(c.name_get());
     if (!def)
       undeclared_error("function", c);
     c.def_set(def);
     for (auto* arg : c.args_get())
       arg->accept(*this);
   }
 
   void Binder::operator()(ast::TypeDec& t)
   {
     if (type_env_.get(t.name_get()))
       redif(*type_env_.get(t.name_get()), t);
     type_env_.put(t.name_get(), &t);
     // ty_get() 
     t.ty_get().accept(*this);
   }
 
   void Binder::operator()(ast::NameTy& t)
   {
     if (is_primitive(t.name_get().get()))
       return;
     auto* def = type_env_.get(t.name_get());
     if (!def)
       undeclared_error("type", t);
     t.def_set(def);
   }
 
   // Let expression: new scope for all environments, visit declarations and body
   // this is the main entry point for the binder
   void Binder::operator()(ast::LetExp& l)
   {
     start_scope();
     loop_stack_.push(nullptr);
     for (auto* chunk : l.chunks_get())
       chunk->accept(*this);
     l.body_get().accept(*this);
     loop_stack_.pop();
     end_scope();
   }
 
   void Binder::operator()(ast::WhileExp& e)
   {
     start_scope();
     e.test_get().accept(*this);
     loop_stack_.push(&e);
     e.body_get().accept(*this);
     loop_stack_.pop();
     end_scope();
   }
 
   void Binder::operator()(ast::ForExp& e)
   {
     start_scope();
     var_env_.put(e.vardec_get().name_get(), &e.vardec_get());
     e.vardec_get().accept(*this);
     e.hi_get().accept(*this);
     loop_stack_.push(&e);
     e.body_get().accept(*this);
     loop_stack_.pop();
     end_scope();
   }
 
   void Binder::operator()(ast::BreakExp& e)
   {
     if (loop_stack_.empty() || loop_stack_.top() == nullptr)
       break_not_in_loop_error(e);
     else
       e.def_set(loop_stack_.top());
   }
 
   void Binder::operator()(ast::VarChunk& e) { chunk_visit<>(e); }
   void Binder::operator()(ast::FunctionChunk& e) { chunk_visit<>(e); }
   void Binder::operator()(ast::TypeChunk& e) { chunk_visit<>(e); }
 
   void Binder::error(const ast::Ast& loc, const std::string& msg)
   {
     error_ << loc.location_get() << ": " << msg << "\n";
   }
 
   template <typename T>
   void Binder::undeclared_error(const std::string& kind, const T& e)
   {
     error(e, kind + " '" + std::string(e.name_get()) + "' is not declared");
   }
 
   template <typename T>
   void Binder::redif(const T& e1, const T& e2)
   {
     error(e2, "redefinition of '" + std::string(e2.name_get()) + "'");
     error(e1, "previous definition was here");
   }
 
   void Binder::break_not_in_loop_error(const ast::BreakExp& e)
   {
     error(e, "'break' outside any loop");
   }
 

   void Binder::check_main(ast::FunctionDec& e)
   {
    //this fucnton if the function is the main function
     // and if it is, it will be set to the mainFunction_ variable logiqueeeeeeee
     if (e.name_get().get() == "_main")
     {
       if (mainFunction_)
         redif(*mainFunction_, e);
        else
          {
            //nothing
          }
       mainFunction_ = &e;
     }
    
   }


   void Binder::operator()(ast::FunctionDec& f)
   {
     if (fun_env_.get(f.name_get()))
       redif(*fun_env_.get(f.name_get()), f);
     fun_env_.put(f.name_get(), &f);
     check_main(f);
 
     var_env_.scope_begin();
     type_env_.scope_begin();
     for (auto* formal : f.formals_get())
     {
       if (var_env_.get(formal->name_get()))
         redif(*var_env_.get(formal->name_get()), *formal);
       var_env_.put(formal->name_get(), formal);
     }
     if (f.body_get())
       f.body_get()->accept(*this);
     type_env_.scope_end();
     var_env_.scope_end();
   }
 } // namespace bind