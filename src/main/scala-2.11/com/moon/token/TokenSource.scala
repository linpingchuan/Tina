package com.moon.token

/**
  * Created by lin on 3/14/17.
  */
trait TokenSource {
  /**
    * 返回下一个TinaToken
    * @return
    */
  def nextToken(): TinaToken

  /**
    * 获取Token源的名称
    * @return
    */
  def getSourceName(): String
}
