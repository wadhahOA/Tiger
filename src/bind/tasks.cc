/**
 ** \file bind/tasks.cc
 ** \brief Bind module tasks implementation.
 */

 #include <ast/tasks.hh>
 #include <bind/binder.hh>
 #include <common.hh>
 #define DEFINE_TASKS 1
 #include <bind/tasks.hh>
 #undef DEFINE_TASKS
 
 namespace bind::tasks
 {


   /// The routine executed by the bindings-compute task.
   //https://assignments.lrde.epita.fr/source_code/project_layout/src_task.html#src-task
   //
   //
   void bindings_compute()
   {
     precondition(ast::tasks::the_program)
       << "No AST to bind -> did you parse?";
     bind::Binder binder;
     ast::tasks::the_program->accept(binder);
     task_error() << binder.error_get();
     if (binder.error_get())
       task_error().exit();
   }
//    void bind() {
//      bind::bind(*ast::tasks::the_program); }

//    void bind_display() { ast::bindings_display(std::cout) = true; }
 
//    void rename() { bind::rename(*ast::tasks::the_program); }
 } // namespace bind::tasks