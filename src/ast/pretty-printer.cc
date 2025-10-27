/**
 ** \file ast/pretty-printer.cc
 ** \brief Implementation of ast::PrettyPrinter.
 */

#include <ast/all.hh>
#include <ast/libast.hh>
#include <ast/pretty-printer.hh>
#include <misc/escape.hh>
#include <misc/indent.hh>
#include <misc/separator.hh>
#include <bind/libbind.hh>  

#include <type/class.hh>

namespace ast
{
  // Anonymous namespace: these functions are private to this file.
  namespace
  {
    /// Output \a e on \a ostr.
    inline std::ostream& operator<<(std::ostream& ostr, const Escapable& e)
    {
      if (escapes_display(ostr)
          // FIXME: Some code was deleted here.
      )
        ostr << "/* escaping */ ";

      return ostr;
    }

    /// \brief Output \a e on \a ostr.
    ///
    /// Used to factor the output of the name declared,
    /// and its possible additional attributes.
    inline std::ostream& operator<<(std::ostream& ostr, const Dec& e)
    {
      ostr << e.name_get();
      //if (bindings_display(ostr))
      //  ostr << " /* " << &e << " */";
      return ostr;
    }
  } // namespace

  PrettyPrinter::PrettyPrinter(std::ostream& ostr)
    : ostr_(ostr)
    , bindings_display_(ostr.iword(bindings_display.index())) //ignore doesnt work
  {}
 
  void PrettyPrinter::print_binding(const Ast* e)
  {
    //std::cerr << "print_binding called with e=" << e 
    //<< ", bindings_display=" << bindings_display(ostr_)
    //<< ", bind::bindings_computed=" << bind::bindings_computed << std::endl;
    
    if (bindings_display(ostr_)  && bind::bindings_computed)
    {
      ostr_ << " /* " << e << " */";
    }
  }


  void PrettyPrinter::operator()(const SimpleVar& e)
  {
    ostr_ << e.name_get();
    print_binding(e.def_get());
  }

  void PrettyPrinter::operator()(const FieldVar& e)
  {
    // FIXME: Some code was deleted here.
    ostr_ << e.var_get() << '.' << e.name_get();
  }

  /* Foo[10]. */
  void PrettyPrinter::operator()(const SubscriptVar& e)
  {
    ostr_ << e.var_get() << '[' << misc::incindent << e.index_get()
          << misc::decindent << ']';
  }

  void PrettyPrinter::operator()(const CastExp& e)
  {
    ostr_ << "_cast(" << e.exp_get() << ", " << e.ty_get() << ')';
  }

  // FIXME: Some code was deleted here.
  // our code
  void PrettyPrinter::operator()(const IntExp& e) { ostr_ << e.value_get(); }

  void PrettyPrinter::operator()(const StringExp& e)
  {
    ostr_ << '"' << misc::escape(e.value_get()) << '"';
  }

  void PrettyPrinter::operator()(const OpExp& e)
  {
    ostr_ << '(' << e.left_get() << ' ' << str(e.oper_get()) << ' '
          << e.right_get() << ')';
  }

  void PrettyPrinter::operator()(const NilExp&) { ostr_ << "nil"; }

  void PrettyPrinter::operator()(const BreakExp& e) { 
    ostr_ << "break";
    print_binding(&e); 
  }

  void PrettyPrinter::operator()(const LetExp& e)
  {
    ostr_ << "let" << misc::incendl;
    ostr_ << e.chunks_get() << misc::decendl;
    ostr_ << "in" << misc::incendl;
    ostr_ << e.body_get() << misc::decendl;
    ostr_ << "end";
  }
  void PrettyPrinter::operator()(const WhileExp& e)
  {
    ostr_ << "while " << e.test_get() << " do" << misc::incendl;
    ostr_ << e.body_get() << misc::decendl;
  }
  void PrettyPrinter::operator()(const ForExp& e)
  {
    ostr_ << "for " << e.vardec_get().name_get();
    if (e.vardec_get().init_get())
      ostr_ << " := " << *e.vardec_get().init_get();
    ostr_ << " to " << e.hi_get() << " do" << misc::incendl;
    ostr_ << e.body_get() << misc::decendl;
  }

  void PrettyPrinter::operator()(const IfExp& e)
  {
    ostr_ << "if " << e.test_get() << " then" << misc::incendl;

    if (&e.thenclause_get())
      ostr_ << e.thenclause_get();
    ostr_ << misc::decendl;
    const Exp* elseclause = &e.elseclause_get();
    if (elseclause) {
      const SeqExp* seq = dynamic_cast<const SeqExp*>(elseclause);
      if (!(seq && seq->exps_get().empty())) {
        ostr_ << "else" << misc::incendl;
        ostr_ << *elseclause << misc::decendl;
      }
    }
  }

