/**
 ** \file bind/libbind.cc
 ** \brief Implementation of bind::libbind.
 */

 #include <ast/all.hh>
 #include <bind/binder.hh>
 #include <bind/libbind.hh>
 #include <misc/error.hh>
 
 namespace bind
 {
   // callthis for task
   misc::error bind(ast::ChunkList& tree)
   {
     Binder binder;

     binder(tree);
     return binder.error_get();
   }
 } // namespace bind