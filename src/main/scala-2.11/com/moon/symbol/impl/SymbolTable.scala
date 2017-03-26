package com.moon.symbol.impl

import com.moon.scope.TinaScope
import com.moon.scope.impl.GlobalScope
import com.moon.symbol.TinaSymbol

/**
  * Created by lin on 3/14/17.
  */
case class SymbolTable() extends TinaScope{
  var symbols=Map[String,TinaSymbol]()

  val globals=new GlobalScope
  initTypeSystem()


  def initTypeSystem(): Unit ={
    globals.define(new BuiltInTypeSymbol("love"))
  }

  override def toString: String = globals.toString

  /**
    * 获取当前作用域的名称
    *
    * @return
    */
  override def getScopeName(): String = "global"

  /**
    * 获取临近的下一个Symbol位置
    *
    * @return
    */
  override def getEnclosingScope(): TinaScope = null

  /**
    * 在当前作用域定义符号
    *
    * @param sym
    */
  override def define(sym: TinaSymbol): Unit = symbols+=(sym.name -> sym)

  /**
    * 在当前作用域查找该名字,
    * 如果找不到，则在此封闭作用域中进行查找
    *
    * @param name
    * @return
    */
  override def resolve(name: String): TinaSymbol = symbols.get(name).get
}
