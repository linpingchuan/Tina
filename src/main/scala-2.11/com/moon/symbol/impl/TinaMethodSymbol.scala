package com.moon.symbol.impl

import java.util.Objects

import com.moon.scope.TinaScope
import com.moon.symbol.TinaSymbol
import com.moon.tp.TinaType

import scala.collection.mutable

/**
  * Created by lin on 3/14/17.
  */
class TinaMethodSymbol(nm: String, retType: Int, enclosingScope: TinaScope) extends TinaSymbol(nm, retType) with TinaScope {
  var orderedArgs = Map[String, TinaSymbol]()

  /**
    * 获取当前作用域的名称
    *
    * @return
    */
  override def getScopeName(): String = name

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
    orderedArgs += (sym.name -> sym)
    sym.scope=this
  }

  /**
    * 在当前作用域查找该名字,
    * 如果找不到，则在此封闭作用域中进行查找
    *
    * @param name
    * @return
    */
  override def resolve(name: String): TinaSymbol =
    name match {
      case nonEmtpy if orderedArgs.get(name).nonEmpty => orderedArgs.get(name).get
      case isInEnclosing if Objects.nonNull(getEnclosingScope()) => getEnclosingScope().resolve(name)
      case _ => null
    }

  override def toString: String = "method"+super.toString+":"+orderedArgs.values
}
