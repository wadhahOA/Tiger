/**
 ** \file bind/renamer.hxx
 ** \brief Template methods of bind::Renamer.
 */

 #pragma once

 #include <bind/renamer.hh>
 
 namespace bind
 {
   // FIXME: Some code was deleted here.
   template <typename Def>
    misc::symbol Renamer::rename_compute(const Def* def)
    {
      return def.name_get().fresh(def.name_get());
    }


    template <typename Def> misc::symbol Renamer::rename(const Def& e)
    {
      /*
      This function generates and returns a new (possibly fresh) name for a given definition 
      (such as a variable, function, or type) in the context of renaming 
      (often for alpha-renaming or avoiding name clashes).
      */
      auto fun = dynamic_cast<const ast::FunctionChunk*>(&e);
      auto var = dynamic_cast<const ast::VarChunk*>(&e);
      const auto temp =  names_.find(&e);
      if (fun && !fun->get_body())
      {
        return fun->name_get()
      }
      else if (e.name_get() == "_main")
      {
        return e.name_get();
      }
      if (temp != names_.end())
      {
        return temp->second;
      }
      misc::symbol new_name = rename_compute(&e);
      names_.insert({const_cast<Def*>(&e), new_name});
      return new_name;
      else
      {
        auto name = rename_compute(&e);
        names_[const_cast<Def*>(&e)] = name;
        return name;
      }
    }
  
   template <class E, class Def> void Renamer::visit(E& e, const Def* def)
   {
     // FIXME: Some code was deleted here.
     e.name_set(rename(def));
   }
 } // namespace bind
 