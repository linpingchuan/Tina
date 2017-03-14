package com.moon.scope

import java.util.Objects

import com.moon.symbol.TinaSymbol

import scala.collection.mutable

/**
  * Created by lin on 3/14/17.
  */
abstract class BaseScope(enclosingScope: TinaScope) extends TinaScope {
  val symbols = new mutable.LinkedHashMap[String, TinaSymbol]

  /**
    * 获取临近的下一个Symbol位置
    *
    * @return
    */
  override def getEnclosingScope(): TinaScope = enclosingScope

  /**
    * 在当前作用域定义符号
    *
    * @param sym
    */
  override def define(sym: TinaSymbol): Unit = {
    symbols += (sym.name -> sym)
    sym.scope = this
  }

  /**
    * 在当前作用域查找该名字,
    * 如果找不到，则在此封闭作用域中进行查找
    *
    * @param name
    * @return
    */
  override def resolve(name: String): TinaSymbol = name
  match {
    case nonNull if Objects.nonNull(symbols.get(name).get) => symbols.get(name).get
    case isEnclosing if Objects.nonNull(enclosingScope) => enclosingScope.resolve(name)
    case _ => null
  }
}
