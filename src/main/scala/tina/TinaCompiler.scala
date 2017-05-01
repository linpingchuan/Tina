package tina

/**
  * Created by Lin on 17/4/28.
  */
case class TinaCompiler() {

}

case class TinaToken(name: Any, kind: Int, line: Int, pos: Int) {
  override def toString: String = "name: " + name + " kind: " + TinaToken.tokenNames(kind) + " in line: " + line + " pos: " + pos
}

case class MisMatchTokenException(e: String) extends RuntimeException(e)

object TinaToken {
  val INT = 0
  val FLOAT = 1
  val LEFT_PARENT = 2
  val RIGHT_PARENT = 3
  val LEFT_BRACE = 4
  val RIGHT_BRACE = 5
  val TINA = 6
  val LOVE = 7
  val STRING = 8
  val ADD = 9
  val SUB = 10
  val MUL = 11
  val DIV = 12
  val WHILE = 13
  val UNIT = 14
  val RETURN = 15
  val FOR = 16
  val IF = 17
  val ELSE = 18
  val ELIF = 19
  val DOUBLE_QUOTATION = 20
  val SINGLE_QUOTATION = 21
  val CLASS = 22
  val COLON = 23
  val VAR = 24
  val EQUAL = 25
  val FUNCTION = 26
  val tokenNames: List[String] = List[String](
    "int", "float", "(", ")", "{",
    "}", "tina", "love", "string", "+",
    "-", "*", "/", "while", "unit",
    "return", "for", "if", "else", "elif",
    "\"", "'", "class", ":", "var",
    "=", "function"
  )
}

