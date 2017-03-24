package com.moon.parser.impl

import com.moon.lexer.impl.TinaLexer
import com.moon.symbol.impl.SymbolTable

/**
  * Created by lin on 3/14/17.
  */
case class TinaParser(lexer:TinaLexer,symtab:SymbolTable) {
  val tinaTokens=lexer.tokens()
  def compilationUnit(): Unit ={

  }

  def varDeclaration(): Unit ={

  }

  def expression(): Unit ={

  }
}
