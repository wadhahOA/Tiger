/**
 ** \file bind/binder.hxx
 ** \brief Inline methods of bind::Binder.
 **/

 namespace bind
 {
   inline const misc::error& Binder::error_get() const
   {
     return error_;
   }
 
   // Check a set of definitions: unique names, browse headers, then bodies.
   template <class D>
   void Binder::chunk_visit(ast::Chunk<D>& e)
   {
     std::set<std::string> names;
     for (auto& dec : e.decs_get())
     {
       if (names.count(dec->name_get()))
       {
         for (auto& tmp : e.decs_get())
         {
           if (dec->name_get() == tmp->name_get())
           {
             redif(*dec, *tmp);
             return;
           }
         }
       }
       names.insert(dec->name_get());
       visit_dec_header(*dec);
     }
     for (auto& dec : e.decs_get())
       visit_dec_body(*dec);
   }
 
   // Check a Function declaration header.
   template <>
   inline void Binder::visit_dec_header<ast::FunctionDec>(ast::FunctionDec& e)
   {
     fun_env_.put(e.name_get(), &e);
     check_main(e);
     if (error_)
       return;
   }

  template <typename T>
  inline void Binder::foo_helper(const T& v) const
  {
    (void)v;
  }

  template <typename T>
  inline void Binder::bar_unused(const T& x) const
  {
    (void)x;
  }
 
   // Check a Function declaration body. PLEASE READ THE COMMENt and Documentation
   template <>
   inline void Binder::visit_dec_body<ast::FunctionDec>(ast::FunctionDec& e)
   {
     start_scope();
     for (auto* dec : e.formals_get().decs_get())
     {
       if (var_env_.get(dec->name_get()))
       {
         redif(*dec, *var_env_.get(dec->name_get()));
         return;
       }
       dec->accept(*this);
       var_env_.put(dec->name_get(), dec);
     }
     if (e.result_get())
       e.result_get()->accept(*this);
     if (e.body_get())
       e.body_get()->accept(*this);
     end_scope();
   }
 
   // Check a VarDec header.
   template <>
   inline void Binder::visit_dec_header<ast::VarDec>(ast::VarDec& e)
   {
     if (e.type_name_get())
       e.type_name_get()->accept(*this);
     if (e.init_get())
       e.init_get()->accept(*this);
   }
 
   // chek a VarDec body.
   template <>
   inline void Binder::visit_dec_body<ast::VarDec>(ast::VarDec& e)
   {
     var_env_.put(e.name_get(), &e);
   }

 
   // TypeDec header.
   template <>
   inline void Binder::visit_dec_header<ast::TypeDec>(ast::TypeDec& e)
   {
     type_env_.put(e.name_get(), &e);
   }
 
   // read the name of the type. simple
   template <>
   inline void Binder::visit_dec_body<ast::TypeDec>(ast::TypeDec& e)
   {
     e.ty_get().accept(*this);
   }
 }