case class TinaLexer(src: Array[Char]) {
  var index: Int = 0
  var lineIndex: Int = 0
  var inferIndexs: List[(Int, Int)] = List[(Int, Int)]()

  var isInfer: Boolean = false
  var isComment: Boolean = false

  var buffer: List[TinaToken] = List[TinaToken]()
  var line: Int = 1

  def matchToken(kind: Int): TinaToken = {
    null
  }

  def skip(): Unit = index match {
    case lt if index < src.length =>
      val char = src(index)
      char match {
        case isSkip if (char == '\t' || char == ' ' || char == '\r') && (!isComment) =>
          index = index + 1
          lineIndex = lineIndex + 1
          skip()

        case isEscaped if char == '\\' =>
          if (index + 2 < src.length)
            index = index + 2
          lineIndex = lineIndex + 2
          skip()

        case isBeginComment if char == '/' && (index + 1) < src.length && src(index + 1) == '*' =>
          index = index + 2
          lineIndex = lineIndex + 2
          isComment = true
          skip()

        case isEndComment if isComment && char == '*' && (index + 1) < src.length && src(index + 1) == '/' =>
          index = index + 2
          lineIndex = lineIndex + 2
          isComment = false
          skip()

        case isInComment if isComment =>
          index = index + 1
          lineIndex = lineIndex + 1
          skip()

        case _ =>
      }

    case _ =>

  }

  def hasNumber(): Boolean = {
    var result = false
    if (index < src.length) {
      val char = src(index)
      char match {
        case isNum if char >= '0' && char <= '9' => result = true
        case _ => result = false
      }
    }
    result
  }

  def hasLetter(): Boolean = {
    var result = false
    if (index < src.length) {
      val char = src(index)
      char match {
        case isTrue if (char >= 'a' && char <= 'z') || (char >= 'A' && char <= 'Z') || char == '_' || (char >= '0' && char <= '9') => result = true
        case _ => result = false
      }
    }
    result
  }

  def nextLetters(token: TinaToken): TinaToken = if (hasLetter()) {
    val name = token.name.asInstanceOf[String] + src(index)
    index = index + 1
    lineIndex = lineIndex + 1
    if (name == TinaToken.tokenNames(TinaToken.IF)) {
      nextLetters(TinaToken(name, TinaToken.IF, line, token.pos + 1))
    } else if (name == TinaToken.tokenNames(TinaToken.LOVE)) {
      nextLetters(TinaToken(name, TinaToken.LOVE, line, token.pos + 1))
    } else if (name == TinaToken.tokenNames(TinaToken.TINA)) {
      nextLetters(TinaToken(name, TinaToken.TINA, line, token.pos + 1))
    }
    else if (src(index).toString == TinaToken.tokenNames(TinaToken.LEFT_PARENT)) {
      nextLetters(TinaToken(name, TinaToken.FUNCTION, line, token.pos + 1))
    }
    else {
      nextLetters(TinaToken(name, TinaToken.VAR, line, token.pos + 1))
    }
  } else token


  def nextNumber(token: TinaToken, dotNum: Int): TinaToken = {
    var result = token
    if (index < src.length) {
      if (src(index) == '.' && token.kind == TinaToken.INT) {
        index = index + 1
        lineIndex = lineIndex + 1
        result = nextNumber(TinaToken(token.name.asInstanceOf[Int].toDouble, TinaToken.FLOAT, token.line, token.pos + 1), dotNum)
      }
      else if (hasNumber()) {
        val int = src(index).toInt - '0'.toInt
        index = index + 1
        lineIndex = lineIndex + 1
        if (token.kind == TinaToken.INT)
          result = nextNumber(TinaToken(token.name.asInstanceOf[Int] * 10 + int, token.kind, token.line, token.pos + 1), dotNum)
        else {

          var dotResult = int.toDouble
          for (i <- 0 to dotNum)
            dotResult = dotResult / 10

          result = nextNumber(TinaToken(token.name.asInstanceOf[Double] + dotResult, token.kind, token.line, token.pos + 1), dotNum + 1)

        }
      }
      else if (src(index - 1) == '.' && token.kind == TinaToken.FLOAT)
        throw MisMatchTokenException("expecting original type is int but found float.Is double dot in this number :) ")
    }
    result
  }


  def hasSpecialLetter(): Boolean = {
    var result = false
    if (index < src.length) {
      val char = src(index)
      char match {
        case isSpecial if char == '(' || char == ')' || char == '{' || char == '}' => result = true
        case _ =>
      }
    }
    result
  }

  def nextSpecialLetter(): TinaToken = if (hasSpecialLetter()) {
    val name = src(index).toString
    index = index + 1
    lineIndex = lineIndex + 1
    if (name == TinaToken.tokenNames(TinaToken.LEFT_PARENT)) {
      TinaToken(name, TinaToken.LEFT_PARENT, line, lineIndex)
    } else if (name == TinaToken.tokenNames(TinaToken.RIGHT_PARENT)) {
      TinaToken(name, TinaToken.RIGHT_PARENT, line, lineIndex)
    } else if (name == TinaToken.tokenNames(TinaToken.LEFT_BRACE)) {
      TinaToken(name, TinaToken.LEFT_BRACE, line, lineIndex)
    } else if (name == TinaToken.tokenNames(TinaToken.RIGHT_BRACE)) {
      TinaToken(name, TinaToken.RIGHT_BRACE, line, lineIndex)
    } else if (name == TinaToken.tokenNames(TinaToken.EQUAL)) {
      TinaToken(name, TinaToken.EQUAL, line, lineIndex)
    }
    else
      throw MisMatchTokenException("expecting special letter but not found")
  }
  else throw MisMatchTokenException("expecting special letter but not found")

  def nextToken(): TinaToken = {
    var token: TinaToken = null
    if (index < src.length) {
      val char = src(index)
      char match {
        case skipToken if char == '\t' || char == ' ' || char == '\r' || char == '/' =>
          skip()
          val flag = isInfer
          isInfer = false
          token = nextToken()
          isInfer = flag

        case nextLine if char == '\n' =>
          lineIndex = 0
          line = line + 1
          index = index + 1
          val flag = isInfer
          isInfer = false
          token = nextToken()
          isInfer = flag

        case isNum if hasNumber() =>
          token = nextNumber(TinaToken(0, TinaToken.INT, line, lineIndex - 1), 0)

        case isLetters if hasLetter() =>
          token = nextLetters(TinaToken("", TinaToken.VAR, line, lineIndex))

        case isSpecialLetters if hasSpecialLetter() =>
          token = nextSpecialLetter()

        case _ =>
          throw MisMatchTokenException(char + " can not match")

      }
    }


    token
  }

  /**
    * 向前同步N个token，有利于判断该Token是哪个Token
    *
    * @param size 同步的个数
    */
  def syn(size: Int): Unit = {
    isInfer = true
    if (isInfer)
      inferIndexs = (index, lineIndex) +: inferIndexs
    for (i <- 0 until size) {
      buffer = buffer :+ nextToken()
    }
    if (isInfer) {
      index = inferIndexs.head._1
      lineIndex = inferIndexs.head._2
      inferIndexs = inferIndexs.slice(1, inferIndexs.length)
    }
    isInfer = false

  }
}

case class TinaState(char){

}
object TinaState{
  val state1:List[TinaState]=List[TinaState](

  )
}
case class TinaStateMachine(){

}
case class TinaParser(lexer: TinaLexer) {

}