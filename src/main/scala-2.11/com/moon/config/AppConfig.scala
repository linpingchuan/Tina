package com.moon.config

/**
  * 编译器配置
  * Created by lin on 3/14/17.
  */
object AppConfig {
  val tokenNames:Array[String]=Array(
    "n/a","VAR","Int","+","print",","
  )
  val EOF= -1
  val VARIABLE=1
  val INTEGER=2
  val ADD=3
  val PRINT=4
  val COMMA=5
}
