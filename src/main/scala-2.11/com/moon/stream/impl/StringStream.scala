package com.moon.stream.impl

import java.util
import java.util.Objects

import com.moon.state.TinaState
import com.moon.stream.TinaStream

/**
  * Created by lin on 3/15/17.
  */
case class StringStream(data: String) extends TinaStream {
  // 字符串的大小
  var size: Int = data.size
  // 行号
  var line: Int = 1
  // 字符串在一行中所在的位置
  var charPositionInLine: Int = 0
  // 在字符串中，下个字符的位置
  var p: Int = 0
  // 最后一个的标记位所在的位置
  var lastMarker: Int = _

  var markDepth: Int = 0

  var markers:util.List[TinaState]

  override def consume(): Unit = {
    if (p < size) {
      charPositionInLine += 1
      if (data.charAt(p) == '\n') {
        line += 1
        charPositionInLine = 0
      }
      p += 1
    }
  }

  override def mark(): Int = {
    if(Objects.isNull(markers)){
      markers=new util.ArrayList[TinaState]
      markers.add(null)
    }
    markDepth += 1

  }

  override def lookAhead(index: Int): Int = index match {
    case zero if index == 0 => 0
    case lowerZero if (index < 0) && ((p + index) < 0) => TinaStream.EOF
    case greaterN if ((p + index - 1) >= size) => TinaStream.EOF
    case _ => data.charAt(p + index - 1)
  }

  override def seek(index: Int): Unit = {
    if (index <= p) {
      p = index
    } else {
      while (p < index) {
        consume()
      }
    }
  }

  override def index(): Int = p

  /**
    * 重置输入流
    */
  override def reset(): Unit = {
    p = 0
    line = 1
    charPositionInLine = 0
    markDepth = 0
  }

  override def lookAheadType(index: Int): Int = lookAhead(index)

  override def release(marker: Int): Unit = ???
}
