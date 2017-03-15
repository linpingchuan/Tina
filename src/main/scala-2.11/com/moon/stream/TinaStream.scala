package com.moon.stream

/**
  * Created by lin on 3/15/17.
  */
trait TinaStream {
  def consume()

  def reset()

  def lookAhead(index:Int):Int

  def lookAheadType(index:Int):Int

  def index():Int

  def release(marker:Int)

  def mark():Int

  def seek(index:Int)
}

object TinaStream{
  val EOF:Int= -1

}
