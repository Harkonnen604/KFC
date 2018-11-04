#ifndef basic_macros_h
#define basic_macros_h

// -----
// None
// -----
#define NONE

// ----
// NOP
// ----
#define KFC_NOP do {} while(false)

// -----------------
// Single-step loop
// -----------------
#define SINGLE_STEP_LOOP	for(bool _ = true ; _ ; _ = false)

// -----------
// Comma list
// -----------
#define COMMA_LIST0()

#define COMMA_LIST1(Token0) \
	Token0

#define COMMA_LIST2(Token0, Token1) \
	Token0, Token1

#define COMMA_LIST3(Token0, Token1, Token2) \
	Token0, Token1, Token2

#define COMMA_LIST4(Token0, Token1, Token2, Token3) \
	Token0, Token1, Token2, Token3

#define COMMA_LIST5(Token0, Token1, Token2, Token3, Token4) \
	Token0, Token1, Token2, Token3, Token4

#define COMMA_LIST6(Token0, Token1, Token2, Token3, Token4, Token5) \
	Token0, Token1, Token2, Token3, Token4, Token5

#define COMMA_LIST7(Token0, Token1, Token2, Token3, Token4, Token5, Token6) \
	Token0, Token1, Token2, Token3, Token4, Token5, Token6

#define COMMA_LIST8(Token0, Token1, Token2, Token3, Token4, Token5, Token6, Token7) \
	Token0, Token1, Token2, Token3, Token4, Token5, Token6, Token7

#endif // basic_macros_h
