/**
 ** \file bind/libbind.hh
 ** \brief Interface of the bind module.
 */

 #pragma once

 #include <misc/error.hh>

 namespace ast
 {
   class ChunkList;
 }
 
 namespace bind
 {
   misc::error bind(ast::ChunkList& tree);
 
 } // namespace bind
// what