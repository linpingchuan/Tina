package com.moon.symbol

import com.moon.scope.TinaScope
import com.moon.tp.TinaType

/**
  * 所有的符号都至少有一个名字，并且知道它们的类型以及相应包含它们的作用域
  * Created by lin on 3/14/17.
  */
case class TinaSymbol(name:String,tinaType:TinaType,var scope:TinaScope) {

  def this(nm:String){
    this(nm,null,null)
  }

  def this(nm:String,tt:TinaType){
    this(nm,tt,null)
  }

  def getName():String={
    name
  }

  override def toString: String = "<"+getName()+" : "+tinaType+">"
}
