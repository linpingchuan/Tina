package com.moon.symbol.impl

import com.moon.scope.impl.GlobalScope

/**
  * Created by lin on 3/14/17.
  */
case class SymbolTable() {
  val globals=new GlobalScope
  initTypeSystem()


  def initTypeSystem(): Unit ={
    globals.define(new BuiltInTypeSymbol("int"))
    globals.define(new BuiltInTypeSymbol("float"))
    globals.define(new BuiltInTypeSymbol("void"))
  }

  override def toString: String = globals.toString
}