  // new 26-03
  void PrettyPrinter::operator()(const ObjectExp& e)
  {
    ostr_ << "new " << e.type_name_get();
  }

  void PrettyPrinter::operator()(const MethodCallExp& e)
  {
    ostr_ << e.object_get() << "." << e.name_get() << "(";
    const auto& args = e.args_get();
    if (!args.empty())
      ostr_ << misc::separate(args, ", ");
      
    ostr_ << ")";
    print_binding(e.def_get());
    
}

  void PrettyPrinter::operator()(const ClassTy& e)
  {
    ostr_ << "class";
    if (e.super_get().name_get() != "Object")
      ostr_ << " extends " << e.super_get();
    ostr_ << " {" << misc::incendl;
    ostr_ << e.chunks_get() << misc::decendl << "}";
  }

  void PrettyPrinter::operator()(const MethodDec& e)
  {
    ostr_ << "method " << e.name_get() << "(";

    // print parameters
    bool first = true;
    for (auto& param : e.formals_get())
      {
        if (!first)
          ostr_ << ", ";
        first = false;
        ostr_ << param->name_get();
        if (param->type_name_get())
          ostr_ << " : " << param->type_name_get()->name_get();
      }
    ostr_ << ")";

    if (e.result_get())
      ostr_ << " : " << e.result_get()->name_get();

    if (e.body_get())
      ostr_ << " = " << *e.body_get();
  }

  void PrettyPrinter::operator()(const MethodChunk& e)
  {
    for (const MethodDec* dec : e)
      {
        (*this)(*dec);
        ostr_ << misc::iendl;
      }
  }

  void PrettyPrinter::operator()(const ArrayExp& e)
  {
    ostr_ << e.type_name_get() << "[" << e.size_get() << "] of "
          << e.init_get();
  }

  void PrettyPrinter::operator()(const ArrayTy& e)
  {
    ostr_ << "array of " << e.base_type_get();
  }

  void PrettyPrinter::operator()(const AssignExp& e)
  {
    ostr_ << e.var_get() << " := " << e.exp_get();
  }

  void PrettyPrinter::operator()(const CallExp& e)
  {
    ostr_ << e.name_get();
    print_binding(e.def_get());
    ostr_ << "(";
    
    // Just check if the vector is empty
    const auto& args = e.args_get();
    if (!args.empty())
      ostr_ << misc::separate(args, ", ");
      
    ostr_ << ")";
    
  }

  void PrettyPrinter::operator()(const ChunkList& e)
  {
    if (e.chunks_get().empty())
      return;

      //std::cerr << "debug: ChunkList has " << e.chunks_get().size() << " chunks\n";
      for (const ChunkInterface* chunk : e.chunks_get())
    {
      //std::cerr << "Debug chunk of type " << typeid(*chunk).name() << "\n";
      ostr_ << *chunk;
    }
  }

  void PrettyPrinter::operator()(const Field& e)
  {
    ostr_ << e.name_get() << " : " << e.type_name_get();
  }

  void PrettyPrinter::operator()(const FieldInit& e)
  {
    ostr_ << e.name_get() << " = " << e.init_get();
  }

  void PrettyPrinter::operator()(const FunctionDec& e)
  {// more spacing better than too little spacing i would say 
    if (e.body_get())
      ostr_ << "function " << e.name_get();
    else 
      ostr_ << "primitive " << e.name_get();

 /*   
       var me / 0x5632899ae040 / := 0
       function id / 0x5632899adbe0 /(me / 0x5632899add20 / : int /* 0 /) : int /* 0 / =
       me /* 0x5632899add20 /
   in

*/
    print_binding(&e); 
    
    ostr_ << "(";
    
    bool first = true;
    for (const auto* param : e.formals_get())
    {
      if (!first)
        ostr_ << ", ";
      first = false;
      ostr_ << param->name_get();
      if (param->type_name_get())
      {
        print_binding(param);
        ostr_ << " : " << param->type_name_get()->name_get();
        print_binding(param->type_name_get()->def_get());
      }
    }
    ostr_ << ")";
    
    if (e.result_get())
    {
      ostr_ << " : " << e.result_get()->name_get();
      print_binding(e.result_get()->def_get());
    }
  
    // Print body
    if (e.body_get())
      ostr_ << " =" << misc::incendl << *e.body_get() << misc::decendl;
  }

