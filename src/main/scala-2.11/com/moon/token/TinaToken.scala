package com.moon.token

import com.moon.config.AppConfig

/**
  * Created by lin on 3/14/17.
  */
case class TinaToken(input: Any, tinaType: Int) {
  override def toString: String = "< type > : "+AppConfig.tokenNames(tinaType)+" -> "+input
}

object TinaToken {

}