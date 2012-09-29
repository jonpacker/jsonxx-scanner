#!/usr/bin/env python

def set_options(context):
  context.tool_options('compiler_cxx')

def configure(context):
  context.check_tool('compiler_cxx')
  context.check_tool('node_addon')
  context.env.set_variant('Release')

def build(context):
  task = context.new_task_gen('cxx', 'shlib', 'node_addon')
  task.target = 'jsonxx_scanner'
  task.source = ['scanner.cc', 'jsonxx/util.cpp', 'jsonxx/scanner.cpp', 'jsonxx/scanner_listener.cpp', 'jsonxx/scanner_listener_stack.cpp']
