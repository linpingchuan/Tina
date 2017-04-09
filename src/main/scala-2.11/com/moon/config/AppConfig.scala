package com.moon.config

/**
  * 编译器配置
  * Created by lin on 3/14/17.
  */
object AppConfig {
  val tokenNames:Array[String]=Array(
    "n/a","NAME","Int","+","print",",","=","love","[","]","tina"
  )
  val EOF= -1
  val NAME=1
  val INTEGER=2
  val ADD=3
  val PRINT=4
  val COMMA=5
  val EQUALS=6
  val LOVE=7
  val LEFT_BRACKET=8
  val RIGHT_BRACKET=9
  val METHOD_DECL=10
}
