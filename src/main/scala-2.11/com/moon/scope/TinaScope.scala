package com.moon.scope

import com.moon.symbol.TinaSymbol

/**
  * Created by lin on 3/14/17.
  */
trait TinaScope {
  /**
    * 获取当前作用域的名称
    *
    * @return
    */
  def getScopeName(): String

  /**
    * 获取临近的下一个Symbol位置
    *
    * @return
    */
  def getEnclosingScope(): TinaScope

  /**
    * 在当前作用域定义符号
    *
    * @param sym
    */
  def define(sym: TinaSymbol): Unit

  /**
    * 在当前作用域查找该名字,
    * 如果找不到，则在此封闭作用域中进行查找
    *
    * @param name
    * @return
    */
  def resolve(name: String): TinaSymbol
}