  void PrettyPrinter::operator()(const FunctionChunk& e)
  {
    //std::cerr << "Debug: FunctionChunk contains " << e.decs_get().size() << " decs\n";
    for (const FunctionDec* dec : e)
      {
        //std::cerr << "Debug: Printing FunctionDec: " << dec->name_get() << "\n";
        (*this)(*dec);
        ostr_ << misc::iendl;
      }
  }

  void PrettyPrinter::operator()(const NameTy& e) { 
    ostr_ << e.name_get(); 
    print_binding(e.def_get());
  }

  void PrettyPrinter::operator()(const RecordExp& e)
  {
    ostr_ << e.type_name_get() << "{";
    if (!e.fields_get().empty())
      ostr_ << misc::separate(e.fields_get(), ", ");
    ostr_ << "}";
  }

  void PrettyPrinter::operator()(const RecordTy& e)
  {
    ostr_ << "{";
    if (!e.fields_get().empty())
      ostr_ << misc::separate(e.fields_get(), ", ");
    ostr_ << "}";
  }

  void PrettyPrinter::operator()(const SeqExp& e)
  {
    if (e.exps_get().empty()) {
      ostr_ << "()";
      return;
    }
    ostr_ << "(" << misc::incendl;
    bool first = true;
    for (auto exp : e.exps_get()) {
      if (exp) {  // Check if exp is not null
        if (!first)
          ostr_ << ";" << misc::iendl;
        first = false;
        ostr_ << *exp;
      }
    }
    ostr_ << misc::decendl << ")";
  }

  void PrettyPrinter::operator()(const TypeDec& e)
  {
    ostr_ << "type " << e.name_get();
    print_binding(&e); 
    ostr_<< " = " << e.ty_get();
  }

  void PrettyPrinter::operator()(const TypeChunk& e)
  {
    for (const TypeDec* dec : e)
      {
        (*this)(*dec);
        ostr_ << misc::iendl;
      }
  }

  void PrettyPrinter::operator()(const VarDec& e)
  {
    ostr_ << "var " << e.name_get();
    print_binding(&e); 
    if (e.type_name_get())
      ostr_ << " : " << e.type_name_get()->name_get();
    if (e.init_get())
      ostr_ << " := " << *e.init_get();
  }

