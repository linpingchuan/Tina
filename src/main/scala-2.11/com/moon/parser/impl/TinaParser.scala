package com.moon.parser.impl

import java.lang.Exception

import com.moon.config.AppConfig
import com.moon.lexer.impl.TinaLexer
import com.moon.symbol.impl.SymbolTable
import com.moon.token.TinaToken

import scala.annotation.tailrec

/**
  * Created by lin on 3/14/17.
  */
case class TinaParser(lexer: TinaLexer, symtab: SymbolTable) {
  // 栈，存放用于记录位置的位标
  val markers = List[Int]()
  // 大小可变的缓冲区
  var lookahead = List[TinaToken]()
  // 当前向前看词法单元的下标
  var index: Int = 0

  def compilationUnit(): Unit = {

  }

  def speculateVarDeclaration(): Boolean = {
    var success = true
    mark()
    try {
      varDeclaration()
    } catch {
      case e: Exception => {
        e.printStackTrace()
        success = false
      }
    }
    release()
    success
  }

  /**
    * 变量声明应该符合以下格式:
    * love a[,b]
    */
  def varDeclaration(): Unit = {
    def matchVarDeclarations(): Boolean = try {
      matchToken(AppConfig.COMMA)
      matchToken(AppConfig.NAME)
      true
    } catch {
      case e: MismatchedTokenException => false
    }
    @tailrec
    def recursiveMatch(a: () => Boolean): Unit = a() match {
      case true => recursiveMatch(a)
      case _ => Unit
    }

    matchToken(AppConfig.LOVE)
    matchToken(AppConfig.NAME)
    recursiveMatch(matchVarDeclarations)
  }

  def varAssignment(): Unit = {

  }

  def expression(): Unit = {

  }


  def consume(): Unit = {
    index += 1

    /**
      * 非推断状态，而且到达向前看缓冲区的末尾
      * 到了末尾，就该重新从0开始填入新的词法单元
      * 大小清0，回收内存
      */
    index match {
      case isNotSpeculating if index == lookahead.size && markers.size == 0 => {
        index = 0
        lookahead = List[TinaToken]()
      }
    }

    sync(1)
  }

  /**
    * 确保当前位置p之前有i个词法单元
    * 判断是否越界，如果越界获取n个词法单元
    *
    * @param i
    */
  def sync(i: Int): Unit = {
    def fill(n: Int): Unit = {
      for (f <- 1 to n)
        lookahead.+:(lexer.nextToken())
    }

    i match {
      case outOfBorder if i + index > lookahead.size => {
        val n = i + index - lookahead.size
        fill(n)
      }
    }
  }

  /**
    * 标记位标
    *
    * @return
    */
  def mark(): Int = {
    markers.+:(index)
    index
  }

  /**
    * 释放位标
    */
  def release(): Unit = {
    val marker = markers.take(markers.size - 1)
    markers.drop(markers.size - 1)
    index = marker.head
  }

  def lookaheadToken(i: Int): TinaToken = {
    sync(i)
    lookahead.take(i).head
  }

  def matchToken(x: Int): Unit = x match {
    case eqType if lookaheadToken(1).tinaType == x => consume()
    case _ => throw new MismatchedTokenException("expecting " + AppConfig.tokenNames(x) + " found ")
  }
}

case class MismatchedTokenException(e: String) extends RuntimeException(e)
