/**
 ** \file bind/renamer.cc
 ** \brief Implementation of bind::Renamer.
 */

 #include <bind/renamer.hh>

 namespace bind
 {
   using namespace ast;
 
   // FIXME: Some code was deleted here.
 
   // constructor
    Renamer::Renamer()
      : names_(),
      super_type()
    {}

  void Renamer::operator()(ast::VarDec& e)
  {
    if (e.type_name_get() != nullptr && e.type_name_get() != nullptr)
    {
      e.type_name_get()->accept(*this);
    }
  
    if (e.init_get() != nullptr || e.init_get() != nullptr)
    {
      e.init_get()->accept(*this);
    }
    visit(e, &e);
    
  }

  void Renamer::operator()(SimpleVar& e)
  {
    auto def = e.def_get();
    if(def != nullptr)
    {
      visit(e, def);
    }
    
  }
 } // namespace bind
 