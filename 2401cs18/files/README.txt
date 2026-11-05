Build and run (Bison/Flex or Yacc/Lex):

Example 1:
  cd example1
  bison -y -d trans_v1.y
  flex exp_v1.l
  gcc lex.yy.c y.tab.c -o adder
  ./adder            (type: 5+8;)

Example 2:
  cd example2
  bison -y -d calc.y
  flex calc.l
  gcc lex.yy.c y.tab.c -o calc
  ./calc             (type: a=10; print a; exit;)
  ./calc < text      (runs the file-input demo)
