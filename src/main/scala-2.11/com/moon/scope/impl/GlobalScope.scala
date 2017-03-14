package com.moon.scope.impl

import com.moon.scope.BaseScope

/**
  * Created by lin on 3/14/17.
  */
case class GlobalScope() extends BaseScope(null){
  /**
    * 获取当前作用域的名称
    *
    * @return
    */
  override def getScopeName(): String = "global"
}
