package com.moon.scope.impl

import com.moon.scope.{TinaScope, BaseScope}

/**
  * Created by lin on 3/14/17.
  */
case class LocalScope(parent:TinaScope) extends BaseScope(parent){
  /**
    * 获取当前作用域的名称
    *
    * @return
    */
  override def getScopeName(): String =  "local"
}
