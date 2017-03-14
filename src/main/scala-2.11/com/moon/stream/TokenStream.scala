package com.moon.stream

import com.moon.token.TinaToken

/**
  * Created by lin on 3/14/17.
  */
trait TokenStream {
  def LT(k:Int):TinaToken

  def get(i:Int):TinaToken


}