  void PrettyPrinter::operator()(const VarChunk& e)
  {
    for (const VarDec* dec : e)
      {
        (*this)(*dec);
        ostr_ << misc::iendl;
      }
  }



} // namespace ast/**
 ** \file ast/pretty-printer.cc
 ** \brief Implementation of ast::PrettyPrinter.
 */

 #include <ast/all.hh>
 #include <ast/libast.hh>
 #include <ast/pretty-printer.hh>
 #include <ast/escapable.hh>
 
 #include <misc/escape.hh>
 #include <misc/indent.hh>
 #include <misc/separator.hh>
 
 #include <type/class.hh>
 
 namespace ast
 {
 
 void print_escaping(std::ostream& ostr, const Ast* node)
 {
   if (!node || !escapes_display(ostr))
     return;
 
   if (const auto* esc = dynamic_cast<const Escapable*>(node); esc && esc->escaped())
     ostr << " /* escapes */";
 }
 
 /*===============================================================*/
 /*                    Helpers privés au fichier                  */
 /*===============================================================*/
 namespace
 {
   inline std::ostream& operator<<(std::ostream& ostr, const Escapable& e)
   {
     if (escapes_display(ostr) && e.escaped())
       ostr << "/* escapes */ ";
     return ostr;
   }
 
   inline std::ostream& operator<<(std::ostream& ostr, const Dec& e)
   {
     ostr << e.name_get();
     if (bindings_display(ostr))
       ostr << " /* " << &e << " */";
     return ostr;
   }
 } // namespace
 
 /*===============================================================*/
 /*                    Pretty-Printer implémenté                  */
 /*===============================================================*/
 
 PrettyPrinter::PrettyPrinter(std::ostream& ostr)
   : ostr_(ostr)
   , bindings_display_(ostr.iword(bindings_display.index()) != 0)
 {}
 
 void PrettyPrinter::print_binding(const Ast* e)
 {
   if (bindings_display_ && e)
     ostr_ << " /* " << e << " */";
 }
 
 /*--------------------------  Vars  ----------------------------*/
 
 void PrettyPrinter::operator()(const SimpleVar& e)
 {
   ostr_ << e.name_get();
   if (bindings_display(ostr_))
     ostr_ << " /* " << e.def_get() << " */";
 }
 
 void PrettyPrinter::operator()(const FieldVar& e)
 {
   ostr_ << e.var_get() << '.' << e.name_get();
 }
 
 void PrettyPrinter::operator()(const SubscriptVar& e)
 {
   ostr_ << e.var_get() << '[' << misc::incindent << e.index_get()
         << misc::decindent << ']';
 }
 
 /*-------------------------  Exps  ------------------------------*/
 
 void PrettyPrinter::operator()(const CastExp& e)
 {
   ostr_ << "_cast(" << e.exp_get() << ", " << e.ty_get() << ')';
 }
 
 void PrettyPrinter::operator()(const IntExp& e)      { ostr_ << e.value_get(); }
 void PrettyPrinter::operator()(const StringExp& e)   { ostr_ << '"' << misc::escape(e.value_get()) << '"'; }
 void PrettyPrinter::operator()(const NilExp&)        { ostr_ << "nil"; }
 void PrettyPrinter::operator()(const BreakExp&)      { ostr_ << "break"; }
 
 void PrettyPrinter::operator()(const OpExp& e)
 {
   ostr_ << '(' << e.left_get() << ' ' << str(e.oper_get()) << ' '
         << e.right_get() << ')';
 }
 
 /*---------------------  Structures  ----------------------------*/
 
 void PrettyPrinter::operator()(const LetExp& e)
 {
   ostr_ << "let" << misc::incendl
         << e.chunks_get() << misc::decendl
         << "in"  << misc::incendl
         << e.body_get()  << misc::decendl
         << "end";
 }
 
 void PrettyPrinter::operator()(const WhileExp& e)
 {
   ostr_ << "while " << e.test_get() << " do" << misc::incendl
         << e.body_get() << misc::decendl;
 }
 
 void PrettyPrinter::operator()(const ForExp& e)
 {
   ostr_ << "for " << e.vardec_get().name_get();
   if (e.vardec_get().init_get())
     ostr_ << " := " << *e.vardec_get().init_get();
   ostr_ << " to " << e.hi_get() << " do" << misc::incendl
         << e.body_get() << misc::decendl;
 }
 
 void PrettyPrinter::operator()(const IfExp& e)
 {
   ostr_ << "if " << e.test_get() << " then" << misc::incendl
         << e.thenclause_get() << misc::decendl;
 
   const Exp* elseclause = &e.elseclause_get();
   if (elseclause)
   {
     const auto* seq = dynamic_cast<const SeqExp*>(elseclause);
     if (!(seq && seq->exps_get().empty()))
     {
       ostr_ << "else" << misc::incendl
             << *elseclause << misc::decendl;
     }
   }
 }
 
 /*--------------------  OO extensions  --------------------------*/
 
 void PrettyPrinter::operator()(const ObjectExp& e)
 {
   ostr_ << "new " << e.type_name_get();
 }
 
 void PrettyPrinter::operator()(const MethodCallExp& e)
 {
   ostr_ << e.object_get() << '.' << e.name_get() << '(';
   if (!e.args_get().empty())
     ostr_ << misc::separate(e.args_get(), ", ");
   ostr_ << ')';
   print_binding(e.def_get());
 }
 
 /*--------------------------  Class  ----------------------------*/
 
 void PrettyPrinter::operator()(const ClassTy& e)
 {
   ostr_ << "class";
   if (e.super_get().name_get() != "Object")
     ostr_ << " extends " << e.super_get();
   ostr_ << " {" << misc::incendl
         << e.chunks_get() << misc::decendl
         << '}';
 }
 
 /*-----------------------  Méthodes  ----------------------------*/
 
 void PrettyPrinter::operator()(const MethodDec& e)
 {
   print_escaping(ostr_, &e);
 
   ostr_ << "method " << e.name_get() << '(';
   bool first = true;
   for (auto* param : e.formals_get())
   {
     if (!first) ostr_ << ", ";
     first = false;
     print_escaping(ostr_, param);
     ostr_ << param->name_get();
     if (param->type_name_get())
       ostr_ << " : " << param->type_name_get()->name_get();
   }
   ostr_ << ')';
 
   if (e.result_get())
     ostr_ << " : " << e.result_get()->name_get();
   if (e.body_get())
     ostr_ << " = " << *e.body_get();
 }
 
 void PrettyPrinter::operator()(const MethodChunk& e)
 {
   for (const MethodDec* dec : e)
   {
     (*this)(*dec);
     ostr_ << misc::iendl;
   }
 }
 
 /*-------------------------  Arrays  ----------------------------*/
 
 void PrettyPrinter::operator()(const ArrayExp& e)
 {
   ostr_ << e.type_name_get() << '[' << e.size_get() << "] of "
         << e.init_get();
 }
 
 void PrettyPrinter::operator()(const ArrayTy& e)
 {
   ostr_ << "array of " << e.base_type_get();
 }
 
 /*---------------------  Affectations  --------------------------*/
 
 void PrettyPrinter::operator()(const AssignExp& e)
 {
   ostr_ << e.var_get() << " := " << e.exp_get();
 }
 
 /*-------------------  Appel de fonction  -----------------------*/
 
 void PrettyPrinter::operator()(const CallExp& e)
 {
   ostr_ << e.name_get() << '(';
   if (!e.args_get().empty())
     ostr_ << misc::separate(e.args_get(), ", ");
   ostr_ << ')';
   print_binding(e.def_get());
 }
 
 /*--------------------  Chunk / liste  --------------------------*/
 
 void PrettyPrinter::operator()(const ChunkList& e)
 {
   for (const ChunkInterface* chunk : e.chunks_get())
     ostr_ << *chunk;
 }
 
 /*---------------------  Little nodes  --------------------------*/
 
 void PrettyPrinter::operator()(const Field& e)
 {
   ostr_ << e.name_get() << " : " << e.type_name_get();
 }
 
 void PrettyPrinter::operator()(const FieldInit& e)
 {
   ostr_ << e.name_get() << " = " << e.init_get();
 }
 
 /*------------------------  Fonctions  --------------------------*/
 
 void PrettyPrinter::operator()(const FunctionDec& e)
 {
   print_escaping(ostr_, &e);
 
   ostr_ << "function " << e.name_get() << '(';
   bool first = true;
   for (auto* param : e.formals_get())
   {
     if (!first) ostr_ << ", ";
     first = false;
     print_escaping(ostr_, param);
     ostr_ << param->name_get();
     if (param->type_name_get())
       ostr_ << " : " << param->type_name_get()->name_get();
   }
   ostr_ << ')';
 
   if (e.result_get())
     ostr_ << " : " << e.result_get()->name_get();
   if (e.body_get())
     ostr_ << " = " << *e.body_get();
 }
 
 void PrettyPrinter::operator()(const FunctionChunk& e)
 {
   for (const FunctionDec* dec : e)
   {
     (*this)(*dec);
     ostr_ << misc::iendl;
   }
 }
 
 /*--------------------------  Types  ----------------------------*/
 
 void PrettyPrinter::operator()(const NameTy& e)
 {
   ostr_ << e.name_get();
   print_binding(e.def_get());
 }
 
 void PrettyPrinter::operator()(const RecordExp& e)
 {
   ostr_ << e.type_name_get() << '{';
   if (!e.fields_get().empty())
     ostr_ << misc::separate(e.fields_get(), ", ");
   ostr_ << '}';
 }
 
 void PrettyPrinter::operator()(const RecordTy& e)
 {
   ostr_ << '{';
   if (!e.fields_get().empty())
     ostr_ << misc::separate(e.fields_get(), ", ");
   ostr_ << '}';
 }
 
 void PrettyPrinter::operator()(const SeqExp& e)
 {
   ostr_ << '(';
   bool first = true;
   for (auto* exp : e.exps_get())
   {
     if (!exp) continue;
     if (!first) ostr_ << ';' << misc::iendl;
     first = false;
     ostr_ << *exp;
   }
   ostr_ << ')';
 }
 
 void PrettyPrinter::operator()(const TypeDec& e)
 {
   ostr_ << "type " << e.name_get() << " = " << e.ty_get();
 }
 
 void PrettyPrinter::operator()(const TypeChunk& e)
 {
   for (const TypeDec* dec : e)
   {
     (*this)(*dec);
     ostr_ << misc::iendl;
   }
 }
 
 /*------------------------  Variables  --------------------------*/
 
 void PrettyPrinter::operator()(const VarDec& e)
 {
   print_escaping(ostr_, &e);
 
   ostr_ << "var " << e.name_get();
   if (e.type_name_get())
     ostr_ << " : " << e.type_name_get()->name_get();
   if (e.init_get())
     ostr_ << " := " << *e.init_get();
 }
 
 void PrettyPrinter::operator()(const VarChunk& e)
 {
   for (const VarDec* dec : e)
   {
     (*this)(*dec);
     ostr_ << misc::iendl;
   }
 }
 
 } // namespace ast
 