package com.moon.scope.impl

import com.moon.scope.{BaseScope, TinaScope}

/**
  * Created by Lin on 17/4/19.
  */
case class MethodScope(enclosingScope: TinaScope,scopeName:String)extends BaseScope(enclosingScope){
  /**
    * 获取当前作用域的名称
    *
    * @return
    */
  override def getScopeName(): String =scopeName
}
