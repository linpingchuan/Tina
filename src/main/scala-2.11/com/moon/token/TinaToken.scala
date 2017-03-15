package com.moon.token

/**
  * Created by lin on 3/14/17.
  */
case class TinaToken(input: String, tinaType: Int, var channel: Int = TinaToken.DEFAULT_TOKEN_CHANNEL, var start: Int, var stop: Int) {
  var text: String = _
  var line: Int = _
  var charPositionInLine: Int = -1
  var index: Int = -1


  def this(tt: Int) {
    this(null, tt, TinaToken.DEFAULT_TOKEN_CHANNEL, 0, 0)
  }

  def this(tt: Int, txt: String) {
    this(tt)
    text = txt
  }

  def this(oldToken: TinaToken) {
    this(null, oldToken.tinaType, oldToken.channel, oldToken.start, oldToken.stop)
    text = oldToken.text
    line = oldToken.line
    index = oldToken.index
    charPositionInLine = oldToken.charPositionInLine
    channel = oldToken.channel
    oldToken match {
      case _ if oldToken.isInstanceOf[TinaToken] => {
        start = oldToken.start
        stop = oldToken.stop
      }
    }

  }
}

object TinaToken {
  val EOF = -1
  val INVALID_TOKEN_TYPE = 0
  val DEFAULT_TOKEN_CHANNEL = 0
  val EOR_TOKEN_TYPE = 1
  val DOWN = 2
  val UP = 3


  val EOF_TOKEN = new TinaToken(EOF)
  val INVALID_TOKEN = new TinaToken(INVALID_TOKEN_TYPE)
  val SKIP_TOKEN = new TinaToken(INVALID_TOKEN_TYPE)
}